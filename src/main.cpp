#include <Arduino.h>
#include <SparkFunSerialGraphicLCD.h>
#include <SoftwareSerial.h>
#include <config.h>

LCD LCD; // lcd object

// Global config variables
#define SMOKE_SENSOR_THRESH 800
int smoke_flag = false;
int sound_flag = false; 


// Chrono variables
unsigned long chrono = 0;
int seconds = 0;
int minutes = 0;


// Display values
int sec_display = 0;
int min_display = 0;
int status_display = 0;
int fan_display = 0;

void setup() {

  // Start serial
  Serial.begin(115200);

  // init pins
  pinMode(FAN_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(SOUND_SENSOR_PIN, INPUT);

  delay(1200);			//wait for the one second spalsh screen before anything is sent to the LCD.
  
  LCD.restoreDefaultBaud();
  LCD.setHome();		//set the cursor back to 0,0.
  LCD.clearScreen();		//clear anything that may have been previously printed ot the screen.
  delay(10);

  // Write Laser engraver
  LCD.setX((maxX/2)-42);	//these are calculated by dividing maxX by 2, then subtracting (half of the number of chars in the word times 6).
  LCD.setY(maxY - 5);
  LCD.printStr("LASER ENGRAVER");

  // Draw Line at pos maxY-15 on Y
  LCD.drawLine(0,(maxY-15),maxX,(maxY-15),1);//draw top line

  // Write starting
  LCD.setX((maxX/2)-36);
  LCD.setY(maxY - 25);
  LCD.printStr("STARTING ...");
  delay(2000);

  // Clear All
  LCD.setX((maxX/2)-36);
  LCD.setY(maxY - 25);
  LCD.printStr("                  ");
  delay(500);

  // write displayTemplate
  displayTemplate();
}

void loop() {

  // Get reading form sensors
  get_gas_data_fan_control();
  get_sound_reading();

  // Call update time
  if (sound_flag == 1)
    countTime();

  // Display on LCD 
  update_display();
  print_status_serial();

  // 1hz working mode
  delay(1000);

}

void displayTemplate(void)
{
  // Write Status
  LCD.setX((maxX/2) - 33);
  LCD.setY(maxY - 20);
  LCD.printStr("STATUS: OFF");

  // Write Fan
  LCD.setX((maxX/2) - 21);
  LCD.setY(maxY - 30);
  LCD.printStr("FAN: OFF");


  // Write operating time
  LCD.setX((maxX/2) - 45);
  LCD.setY(maxY - 40);
  LCD.printStr("OPERATING TIME: ");

  // Write operating time OO
  LCD.setX((maxX/2) - 33);
  LCD.setY(maxY - 50);
  LCD.printStr("00 M : 00 S");

}

void update_display(void){

  // Updates LCD in case the flag changed
  if (sound_flag != status_display)
  {
    // Erase status machine 
    LCD.setX((maxX/2) - 33);
    LCD.setY(maxY - 20);
    LCD.printStr("                  ");
    delay(10);
    
    // Write new status (TODO 79)
    LCD.setX((maxX/2) - 33);
    LCD.setY(maxY - 20);
    LCD.printStr("STATUS: ");

    if (sound_flag == 0)
      LCD.printStr("OFF");
    else
      LCD.printStr("ON");
    
    // Set new display val
    status_display = sound_flag;
  }

  if (smoke_flag != fan_display)
  {
    // Erase fan status
    LCD.setX((maxX/2) - 21);
    LCD.setY(maxY - 30);
    LCD.printStr("                  ");
    delay(10);
    
    // Write new status (TODO 73)
    LCD.setX((maxX/2) - 21);
    LCD.setY(maxY - 30);
    LCD.printStr("FAN: ");
    if (smoke_flag == 0)
      LCD.printStr("OFF");
    else
      LCD.printStr("ON");
    
    // Set new display val
    fan_display = smoke_flag;
  }

  // Erase min & sec
  LCD.setX((maxX/2) - 33);
  LCD.setY(maxY - 50);
  LCD.printStr("                  ");
  delay(10);
  
  // Write operating time OO
  LCD.setX((maxX/2) - 33);
  LCD.setY(maxY - 50);
  LCD.printNum(minutes);
  LCD.printStr(" M : ");
  LCD.printNum(seconds);
  LCD.printStr(" S");
  
}


void get_gas_data_fan_control(void)
{
  // Citire valoare pin analog
  int reading = analogRead(SMOKE_SENSOR_PIN);

  if (reading > SMOKE_SENSOR_THRESH && smoke_flag == false)
  {
    smoke_flag = true;
    digitalWrite(FAN_PIN, HIGH);
  }
  else if (reading < SMOKE_SENSOR_THRESH && smoke_flag == true) 
  {
    smoke_flag = false;
    digitalWrite(FAN_PIN, LOW);
  }
}


void get_sound_reading(void){
  
  // Get pin reading
  uint8_t reading = digitalRead(SOUND_SENSOR_PIN);

  if(reading > 0 && sound_flag == false) 
  {
    // Turn on flag
    sound_flag = true;
    
    // Start timer
    chrono = millis();

    // Turn on the right LED's
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
  }
  // Turn OFF if reading is FALSE
  else if (reading < 1 && sound_flag == true)
  {
    sound_flag = false;
    
    // Reset timer
    seconds = 0;
    minutes = 0;
    
    // Turn on the right LED's
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
  } 
}

void get_gas_raw_data(void){

  int reading = analogRead(SMOKE_SENSOR_PIN);

  Serial.print("Gas sensor raw data:");
  Serial.println(reading);
}

void countTime(){
  if (millis() - chrono >= 1000) {
    chrono = millis();
    result();
  }
}

void result() {
  seconds++;
  if (seconds == 60) {
    seconds = 0;
    minutes ++;
  }
}



void print_status_serial()
{
    // Write on serial
  Serial.println("------------------------------");
  Serial.println("        LASER ENGRAVER        ");
  Serial.println("------------------------------");
  Serial.print("         STATUS: ");
  if (sound_flag == 0){
      Serial.println("OFF");
  }
  else
  {
    Serial.println("ON");
  }



  Serial.print("           FAN: ");
  if (smoke_flag == 0){
      Serial.println("OFF");
  }
  else
  {
    Serial.println("ON");
  }

  // OPERATING TIME
  Serial.println("          OPERATING TIME: ");
  Serial.print("            ");
  Serial.print(minutes);
  Serial.print("M : ");
  Serial.print(seconds);
  Serial.println(" S");

  Serial.println("------------------------------");
  Serial.println();
  Serial.println(); 
}
