/*
   Eclipse Paho MQTT-JS Utility
   by Elliot Williams for Hackaday article

   Hacked up by:  Mark McDermott for EE445L Lab 4E
   On:            5/29/23   
*/

// ---------------------------------------
// Global variables
//

var client      = null;
var hour        = "";
var minute      = "";
var second      = "";
var ampm        = "";
var mil_time    = "";
var mode3;

// TODO: update eid with your own.
var hostname        = "broker.emqx.io";
var port            = "1883";
var eid             = "ntg337_pt8352"
var clientId        = "mqtt_ee445l_" + eid;

// Subscribe (from board_2_webpage)

var test 	= 	eid + "/test";
var hour_bd =   eid + "/b2w/hour"; 
var min_bd  =   eid + "/b2w/min";
var sec_bd  =   eid + "/b2w/sec"; 
var mode_bd =   eid + "/b2w/mode";   
var mode3_bd = 	eid + "/b2w/mode3";

// Publish (from webpage_2_board) 
 
var w2b   =  eid + "/w2b";

// -----------------------------------------------------------------------
// This is called after the webpage is completely loaded
// It is the main entry point into the JS code

function connect() {
	// Set up the client
	// TODO: We use a default here for emqx, but if you're using ECE445L broker,
	// feel free to replace with the hostname + port specified earlier. 
	const url = 'ws://broker.emqx.io:8083/mqtt';

	const options = {
		// Clean session
		clean: true,
		connectTimeout: 4000,
		// Authentication
		clientId: eid,
		username: null,
		password: null,
	};
	client  = mqtt.connect(url, options);
	client.on('connect', function () {
		onConnect();
	});

	// Receive messages
	client.on('message', function (topic, payload, packet) {
	  	onMessageArrived(topic, payload);
	});
}

function onConnect() {
	console.log("Client Connected.");
    
	// TODO: subscribe to your desired topics.
	client.subscribe(test);
	client.subscribe(hour_bd);
	client.subscribe(min_bd);
	client.subscribe(sec_bd);
    client.subscribe(mode_bd);
	client.subscribe(mode3_bd);
}

function onMessageArrived(topic, message) {
	console.log(topic, String(message));

	// TODO: call method to update logic and update UI.
	// Update element depending on which topic's data came in.
	switch (topic) {
		case test:
			console.log("Test message!");
			break;
		case ('ntg337_pt8352/b2w/hour'):
			hour = message;
		case ('ntg337_pt8352/b2w/min'):
			minute = message;
		case ('ntg337_pt8352/b2w/sec'):
			second = message;
		default:

			break;
	}
}

// -----------------------------------------------------------------------
// Provides the button logic that toggles the mode
// Triggered by pressing the HTML button "12/24"
//
function toggleMode() {
	var payload = "T";
	console.log("Publishing ", payload, " to ", w2b);
	client.publish(w2b, payload);
}
function OkButton() {
	console.log("Publishing OK to ", w2b);
	client.publish(w2b, 'K');
}
function SnoozeButton() {
	console.log("Publishing OK to ", w2b);
	client.publish(w2b, 'Z');
}
function set_clock(){
	console.log("Setting Clock Time", w2b);
	client.publish(w2b, 'c');

}
function set_alarm(){
	console.log("Setting Alarm Time ", w2b);
	client.publish(w2b, 'a');

}
function set_timer(){
	console.log("Setting Timer Time ", w2b);
	client.publish(w2b, 't');

}

function add_hour(){
	console.log("Incrementing Hours ", w2b);
	client.publish(w2b, 'H');
}
function dec_hour(){
	console.log("Decrementing Hours ", w2b);
	client.publish(w2b, 'h');

}
function add_min(){
	console.log("Incrementing Minutes ", w2b);
	client.publish(w2b, 'M');

}
function dec_min(){
	console.log("Decrementing Minutes ", w2b);
	client.publish(w2b, 'm');

}
function add_sec(){
	console.log("Incrementing Seconds ", w2b);
	client.publish(w2b, 'S');

}
function dec_sec(){
	console.log("Decrementing Seconds ", w2b);
	client.publish(w2b, 's');

}

// //////////////////////////////////////////////////////////////////////////
//
//  ADD MORE FUNCTIONS HERE
//
// //////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------
// This function appends AM or PM to the time when not in 24 hour mode
//
function Board_Time() {

    // Creating object of the Date class
  
    // Variable to store AM / PM
    
    var period = "";
  
    // Assigning AM / PM according to current hour
    if ((hour <= 11) && (mil_time == 0))
    {
        period = "AM";
    } 
    else if (hour >= 12 && (mil_time == 0))
    {
        period = "PM";
        if (hour != 12) {
        hour = hour - 12;
        }
    }
    else if (mil_time == 1)
    {
        period ="";
    }
  
    // Adding time elements to the div
    document.getElementById("board-clock").innerText = hour + " : " + minute + " : " + second + " " + period;
  
    // Set Timer to 1 sec (1000 ms)
    setTimeout(Board_Time, 1000);
}

function update(t) {
	if (t < 10) {
		return "0" + t;
	}
	else {
		return t;
	}
}

Board_Time();
