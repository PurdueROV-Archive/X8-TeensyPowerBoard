#include "matrices.h"
#include "overseer.h"
#include "thrust_mapper.h"
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <stdlib.h>

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


  overseer.checkForUpdate();

  if (RampTicker >= 20)
  {
    overseer.doRamping();
    RampTicker = 0;
  }

  Serial.print(overseer.is_Overflowing);
  Serial.print(":");
  Serial.print(overseer.getThrustMapper().getThrustMapperMatrices().currentMapperMatrix);
  Serial.print(":");
  Serial.print(overseer.areOverseerAndMapperCommunicating());
  
  
  /*
  Serial.print(overseer.flag_NewData);
  Serial.print(" : ");
  Serial.print(overseer.getThrustMapper().getCurrentForceVector().L.x);
  Serial.print(" : ");
  Serial.print(overseer.getThrustMapper().getCurrentForceVector().L.y);
  Serial.print(" : ");
  Serial.print(overseer.getThrustMapper().getCurrentForceVector().L.z);
  Serial.print(" : ");
  Serial.print(overseer.getThrustMapper().getCurrentForceVector().R.x);
  Serial.print(" : ");
  Serial.print(overseer.getThrustMapper().getCurrentForceVector().R.y);
  Serial.print(" : ");
  Serial.print(overseer.getThrustMapper().getCurrentForceVector().R.z);
  Serial.print("      ");

  Serial.print(overseer.getTargetForce().L.x);
  Serial.print(" : ");
  Serial.print(overseer.getTargetForce().L.y);
  Serial.print(" : ");
  Serial.print(overseer.getTargetForce().L.z);
  Serial.print(" : ");
  Serial.print(overseer.getTargetForce().R.x);
  Serial.print(" : ");
  Serial.print(overseer.getTargetForce().R.y);
  Serial.print(" : ");
  Serial.print(overseer.getTargetForce().R.z);
  Serial.print("      ");*/
  
  Serial.print(" thrust_map: ");
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
  Serial.print(overseer.getThrust_Map().h);

  period = millis();

  while(can.read(message)) {
    
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
          memcpy(&thrusters[3], &(message.buf[1]), 2);
          memcpy(&thrusters[4], &(message.buf[3]), 2);
          memcpy(&thrusters[5], &(message.buf[5]), 2);
    
          char enabled = 255;
          Serial.print(" CAN: ");
          overseer.update(vect6Make(thrusters[0],thrusters[1],thrusters[2],thrusters[3],thrusters[4],thrusters[5]), vect3Make(0,0,0), enabled);
          
          //Serial.print("\n");
          break;
      }
    }
  }
  Serial.println(" ");
}
