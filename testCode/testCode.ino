#include <i2c_t3.h>
#include "matrices.h"
#include "overseer.h"
#include "thrust_mapper.h"
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <stdlib.h>
#include <Servo.h>
//#include "hydraulics.h"


#define MAIN_CAN_ID 0x13
#define TIMEOUT_LIMIT 500 

Arduino_I2C_ESC motor0(0x29); 
Arduino_I2C_ESC motor1(0x2a);
Arduino_I2C_ESC motor2(0x2b);
Arduino_I2C_ESC motor3(0x2c);
Arduino_I2C_ESC motor4(0x2d);
Arduino_I2C_ESC motor5(0x2e);
Arduino_I2C_ESC motor6(0x2f);
Arduino_I2C_ESC motor7(0x30);

Arduino_I2C_ESC unmodified_motors[8] = {motor0, motor1, motor2, motor3, motor4, motor5, motor6, motor7};
Arduino_I2C_ESC motors[8] = {motor0, motor1, motor2, motor3, motor4, motor5, motor6, motor7};

int led = 13;
int d = 200;
FlexCAN can(500000);
Overseer overseer;
//Hydraulics hydraulics = Hydraulics();
uint8_t hydraulicsTarget = 0;
CAN_message_t message;
int16_t thrusters[6];
volatile uint_fast8_t RampTicker;
long period = 0;
long timeout = 0;
int16_t thrusterRemapping[3];
int signal;

void setup() {
  // put your setup code here, to run once:

  pinMode(led, OUTPUT);

  //filter.id = 0x11;
  //message.timeout = 100;
  
  
  Serial.begin(115200);
  Serial.print("hi there");

  /*message.len = 8;
  message.id = 0x13;
  for( int idx=0; idx<8; ++idx ) {
    message.buf[idx] = 0x00;
  }*/
  //start i2c
  Serial.println("STARTING\n");
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_400);
  Wire.setDefaultTimeout(3000);
  Serial.println("WIRE STARTED\n");
  overseer.update(vect6Make(0,0,0,0,0,0), vect3Make(0,0,0), 0); 
  Serial.print("Hi");

  can.begin();

  overseer = Overseer();

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  signal = 0;
}

void loop() {

  if ( Serial.available() > 0 ) {
    signal = Serial.parseInt();
  }
  
  motor5.set(signal);

  motor5.update();

  Serial.print("ESC: ");
  if(motor5.isAlive()) Serial.print("OK\t\t"); 
  else Serial.print("NA\t\t");
  Serial.print(signal);Serial.print(" \t\t");  
  Serial.print(motor5.rpm());Serial.print(" RPM\t\t");
  Serial.print(motor5.voltage());Serial.print(" V\t\t");
  Serial.print(motor5.current());Serial.print(" A\t\t");
  Serial.print(motor5.temperature());Serial.print(" `C");
  Serial.println();

  delay(250); // Update at roughly 4 hz for the demo

}
