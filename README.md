# SerialVESC
This library is an alternative to [SolidGeek's VescUart](https://github.com/SolidGeek/VescUart) library that is smaller and removes some of the unnecessary fuctions and the compliation errors that it had in PlatformIO.

## Setup
Create a SerialVESC object and give it a Serial port.
Note: Make a data structure for the library to put it's data in at the top of your code
  
```cpp
#include <SerialVESC.h>

d data;

SerialVESC vesc;

void setup() {
    Serial1.begin(115200);
    vesc.setSerialPort(&Serial1);
}
```

## Usage  
Getting VESC data requires storing the updateVESCData() result to a struct `d`. You can then read the values out of this structure.
  
```cpp
void loop()
{
    data = vesc.updateVESCData();
    if (data.connected)
    {
        Serial.printf("ERPM: %d", data.erpm);
    }
}
```