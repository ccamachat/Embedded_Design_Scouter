// Author : Ethan Syed
// Inertial Measurement Unit (IMU)
// Accelerometer
// Adafruit BoB

#include <stdint.h>

// control
void initialize_accelerometer_interrupts(void);
void init_accelerometer(void);
void disable_ODR(void);

void poll_status_reg(void);

// data
int get_x_axis(void);
int get_y_axis(void);
int get_z_axis(void);