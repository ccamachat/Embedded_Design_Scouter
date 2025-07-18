// I2C2.c
// Runs on TM4C123
// Busy-wait device driver for the I2C2.
// Daniel and Jonathan Valvano
// Jan 2, 2021
// This file originally comes from the TIDA-010021 Firmware (tidcf48.zip) and
// was modified by Pololu to support the MSP432P401R. Modified again for TM4C123

/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
      ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2020
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
      ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2020
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
      ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2020

 Copyright 2022 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// TM4C123 hardware
// SDA    PE5 I2C data
// SCL    PE4 I2C clock
/*
 *  I2C0 Conncection | I2C1 Conncection | I2C2 Conncection | I2C3 Conncection
 *  ---------------- | ---------------- | ---------------- | ----------------
 *  SCL -------- PB2 | SCL -------- PA6 | SCL -------- PE4 | SCL -------- PD0
 *  SDA -------- PB3 | SDA -------- PA7 | SDA -------- PE5 | SDA -------- PD1
 */
#include <stdint.h>
#include "../inc/I2C2.h"
#include "../inc/tm4c123gh6pm.h"
#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable
#define MAXRETRIES              5           // number of receive attempts before giving up

// let t be bus period, let F be bus frequency
// let f be I2C frequency
// at F=80 MHz, I2C period = (TPR+1)*250ns 
// f=400kHz,    I2C period = 20*(TPR+1)*12.5ns = 2.5us, with TPR=9
// I2C period, 1/f = 20*(TPR+1)*t 
// F/f = 20*(TPR+1)
// TPR = (F/f/20)-1 
void I2C2_Init(uint32_t I2Cfreq, uint32_t busFreq){
  SYSCTL_RCGCI2C_R |= 0x0004;           // activate I2C2
  SYSCTL_RCGCGPIO_R |= 0x0010;          // activate port E
  while((SYSCTL_PRGPIO_R&0x0010) == 0){};// ready?
  GPIO_PORTE_AFSEL_R |= 0x30;           // 3) enable alt funct on PE5,4
  GPIO_PORTE_ODR_R |= 0x20;             // 4) enable open drain on PE5 only
  GPIO_PORTE_DR8R_R |= 0x30;            //  high current on PE5,4
  GPIO_PORTE_DEN_R |= 0x30;             // 5) enable digital I/O on PE5,4
                                        // 6) configure PE5,4 as I2C
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFF00FFFF)+0x00330000;
  GPIO_PORTE_AMSEL_R &= ~0x30;          // 7) disable analog functionality on PE5,4
  I2C2_MCR_R = I2C_MCR_MFE;             // 9) master function enable, no glitch
  I2C2_MCR2_R = I2C_MCR2_GFPW_BYPASS;   // bypass glitch
//  I2C2_MCR_R = I2C_MCR_MFE|I2C_MCR_GFE;             // 9) master function enable, glitch
//  I2C2_MCR2_R = I2C_MCR2_GFPW_4; // 4 clock glitch
  I2C2_MTPR_R = (busFreq/(I2Cfreq * 20))-1; // 8) configure clock speed
}

