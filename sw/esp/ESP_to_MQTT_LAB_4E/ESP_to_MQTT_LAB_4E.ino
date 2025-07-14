// -----------------------------------------------------------------------------
//
// This code runs on the 8266 and is used to communicate to an MQTT broker. It is
// an mashup of various 8266-MQTT client routines found on the web.
//
//  This code provide the communications between the TM4C and a Web Application 
//  that controls the Lab 4 clock
//
// Author:    Mark McDermott
// Rev 7:     1/24/24
//
// ----------------------------------------------------------------
// ----------------    DEFINES   ----------------------------------
// 
// 
#define     DEBUG1                  // First level of Debug
//#define     DEBUG2                  // Second level of Debug
//#define     DEBUG3                  // Third level of Debug

#define       RDY 2

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""


// ----------------------------------------------------------------
// ---------------   INCLUDES    ----------------------------------
//

// For use with ESP8266
#include <ESP8266WiFi.h>            // WiFi drivers
#include <BlynkSimpleEsp8266.h>     // Blynk timer -- best timer for this appllication

// For use with ESP32
// #include <WiFi.h>
// #include <Blynk.h>

#include <PubSubClient.h>           // MQTT drivers

#include <stdio.h>
#include <string.h>

// ----------------------------------------------------------------
// ----------------  VARIABLES    ---------------------------------         
//
char  eid[20]           = "ntg337_pt8352";        // TODO: Update this with you EID
char  ssid[64]          = "Neilan1";       // TODO: Update this with your WIFI SSID
char  password[64]      = "neilanphebe";   // TODO: Update this with your WIFI Password

// TODO: add more of these depending on your specifications
char  clk_mode[4]       = "";
char  hour[4]           = "";
char  minute[4]         = "";
char  second[4]         = "";
char  mode3[4]          = "";

char  cmd[20];          
#define SER_BUF_LEN 256                              
char  ser_buf[SER_BUF_LEN];         


// ----------------------------------------------------------------
// -------------     UT Server MQTT Broker    ---------------------
//
const char *mqtt_username       = "";     // Not needed for this appication 
const char *mqtt_password       = "";
char        mqtt_broker[20]     = "broker.emqx.io";     // TODO: Replace if required
char        port[5]             = "1883";               // TODO: Replace if required
int         mqtt_port;

// ----------------------------------------------------------------
// --------------     Publish topics     --------------------------
//
// TODO: Update more of these depending on your specifications
const char  *pub_mode           = "/b2w/mode"; 
const char  *pub_hour           = "/b2w/hour";  
const char  *pub_min            = "/b2w/min"; 
const char  *pub_sec            = "/b2w/sec";
const char  *pub_mode3          = "/b2w/mode3";
const char  *pub_msg            = "/b2w/msg";           // Debug only 

// ----------------------------------------------------------------
// --------------     Subscribe topics     ------------------------
//
char  topic_w2b[20]             = "/w2b";                   
char  topic_subscribe[64];
char  topic_publish[64];


// ----------------------------------------------------------------
//  -------     Start services     --------------------------------
// ----------------------------------------------------------------

WiFiClient          espClient;
PubSubClient client(espClient);
BlynkTimer          timer;                // We will use the Blynk timer for scheduling serial port reads

// ----------------------------------------------------------------
//  -------     Define Safer tokenizer     ------------------------
// ----------------------------------------------------------------

//Similar to strcpy(dest, strtok(src, delim)); except without hard faults
int get_next_token(char *dest, char *src, const char *delim){
  static char* last_src;

  //Continue with old token string, if a new one is not provided.
  if(src != NULL) last_src = src;

  //Check for failure conditions
  if((dest == NULL) || (last_src == NULL)) return 1;  //Return failure if either of the source pointers are NULL  
  if(last_src[0] == 0x00) return 1;                 //Return failure if the src string is empty

  //Attempt to copy the string by character
  while(true){
    if( last_src[0] == 0x00 ){
      break;                                        //String was 'split' due to the string ending  
    } else if( strchr(delim, last_src[0]) == NULL ){
      
      dest[0] = last_src[0];                        //If a delimiter character is not in this position, copy the char
      dest++;
      last_src++;
    } else {
      dest[0] = 0x00;
      last_src[0] = 0x00;
      last_src++;                                  //Increment the position so in the next call, we are in valid spot.
      break;
    }
  }
  
  return 0; //Success!
}


