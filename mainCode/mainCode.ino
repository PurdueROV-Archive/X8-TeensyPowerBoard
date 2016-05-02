#include <i2c_t3.h>
#include "matrices.h"
#include "overseer_teensy.h"
//#include "thrust_mapper.h"
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <stdlib.h>
#include <Servo.h>
//#include "hydraulics.h"
#include "thrusterPWMWrapper.h"
#include "Adafruit_PWMServoDriver.h"

#define MAIN_CAN_ID 0x13
#define TIMEOUT_LIMIT 500 

Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();

thrusterPWMWrapper motor0(1,0); 
thrusterPWMWrapper motor1(2,0);
thrusterPWMWrapper motor2(3,0);
thrusterPWMWrapper motor3(4,0); 
thrusterPWMWrapper motor4(5,0); 
thrusterPWMWrapper motor5(6,0);
thrusterPWMWrapper motor6(7,0);
thrusterPWMWrapper motor7(8,0);

thrusterPWMWrapper unmodified_motors[8] = {motor0, motor1, motor2, motor3, motor4, motor5, motor6, motor7};
thrusterPWMWrapper motors[8] = {motor0, motor1, motor2, motor3, motor4, motor5, motor6, motor7};

int led = 13;
int d = 200;
FlexCAN can(500000);
OverseerT overseer;
//Hydraulics hydraulics = Hydraulics();
uint8_t * hydraulicsTarget;
CAN_message_t message;
int16_t thrusters[6];
volatile uint_fast8_t RampTicker;
long period = 0;
long timeout = 0;
int16_t thrusterRemapping[3];

void setup() {
  //setup the PWM driver

  driver.begin();
  driver.setPWMFreq(1600);

  
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
//  overseer.update(vect6Make(0,0,0,0,0,0), vect3Make(0,0,0), 0); 
  Serial.print("Hi");

  can.begin();

  overseer = OverseerT();
  *hydraulicsTarget = 0;

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  
}

void loop() {
  delay(1);
  Serial.print("Main loop");
  // put your main code here, to run repeatedly:

  //Serial.printf("%d\t%d\t%d\t%d\t%d\t%d\n",thrusters[0],thrusters[1],thrusters[2],thrusters[3],thrusters[4],thrusters[5]);
  
  //digitalWrite(led, LOW);
  


  //overseer.checkForUpdate();

  // If Timeout is exceeded, STOP MOTORS:
  if (timeout >= TIMEOUT_LIMIT)
  {
    //int8_t enabled = 0;
    //overseer.update(vect6Make(thrusters[0],thrusters[1],thrusters[2],thrusters[3],thrusters[4],thrusters[5]), vect3Make(0,0,0), enabled);
    overseer.sendToMotors(vect8Make(0,0,0,0,0,0,0,0));
    timeout++;
  }
  else
    overseer.sendToMotors(vect8Make(thrusters[0], thrusters[1], thrusters[2], thrusters[3], thrusters[4], thrusters[5], thrusters[6], thrusters[7]));
  /*else if (RampTicker >= 20)
  {
    overseer.doRamping();
    RampTicker = 0;
	  //hydraulics.set(hydraulicsTarget);
  }else{
    timeout++;
    RampTicker++;
  }*/
 
  //period = millis();
  if (can.available()) {
  if(can.read(message)) {
    digitalWrite(led, HIGH);
    
    //if message came from main micro
    if (message.id == MAIN_CAN_ID && message.len == 8) {
      switch (message.buf[0]) {
        case 'L': // longitudinal data
          memcpy(&thrusters[0], &(message.buf[1]), 1);
          memcpy(&thrusters[1], &(message.buf[2]), 1);
          memcpy(&thrusters[2], &(message.buf[3]), 1);
          memcpy(&thrusters[3], &(message.buf[4]), 1);
          memcpy(&thrusters[4], &(message.buf[5]), 1);
          memcpy(&thrusters[5], &(message.buf[6]), 1);
          break;
        case 'R': // rotational data
        {
          memcpy(&thrusters[6], &(message.buf[1]), 1);
          memcpy(&thrusters[7], &(message.buf[2]), 1);
          memcpy(hydraulicsTarget, &(message.buf[3]), 1);
    
          //char enabled = 255; //249 to remove motors 6 and 7 -- change to 246 to remove 5 and 8 instead
          Serial.println(" CAN: ");
          //overseer.update(vect6Make(thrusters[0],thrusters[1],thrusters[2],thrusters[3],thrusters[4],thrusters[5]), vect3Make(0,0,0), enabled);
          timeout = 0;
          
          //Serial.print("\n");
        }
          break;
        
        case 'T': //remap the motors 
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
  //digitalWrite(led, LOW);
  Serial.println(" ");
  }
}




