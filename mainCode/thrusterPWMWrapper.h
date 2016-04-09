#include "Arduino_I2C_ESC.h"
#include <Servo.h>

class thrusterPWMWrapper {
  public:
    int usePWM;
    thrusterPWMWrapper(int address, int pUsePWM);
    void set(int value);
    int i2c_to_PWM(int i2c);

  private:
    Arduino_I2C_ESC *esc;
    Servo myservo;
};


