#include "thrusterPWMWrapper.h"

thrusterPWMWrapper::thrusterPWMWrapper(int address, int pUsePWM)
{
  if (!pUsePWM) {
    //esc = new Arduino_I2C_ESC(address);
    myaddress = address;
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
    driver.setPWM(myaddress, 0, i2c_to_PWM(value));
  }
  else
  {
    myservo.write(i2c_to_PWM(value));
  }
}

//the value should be between 63 (full reverse for now) and 123 (full forward for now)
//93 means stopped.  these values will be tuned in the future as more testing is done with
//physical hardware
int thrusterPWMWrapper::i2c_to_PWM(int i2c)
{
  if (i2c == 0) return 93; //force the mapped value to be 0
  return map(i2c, -32768, 32767, 63, 123);
}
