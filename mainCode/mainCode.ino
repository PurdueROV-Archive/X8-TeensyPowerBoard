#include <Wire.h>
#include "matrices.h"
#include "overseer.h"
#include "thrust_mapper.h"
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <stdlib.h>
#include "hydraulics.h"


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
Hydraulics hydraulics = Hydraulics();
uint8_t hydraulicsTarget = 0;
CAN_message_t message;
int16_t thrusters[6];
volatile uint_fast8_t RampTicker;
long period = 0;
long timeout = 0;
int16_t thrusterRemapping[3];

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
  Serial.print("STARTING\n");
  Wire.begin();
  Serial.print("WIRE STARTED\n");
  //overseer.update(vect6Make(0,0,0,0,0,0), vect3Make(0,0,0), 0); 
  Serial.print("Hi");

  can.begin();

  

  overseer = Overseer();
}

void loop() {
  delay(1);
  //Serial.print("Main loop");
  // put your main code here, to run repeatedly:

  
  //digitalWrite(led, LOW);
  



  //Serial.print("Hello");

  overseer.checkForUpdate();

  if (timeout >= TIMEOUT_LIMIT)
  {
    int8_t enabled = 0;
    overseer.update(vect6Make(thrusters[0],thrusters[1],thrusters[2],thrusters[3],thrusters[4],thrusters[5]), vect3Make(0,0,0), enabled);
    timeout++;
  }
  else if (RampTicker >= 20)
  {
    overseer.doRamping();
    RampTicker = 0;
	hydraulics.set(hydraulicsTarget);
  }else{
    timeout++;
    RampTicker++;
  }

 

  //period = millis();
  if (can.available()) {
  if(can.read(message)) {
    digitalWrite(led, HIGH);
    
    //if message came from main micro
    if (message.id == MAIN_CAN_ID && message.len == 8) {
      switch (message.buf[0]) {
        case 'L': // longitudinal data
          memcpy(&thrusters[0], &(message.buf[1]), 2);
          memcpy(&thrusters[1], &(message.buf[3]), 2);
          memcpy(&thrusters[2], &(message.buf[5]), 2);
          break;
        case 'R': // rotational data
        {
          memcpy(&thrusters[3], &(message.buf[1]), 2);
          memcpy(&thrusters[4], &(message.buf[3]), 2);
          memcpy(&thrusters[5], &(message.buf[5]), 2);
    
          char enabled = 249; //remove motors 6 and 7 -- change to 246 to remove 5 and 8 instead
          Serial.print(" CAN: ");
          overseer.update(vect6Make(thrusters[0],thrusters[1],thrusters[2],thrusters[3],thrusters[4],thrusters[5]), vect3Make(0,0,0), enabled);
          timeout = 0;
          //Serial.print("\n");
        }
          break;
        
        case 'T': //remap the horizontal motors 
        {
          memcpy(&thrusterRemapping[0],&(message.buf[1]), 2);
          memcpy(&thrusterRemapping[1],&(message.buf[2]), 2);
          memcpy(&thrusterRemapping[2],&(message.buf[3]), 2);

          int i = 0;
          int val = 0;
          for(i = 0; i < 8; i++)
          {
             val = 4 * (thrusterRemapping[2] && 1 << i) + 2 * (thrusterRemapping[1] && 1 << i) + (thrusterRemapping[0] && 1 << i);
             motors[i] = unmodified_motors[val];
          }
         }
             
          break;
          
      }
    }
  }
  digitalWrite(led, LOW);
  Serial.print(" ");
  }
}
