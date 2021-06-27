#include "./Pins.h"
#include "./System.h"

byte var_global1 = 0;
boolean var_global2 = true;

//Setup da serial
void InitSerial()
{
	Serial.begin(115200);
}

//Setup do arduino
void InitPinMode()
{
}
