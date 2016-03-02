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
uint16_t thrusters[6];
volatile uint_fast8_t RampTicker;

void setup() {
  // put your setup code here, to run once:

  pinMode(led, OUTPUT);

  //filter.id = 0x11;
  //message.timeout = 0;
  can.begin();
  overseer = Overseer();

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

  overseer.checkForUpdate();

  if (RampTicker >= 20)
  {
    overseer.doRamping();
    RampTicker = 0;
  }

  while(can.read(message)) {
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
