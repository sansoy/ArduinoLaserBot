#include <Firmata.h>
#include <Servo.h>
Servo servo9;
Servo servo10;
//Servo laser12;
int laser12 = 12;

void analogWriteCallback(byte pin, int value)
{
    if(pin == 9)
      servo9.write(value);
    if(pin == 10)
      servo10.write(value);
    if(pin == 12){
        //laser12.write(value);
        switch(value){
          case 1:
            digitalWrite(laser12,HIGH);
            break;
          default:
            digitalWrite(laser12,LOW);
        }
    }
      
}


void setup() 
{
    Firmata.setFirmwareVersion(0, 2);
    Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);

    servo9.attach(9);
    servo10.attach(10);
    //laser12.attach(12);
    pinMode(laser12, OUTPUT);
   
    Firmata.begin(57600);
}

void loop() 
{
    while(Firmata.available())
        Firmata.processInput();
}
