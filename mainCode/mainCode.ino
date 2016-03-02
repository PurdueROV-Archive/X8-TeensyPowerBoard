#include "matrices.h"
#include "overseer.h"
#include "thrust_mapper.h"
#include <FlexCAN.h>
#include <kinetis_flexcan.h>

#define MAIN_CAN_ID 0x13

int led = 13;
int d = 200;
FlexCAN can(500000);
Overseer overseer;
CAN_message_t message;
int16_t thrusters[6];
volatile uint_fast8_t RampTicker;
long period = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(led, OUTPUT);

  //filter.id = 0x11;
  //message.timeout = 0;
  can.begin();
  overseer = Overseer();
  Serial.begin(115200);

  /*message.len = 8;
  message.id = 0x13;
  for( int idx=0; idx<8; ++idx ) {
    message.buf[idx] = 0x00;
  }*/

}

void loop() {
  // put your main code here, to run repeatedly:

  /* blink led
  digitalWrite(led, HIGH);
  delay(d);
  digitalWrite(led, LOW);
  delay(d); */
  
  digitalWrite(led, LOW);

  overseer.checkForUpdate();

  if (RampTicker >= 20)
  {
    overseer.doRamping();
    RampTicker = 0;
  }
  Serial.print("Thrusters: ");
  Serial.print(thrusters[0]);
  Serial.print(" : ");
  Serial.print(thrusters[1]);
  Serial.print(" : ");
  Serial.print(thrusters[2]);
  Serial.print(" : ");
  Serial.print(thrusters[3]);
  Serial.print(" : ");
  Serial.print(thrusters[4]);
  Serial.print(" : ");
  Serial.print(thrusters[5]);
  Serial.print(" : ms: ");
  Serial.print(millis() - period);
  Serial.print("       ");
  
  Serial.print("thrust_map: ");
  Serial.print(overseer.getThrust_Map().a);
  Serial.print(" : ");
  Serial.print(overseer.getThrust_Map().b);
  Serial.print(" : ");
  Serial.print(overseer.getThrust_Map().c);
  Serial.print(" : ");
  Serial.print(overseer.getThrust_Map().d);
  Serial.print(" : ");
  Serial.print(overseer.getThrust_Map().e);
  Serial.print(" : ");
  Serial.print(overseer.getThrust_Map().f);
  Serial.print(" : ");
  Serial.print(overseer.getThrust_Map().g);
  Serial.print(" : ");
  Serial.println(overseer.getThrust_Map().h);

  period = millis();

  while(can.read(message)) {
    
    digitalWrite(led, HIGH);
    
    //if message came from main micro
    if (message.id == MAIN_CAN_ID && message.len == 8) {
      int j = 0;
      switch (message.buf[0]) {
        case 'L': // longitudinal data
          for (int i = 1; i < message.len - 1; i+=2) {
            thrusters[j] = (message.buf[i] << 8) | message.buf[i+1];
            //Serial.print(thrusters[j]);
            //Serial.print("\n");
            j++;
          }
          break;
        case 'R': // rotational data
          j = 3;
          for (int i = 1; i < message.len - 1; i+=2) {
            thrusters[j] = (message.buf[i] << 8) | (message.buf[i+1]);
            //Serial.print(thrusters[j]);
            //Serial.print("\n");
            j++;
          }
          char enabled = 255;

          overseer.update(vect6Make(thrusters[0],thrusters[1],thrusters[2],thrusters[3],thrusters[4],thrusters[5]), vect3Make(0,0,0), enabled);
          
          //Serial.print("\n");
          break;
      }
    }
  }

}
