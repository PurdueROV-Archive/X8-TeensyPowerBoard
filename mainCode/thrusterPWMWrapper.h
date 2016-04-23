#ifndef thrusterPWMWrapper__H
#define thrusterPWMWrapper__H
#include "Arduino_I2C_ESC.h"
#include "Adafruit_PWMServoDriver.h"
#include <Servo.h>

extern Adafruit_PWMServoDriver driver;

class thrusterPWMWrapper {
  public:
    int usePWM;
    thrusterPWMWrapper(int address, int pUsePWM);
    void set(int value);
    int i2c_to_PWM(int i2c);

  private:
    Arduino_I2C_ESC *esc;
    Servo myservo;
    int myaddress;
};

#endif