// ----------------------------------------------------------------------------
// This routine sets up Wifi. First step is receive the SSID, Password and
// student EID code using CSV format. Second step is to parse it and try
// to connect to the WiFi hotspot. Once the WiFI connection is established
// we then connect to the MQTT broker
//

void Setup_Wifi(void) {

  //Create a buffer to handle commands sent via UART to the ESP during setup.
  static int bufpos = 0;                // starts the buffer back at the first position in the incoming serial.read

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();                    // Disconnect the Wifi before re-enabling it

  // Wait for system to stabilize
  delay(1000);  
  Serial.print("\nSend a newline, or a command string to start\n");
  Serial.flush();                       // Ensure no old data is left in the buffer before signaling RDY 

  //Indicate to the connected device that the ESP is ready to recive a command 
  digitalWrite(RDY, HIGH);              // Set RDY to TM4C
  delay (500);                          // Wait before checking if serial data is being sent

  //Wait until at least one byte of data is recevied via UART, then record data to the 
  //bufffer until a new line character ('\n') is recevied.
  while ((Serial.available() == 0)) {}
  while (Serial.available() > 0)   {
      char inchar = Serial.read();  // assigns one byte (as serial.read()'s only input one byte at a time
      if (inchar != '\n') {         // if the incoming character is not a newline then process byte
        ser_buf[bufpos] = inchar;   // the buffer position in the array get assigned to the current read
        bufpos++;                   // once that has happend the buffer advances, doing this over and over again until the end of package marker is read.
        delay(10);
      }
  }

  // If any non-newline characters were sent, interprete it as a comma seperated
  // set of values to overwrite the default eid, ssid, password, mqtt broker, and port
  if (bufpos  > 0) {
    char pf = 0; //Parse Failed
    pf |= get_next_token(eid,          ser_buf, ",");
    pf |= get_next_token(ssid,         NULL,    ",");
    pf |= get_next_token(password,     NULL,    ",");
    pf |= get_next_token(mqtt_broker,  NULL,    ",");
    pf |= get_next_token(port,         NULL,    ",");
 
    for (int i = 0; i < SER_BUF_LEN; i++)  (ser_buf[i]) = 0;
    bufpos = 0;     // Reset buffer pointer
      
    #ifdef DEBUG2
      Serial.println();
      Serial.println(ssid);
      Serial.println(password);
      if(pf)
        Serial.print("\nFailed to parse one or more input strings!\n");
    #endif
  }
  mqtt_port = atoi(port);

  // connect to a WiFi network
  WiFi.begin(ssid, password);
  
  #ifdef DEBUG1
    Serial.print("\nConnecting to WiFi..");
  #endif

  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");                 // Feedback that we are still connecting
  }
  
  #ifdef DEBUG1
    Serial.println("\nConnected to the WiFi network");
    Serial.println();
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
  #endif
  
  Serial.flush();

}   //  END OF WiFi Setup


// ----------------------------------------------------------------
// -----------------   MAIN SETUP  --------------------------------
// ----------------------------------------------------------------

void setup() {

  //Setup UART
  Serial.begin(9600);                       // Set baud rate to 9600;
  Serial.flush();                           // Flush the serial port buffer
  delay(1000);
  Serial.println("Hello world starting up!");

  //Setup GPIO
  pinMode(0, INPUT);                        // Set GPIO_0 to an input
  pinMode(2, OUTPUT);                       // Set GPIO_2 to an output - RDY signal to the TM4C
  digitalWrite(RDY, LOW);                   // Set the RDY pin LOW

  //Setup Wifi & MQTT 
  Setup_Wifi();                             // This routine reads in the  EID, SSID, Password
  delay(100);                               // Wait before using WIFI
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  
  // ---   Connect to a mqtt broker    ---

  while (!client.connected()) {

      //Create a 'unique' ID so that you are able to connect
      String client_id = "ee445l-mqtt-";
      client_id += eid;
      
      #ifdef DEBUG1
        Serial.print("The client is connecting to the mqtt broker using client ID:  "); 
        Serial.println(client_id.c_str());
        Serial.flush();
      #endif

      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("EE445L MQTT broker connected");
          Serial.flush();
      } else {
          Serial.print("Connection failed with state = ");
          Serial.print(client.state());
          Serial.flush();
      }
  }
  // MQTT publish and subscribe
  snprintf(topic_subscribe, sizeof(topic_subscribe), "%s%s", eid, topic_w2b);   // Prepend EID to top
  
  if (client.subscribe(topic_subscribe)){
    Serial.print("Subscribed to: ");
    Serial.println(topic_subscribe);
    #ifdef DEBUG2
      Serial.print("EID: ");
      Serial.println(eid);
      Serial.flush();
    #endif
    Serial.flush();
  } else {
    Serial.print("Subscribe failed with state = ");
    Serial.print(client.state());
    Serial.flush();
  }                 // Subscribe to the Web based controller 
  
  
  #ifdef DEBUG1
    Serial.println("Publishing to the following topics");
    snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_mode); Serial.println(topic_publish);
    snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_hour); Serial.println(topic_publish);
    snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_min);  Serial.println(topic_publish);
    snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_sec);  Serial.println(topic_publish);
    snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_mode3);  Serial.println(topic_publish);
    Serial.flush();
  #endif
 
  timer.setInterval(1000L, tm4c2mqtt);            // Run the TM4C to MQTT interface once per second
  digitalWrite(RDY, LOW);                         // Set the RDY pin LOW
}


