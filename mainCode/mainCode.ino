#include "matrices.h"
#include "overseer.h"
#include "thrust_mapper.h"
#include <FlexCAN.h>
#include <kinetis_flexcan.h>

#define MAIN_CAN_ID 0x13

int led = 13;
int d = 200;
FlexCAN can(500000);
CAN_message_t message;
uint16_t thrusters[6];

void setup() {
  // put your setup code here, to run once:

  pinMode(led, OUTPUT);

  //filter.id = 0x11;
  //message.timeout = 0;
  can.begin();

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
          //Serial.print("\n");
          break;
      }
    }
  }
}
