// Author : Ethan Syed
// Testcases for heartbeat sensor
// PN : MAX30102

#include <stdint.h>
#include <stdbool.h>
#include "../src/MAX30102_driver.h"
#include "../inc/tm4c123gh6pm.h"
#include "ADC.h"
uint16_t readHeartbeatSensor();

uint16_t readHeartbeatSensor() {
  //read heartbeat from adc
	uint32_t heartbeat_reading = get_raw();
	return heartbeat_reading;
  
}