int I2C2_Send(uint8_t slaveAddr, uint8_t *pData, uint32_t count){
  while(I2C2_MCS_R & I2C_MCS_BUSY){};                // wait for I2C ready
  I2C2_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;      // MSA[7:1] is slave address
  I2C2_MSA_R &= ~I2C_MSA_RS;                         // MSA[0] is 0 for send
    
  if(count == 1) {
    I2C2_MDR_R = pData[0] & I2C_MDR_DATA_M;          // prepare data byte
    I2C2_MCS_R = (I2C_MCS_STOP |                     // generate stop
    
                  I2C_MCS_START |                    // generate start/restart
   
                  I2C_MCS_RUN);                      // master enable
    while (I2C2_MCS_R & I2C_MCS_BUSY){};             // wait for transmission done
    // return error bits
    return (I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
        
  }else {
    I2C2_MDR_R = pData[0] & I2C_MDR_DATA_M;           // prepare data byte
    I2C2_MCS_R |= I2C_MCS_RUN|I2C_MCS_START;           // run and start
//		uint32_t mcs = I2C2_MCS_R ;
//		mcs &= ~0x00000014; // ---0-0-- 
//		mcs |= 0x00000003;  // ------11
//		I2C2_MCS_R = mcs ;
    while (I2C2_MCS_R & I2C_MCS_BUSY){};              // wait for transmission done
		I2C2_MRIS_R &= 0x03; // Manually clear Interrupt status bits
		uint32_t x = I2C2_MCS_R;
		while (I2C2_MCS_R & I2C_MCS_BUSY){};              // wait for transmission done

//    if((I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
//      I2C2_MCS_R = I2C_MCS_STOP;                      // stop transmission
//       // return error bits if nonzero
//      return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
//    }
    for(int i = 1; i < count-1; i++) {
      I2C2_MDR_R = pData[i] & I2C_MDR_DATA_M;         // prepare data byte
      I2C2_MCS_R = I2C_MCS_RUN;                       // master enable
      while (I2C2_MCS_R & I2C_MCS_BUSY){};            // wait for transmission done
//      // check error bits
//      if((I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
//        I2C2_MCS_R = I2C_MCS_STOP;                    // stop transmission
//       // return error bits if nonzero
//        return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
//      }
    }      
    I2C2_MDR_R = pData[count-1] & I2C_MDR_DATA_M;     // prepare last byte
    I2C2_MCS_R = (I2C_MCS_STOP |                      // generate stop
                  I2C_MCS_RUN);                       // master enable
    while (I2C2_MCS_R & I2C_MCS_BUSY) {};             // wait for transmission done
    // return error bits
    return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
}
uint32_t I2C2_Send3(int8_t slave, uint8_t data1, uint8_t data2, uint8_t data3){
  while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
  I2C2_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
  I2C2_MSA_R &= ~0x01;             // MSA[0] is 0 for send
  I2C2_MDR_R = data1&0xFF;         // prepare first byte
  I2C2_MCS_R = (0
											| I2C_MCS_ACK     // no data ack (no data on send)
                     //  & ~I2C_MCS_STOP    // no stop
                       | I2C_MCS_START    // generate start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C2_MCS_R = (0                // send stop if nonzero
                       | I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // stop
                     //  & ~I2C_MCS_START   // no start/restart
                     //  & ~I2C_MCS_RUN   // master disable
                       );   
                                          // return error bits if nonzero
    return (I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  I2C2_MDR_R = data2&0xFF;         // prepare second byte
  I2C2_MCS_R = (0
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                     //  & ~I2C_MCS_STOP    // no stop
                     //  & ~I2C_MCS_START   // no start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C2_MCS_R = (0                // send stop if nonzero
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // stop
                     //  & ~I2C_MCS_START   // no start/restart
                     //  & ~I2C_MCS_RUN   // master disable
                        );
                                          // return error bits if nonzero
    return (I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  I2C2_MDR_R = data3&0xFF;         // prepare third byte
  I2C2_MCS_R = (0
                    //   & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // generate stop
                    //   & ~I2C_MCS_START   // no start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // return error bits
  return (I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}
void I2C_Write(uint8_t slaveAddr, uint8_t regAddr, uint8_t data) {
    // Write the slave address to the data register
    I2C2_MSA_R = (slaveAddr << 1); // Write operation

    // Write the register address to the data register
    I2C2_MDR_R = regAddr&0xFF;
    I2C2_MCS_R = (I2C_MCS_START | I2C_MCS_RUN); // Start transmission
    while (I2C2_MCS_R & I2C_MCS_BUSY) {} // Wait for transmission to complete

    // Check for acknowledgment
    if (!(I2C2_MCS_R & I2C_MCS_ERROR)) {
        // Write data to the register
        I2C2_MDR_R = data;
        I2C2_MCS_R = (I2C_MCS_STOP | I2C_MCS_RUN); // Start transmission
        while (I2C2_MCS_R & I2C_MCS_BUSY) {} // Wait for transmission to complete
    }
}

int I2C2_Send1(uint8_t slaveAddr, uint8_t data){
  while(I2C2_MCS_R & I2C_MCS_BUSY){};  // wait for I2C ready
  I2C2_MSA_R = (slaveAddr << 1);       // MSA[7:1] is slave address
                                       // MSA[0] is 0 for send
  I2C2_MDR_R = data;                   // prepare data byte
  I2C2_MCS_R = (I2C_MCS_STOP |         // generate stop
                I2C_MCS_START |        // generate start/restart
                I2C_MCS_RUN);          // master enable
  while (I2C2_MCS_R & I2C_MCS_BUSY){}; // wait for transmission done
    // return error bits
  return (I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));     
}

// sends two bytes to specified slave
// Returns 0 if successful, nonzero if error
int I2C2_Send2(int8_t slave, uint8_t data1, uint8_t data2){
  while(I2C2_MCS_R&0x01){}; // wait for I2C ready
  I2C2_MSA_R = slave<<1;    // MSA[7:1] is slave address
                            // MSA[0] is 0 for send
  I2C2_MDR_R = data1;       // prepare first byte
  I2C2_MCS_R = 0x03; // no stop, generate start, enable
  while(I2C2_MCS_R&0x01){}; // wait for transmission done
  if((I2C2_MCS_R&0x0E) != 0){ 
    I2C2_MCS_R = 0x04;      // send stop if error
    return I2C2_MCS_R&0x0E; // return error bits
  }
  I2C2_MDR_R = data2;       // prepare second byte
  I2C2_MCS_R = 0x05;        // generate stop, no start, enable
  while(I2C2_MCS_R&0x01){}; // wait for transmission done
  return I2C2_MCS_R&0x0E;   // return error bits, if any
}

//int I2C2_Send2b(int8_t slave, uint8_t data1, uint8_t data2){
//  while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
//  I2C2_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
//  I2C2_MSA_R &= ~0x01;             // MSA[0] is 0 for send
//  I2C2_MDR_R = data1&0xFF;         // prepare first byte
//  I2C2_MCS_R = (0
//                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
//                    //   & ~I2C_MCS_STOP    // no stop
//                       | I2C_MCS_START    // generate start/restart
//                       | I2C_MCS_RUN);    // master enable
//  while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
//                                          // check error bits
//  if((I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
//    I2C2_MCS_R = (0                // send stop if nonzero
//                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
//                       | I2C_MCS_STOP     // stop
//                     //  & ~I2C_MCS_START   // no start/restart
//                     //  & ~I2C_MCS_RUN    // master disable
//                        );   
//                                          // return error bits if nonzero
//    return (I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
//  }
//  I2C2_MDR_R = data2&0xFF;         // prepare second byte
//  I2C2_MCS_R = (0
//                      // & ~I2C_MCS_ACK     // no data ack (no data on send)
//                       | I2C_MCS_STOP     // generate stop
//                      // & ~I2C_MCS_START   // no start/restart
//                       | I2C_MCS_RUN);    // master enable
//  while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
//                                          // return error bits
//  return (I2C2_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
//}

int I2C2_Send4(uint8_t slaveAddr, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4){
  while(I2C2_MCS_R & I2C_MCS_BUSY){};       // wait for I2C ready
  I2C2_MSA_R = (slaveAddr << 1);            // MSA[7:1] is slave address
                                            // MSA[0] is 0 for send
    
  I2C2_MDR_R = data1;                       // prepare data byte
  I2C2_MCS_R = I2C_MCS_RUN|I2C_MCS_START;   // run and start
  while (I2C2_MCS_R & I2C_MCS_BUSY){};      // wait for transmission done
  if((I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C2_MCS_R = I2C_MCS_STOP;              // stop transmission
       // return error bits if nonzero
    return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  // second
  I2C2_MDR_R = data2;                       // prepare data byte
  I2C2_MCS_R = I2C_MCS_RUN;                 // master enable
  while (I2C2_MCS_R & I2C_MCS_BUSY){};      // wait for transmission done
      // check error bits
  if((I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C2_MCS_R = I2C_MCS_STOP;              // stop transmission
       // return error bits if nonzero
    return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
   // third
  I2C2_MDR_R = data3;                       // prepare data byte
  I2C2_MCS_R = I2C_MCS_RUN;                 // master enable
  while (I2C2_MCS_R & I2C_MCS_BUSY){};      // wait for transmission done
      // check error bits
  if((I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C2_MCS_R = I2C_MCS_STOP;              // stop transmission
       // return error bits if nonzero
    return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
   // fourth      
  I2C2_MDR_R = data4;                       // prepare last byte
  I2C2_MCS_R = (I2C_MCS_STOP |              // generate stop
                I2C_MCS_RUN);               // master enable
  while (I2C2_MCS_R & I2C_MCS_BUSY) {};     // wait for transmission done
  // return error bits
  return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}




// receives one byte from specified slave
// Note for HMC6352 compass only:
// Used with 'r' and 'g' commands
// Note for TMP102 thermometer only:
// Used to read the top byte of the contents of the pointer register
//  This will work but is probably not what you want to do.
uint8_t I2C2_Recv1(int8_t slave){
  int retryCounter = 1;
  do{
    while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
    I2C2_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
    I2C2_MSA_R |= 0x01;              // MSA[0] is 1 for receive
    I2C2_MCS_R = (0
                        // & ~I2C_MCS_ACK     // negative data ack (last byte)
                         | I2C_MCS_STOP     // generate stop
                         | I2C_MCS_START    // generate start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    retryCounter = retryCounter + 1;        // increment retry counter
  }                                         // repeat if error
  while(((I2C2_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
  return (I2C2_MDR_R&0xFF);          // usually returns 0xFF on error
}
int I2C2_Recv(uint8_t slaveAddr, uint8_t *pData, uint32_t count){
  while(I2C2_MCS_R & I2C_MCS_BUSY){};                    // wait for I2C ready
  switch(count){
    case 1:
        I2C2_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;    // MSA[7:1] is slave address
        I2C2_MSA_R |= I2C_MSA_RS;                        // MSA[0] is 1 for receive
              
        I2C2_MCS_R = (I2C_MCS_STOP  |                    // generate stop
                      I2C_MCS_START |                    // generate start/restart
                      I2C_MCS_RUN);                      // master enable
        while (I2C2_MCS_R & I2C_MCS_BUSY) {};            // wait for transmission done
        pData[0] = (I2C2_MDR_R & I2C_MDR_DATA_M);        // usually 0xFF on error
        break;
    case 2:
        I2C2_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;    // MSA[7:1] is slave address
        I2C2_MSA_R |= I2C_MSA_RS;                        // MSA[0] is 1 for receive
            
        I2C2_MCS_R = (I2C_MCS_ACK   |                    // positive data ack
                      I2C_MCS_START |                    // generate start/restart
                      I2C_MCS_RUN);                      // master enable
        while (I2C2_MCS_R & I2C_MCS_BUSY) {};            // wait for transmission done
        pData[0] = (I2C2_MDR_R & I2C_MDR_DATA_M);        // most significant byte
              
        I2C2_MCS_R = (I2C_MCS_STOP |                     // generate stop
                      I2C_MCS_RUN);                      // master enable
        while(I2C2_MCS_R & I2C_MCS_BUSY){};              // wait for transmission done
        pData[1] = (I2C2_MDR_R & I2C_MDR_DATA_M);        // least significant byte                                                       // repeat if error
        break;
    default:
        I2C2_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;    // MSA[7:1] is slave address
        I2C2_MSA_R |= I2C_MSA_RS;                        // MSA[0] is 1 for receive
              
        I2C2_MCS_R = (I2C_MCS_ACK   |                    // positive data ack
                      I2C_MCS_START |                    // generate start/restart
                      I2C_MCS_RUN);                      // master enable
        while (I2C2_MCS_R & I2C_MCS_BUSY) {};            // wait for transmission done
        pData[0] = (I2C2_MDR_R & I2C_MDR_DATA_M);        // most significant byte               
        for(int i = 1; i < count-1; i++){
          I2C2_MCS_R = (I2C_MCS_ACK |                    // positive data ack
                        I2C_MCS_RUN);                    // master enable
          while (I2C2_MCS_R & I2C_MCS_BUSY) {};          // wait for transmission done
          pData[i] = (I2C2_MDR_R & I2C_MDR_DATA_M);      // read byte
        }
             
        I2C2_MCS_R = (I2C_MCS_STOP |                     // generate stop
                      I2C_MCS_RUN);                      // master enable
        while (I2C2_MCS_R & I2C_MCS_BUSY) {};            // wait for transmission done
        pData[count-1] = (I2C2_MDR_R & I2C_MDR_DATA_M);  // least significant byte
        break;
  }
  return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}
// receives two bytes from specified slave
// Note for HMC6352 compass only:
// Used with 'A' commands
// Note for TMP102 thermometer only:
// Used to read the contents of the pointer register
uint16_t I2C2_Recv2(int8_t slave){
  uint8_t data1,data2;
  int retryCounter = 1;
  do{
    while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
    I2C2_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
    I2C2_MSA_R |= 0x01;              // MSA[0] is 1 for receive
    I2C2_MCS_R = (0
                         | I2C_MCS_ACK      // positive data ack
                       //  & ~I2C_MCS_STOP    // no stop
                         | I2C_MCS_START    // generate start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    data1 = (I2C2_MDR_R&0xFF);       // MSB data sent first
    I2C2_MCS_R = (0
                       //  & ~I2C_MCS_ACK     // negative data ack (last byte)
                         | I2C_MCS_STOP     // generate stop
                       //  & ~I2C_MCS_START   // no start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C2_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    data2 = (I2C2_MDR_R&0xFF);       // LSB data sent last
    retryCounter = retryCounter + 1;        // increment retry counter
  }                                         // repeat if error
  while(((I2C2_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
  return (data1<<8)+data2;                  // usually returns 0xFFFF on error
}

int I2C2_Recv3(uint8_t slaveAddr, uint8_t data[3]){
  while(I2C2_MCS_R & I2C_MCS_BUSY){};         // wait for I2C ready
  I2C2_MSA_R = (slaveAddr << 1)|I2C_MSA_RS;   // MSA[7:1] is slave address
                                              // MSA[0] is 1 for receive
// first              
  I2C2_MCS_R = (I2C_MCS_ACK   |               // positive data ack
                I2C_MCS_START |               // generate start/restart
                I2C_MCS_RUN);                 // master enable
  while (I2C2_MCS_R & I2C_MCS_BUSY) {};       // wait for transmission done
  data[0] = I2C2_MDR_R ;                      // most significant byte               
 // second
  I2C2_MCS_R = (I2C_MCS_ACK |                 // positive data ack
                I2C_MCS_RUN);                 // master enable
  while (I2C2_MCS_R & I2C_MCS_BUSY) {};       // wait for transmission done
  data[1] = I2C2_MDR_R;                       // read byte
// third     
  I2C2_MCS_R = (I2C_MCS_STOP |                // generate stop
                I2C_MCS_RUN);                 // master enable
  while (I2C2_MCS_R & I2C_MCS_BUSY) {};       // wait for transmission done
  data[2] = I2C2_MDR_R;                       // least significant byte
  return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}


int I2C2_SendData(uint8_t slaveAddr, uint8_t *pData, uint32_t count){
  while(I2C2_MCS_R & I2C_MCS_BUSY){};                // wait for I2C ready
  I2C2_MSA_R = (slaveAddr << 1) ;      // MSA[7:1] is slave address
 // I2C2_MSA_R &= ~I2C_MSA_RS;                         // MSA[0] is 0 for send
    

  I2C2_MDR_R = 0x40;           // first byte 0x40 means data bytes
  I2C2_MCS_R = I2C_MCS_RUN|I2C_MCS_START;           // run and start
  while (I2C2_MCS_R & I2C_MCS_BUSY){};              // wait for transmission done
//  if((I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
//    I2C2_MCS_R = I2C_MCS_STOP;                      // stop transmission
//     // return error bits if nonzero
//    return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
//  }
  for(int i = 0; i < count-1; i++) {
    I2C2_MDR_R = pData[i] ;         // prepare data byte
    I2C2_MCS_R = I2C_MCS_RUN;                       // master enable
    while (I2C2_MCS_R & I2C_MCS_BUSY){};            // wait for transmission done
      // check error bits
//    if((I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
//      I2C2_MCS_R = I2C_MCS_STOP;                    // stop transmission
//       // return error bits if nonzero
//      return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
//    }
  }      
  I2C2_MDR_R = pData[count-1] ;     // prepare last byte
  I2C2_MCS_R = (I2C_MCS_STOP |                      // generate stop
                I2C_MCS_RUN);                       // master enable
  while (I2C2_MCS_R & I2C_MCS_BUSY) {};             // wait for transmission done
  // return error bits
  return (I2C2_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));

}
