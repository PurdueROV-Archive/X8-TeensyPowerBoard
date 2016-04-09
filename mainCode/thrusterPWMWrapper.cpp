#include "thrusterPWMWrapper.h"

thrusterPWMWrapper::thrusterPWMWrapper(int address, int pUsePWM)
{
  if (!pUsePWM) {
    esc = new Arduino_I2C_ESC(address);
  }
  else
  {
    myservo.attach(address);
  }
}

void thrusterPWMWrapper::set(int value)
{
  if (!usePWM)
  {
    esc->set(value);
  }
  else
  {
    myservo.write(i2c_to_PWM(value));
  }
}

int thrusterPWMWrapper::i2c_to_PWM(int i2c)
{
  if (i2c == 0) return 93; //force the mapped value to be 0
  return map(i2c, -32768, 32767, 63, 123);
}
