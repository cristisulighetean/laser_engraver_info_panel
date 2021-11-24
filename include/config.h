#ifndef LCD_UTILS
#define LCD_UTILS

// LCD Setup
/*Backpack LCD RX goes to Arduino TX which is pin 3 */
#define maxX 127//159 
#define maxY 63 //127

// IO pins
#define FAN_PIN 4
#define SOUND_SENSOR_PIN 5
#define SMOKE_SENSOR_PIN A0
#define RED_LED_PIN 7
#define GREEN_LED_PIN 6

// Function headers
void get_gas_data_fan_control(void);

void get_sound_reading(void);

void get_gas_raw_data(void);

void countTime();

void result();

void displayTemplate(void);

void update_display(void);

void print_status_serial();










#endif