// -----------------------------------------------------------------------------------
// ---  This is the callback when messages are received from the Web Contoller  -----
// -----------------------------------------------------------------------------------

void callback(char *topic_subscribe, byte *payload, unsigned int length) {
  
  payload[length] = '\0';
  
//  #ifdef DEBUG4
//    Serial.println("-----------------------");
//    Serial.print("Message arrived in topic:  ");
//    Serial.println(topic_subscribe);
//    Serial.print("Message (char):  ");
//    for (int i = 0; i < length; i++){
//      Serial.print((char) payload[i]);
//    }
//    Serial.println();
//    Serial.flush();
//  #endif

  // Retreive W2B command from received data
  if (length  > 0) {
    strcpy(cmd,    strtok((char *)payload, ""));
    Serial.println(cmd);                  // Send the command to the TM4C
    Serial.flush();                       // Ensure the serial buffer is emptied

    // #ifdef DEBUG4
    // Serial.print("W2B Command:  ");
    // Serial.println(cmd);
    // Serial.println("-----------------------");
    // #endif
  }
}

// ------------------------------------------------------------------------
//  This routine sends Lab 4E data to the Web page
//
void tm4c2mqtt(void) {
  
  static int bufpos = 0;              // starts the buffer back at the first position in the incoming serial.read

  while (Serial.available() > 0)      // Wait for date from the TM4C
  {
    char inchar = Serial.read();      // Assigns one byte (as serial.read()'s only input one byte at a time
        
    if (inchar != '\n') {             // if the incoming character is not a newline then process byte the buffer position
      ser_buf[bufpos] = inchar;       // in the array get assigned to the current read once that has happend the buffer advances,
      bufpos++;                       //  doing this over and over again until the end of package marker is read.
      delay(10);
    }
    else if (inchar == '\n')
    {
       if (bufpos  > 0) 
       {

        char pf = 0; //Parse Failed
        pf |= get_next_token(clk_mode, ser_buf, ",");
        pf |= get_next_token(hour,     NULL,    ",");
        pf |= get_next_token(minute,   NULL,    ",");
        pf |= get_next_token(second,   NULL,    ",");
        pf |= get_next_token(mode3,   NULL,    ",");
        #ifdef DEBUG2
          if(pf)
            Serial.print("\nFailed to parse one or more input strings!\n");
        #endif
        
        snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_mode);
        client.publish(topic_publish,   clk_mode,    1); 
        
        snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_hour);
        client.publish(topic_publish,  hour,     1); 
        
        snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_min);
        client.publish(topic_publish,   minute,  1); 
        
        snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_sec);
        client.publish(topic_publish,   second,  1); 

        snprintf(topic_publish, sizeof(topic_publish), "%s%s", eid, pub_mode3);
        client.publish(topic_publish,   mode3,  1); 
      }

      for (int i = 0; i < SER_BUF_LEN; i++)  (ser_buf[i]) = 0;
      bufpos = 0;     // Reset buffer pointer
      clk_mode[0] = 0x00; //Clear string after use
      hour[0] = 0x00;
      minute[0] = 0x00;
      second[0] = 0x00;
    }
  
  } 
}

void loop() {
  timer.run();
  client.loop();

}
