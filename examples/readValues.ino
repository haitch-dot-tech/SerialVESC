#include <Arduino.h>
#include <SerialVESC.hpp>

d data;

SerialVESC vesc;

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);

    vesc.setSerialPort(&Serial1);

    Serial.print("SerialVESC Demo\n\n");
}

void loop()
{
    data = vesc.updateVESCData();

    if (data.connected == true)
    {
        Serial.print("VESC Connected\n");
        Serial.printf("FET Temp: %02f", d.tempFET);
    }
}
