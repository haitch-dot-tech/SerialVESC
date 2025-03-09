/**
 * SerialVESC.hpp
 * - Library for communicating with the VESC over Serial.
 *   Heavily based off SolidGeek/VescUart and a lot of credit goes to them.
 *   - All that has been changed is some names and certain functions.
 *   - Code has also been fixed to work out-of-the-box with PlatformIO which useed to throw compilation errors.
 */

#pragma once

#include <Arduino.h>
#include "datatypes.h"
#include "buffer.h"
#include "crc.h"

struct d
{
	bool connected;
	float tempFET;
	float tempMTR;
	float avMotorCurrent;
	float avInputCurrent;
	float avID;
	float avIQ;
	float duty;
	float erpm;
	float batteryVoltage;
	float ampHoursUsed;
	float ampHoursCharged;
	float wattHoursUsed;
	float wattHoursCharged;
	long tacho;
	long tachoAbs;
	uint8_t error;
	float PIDPos;
	uint8_t ID;
};

class SerialVESC
{
	public:
		SerialVESC(void);

		void setSerialPort(Stream* port);
		d updateVESCData(void);

		d data;

	private: 
		int packSendPayload(uint8_t *payload, int len);
		int receiveSerialMessage(uint8_t * payloadReceived);
		bool unpackPayload(uint8_t * message, int len, uint8_t * payload);
		bool readPacket(uint8_t *message);

		Stream *serialPort = NULL;
};
