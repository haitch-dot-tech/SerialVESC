/**
 * SerialVESC.cpp
 * - Library for communicating with the VESC over Serial.
 */

#include <stdint.h>
#include "SerialVESC.hpp"

/**
 * @brief SerialVESC Class
 */
SerialVESC::SerialVESC(void) {}

/**
 * @brief Connect the Serial port for VESC
 * @param port Pointer to Serial port
 * @example &Serial1
 */
void SerialVESC::setSerialPort(Stream *port)
{
	serialPort = port;
}

/**
 * @brief Updates the data struct by asking the VESC for a new message
 * @returns `true` if successful
 */
d SerialVESC::updateVESCData(void)
{
	uint8_t command[1] = {COMM_GET_VALUES};
	uint8_t payload[256];

	packSendPayload(command, 1);

	int len = receiveSerialMessage(payload);

	if (len > 55)
	{
		data.connected = true;
		readPacket(payload);
	}
	else
	{
		data.connected = false;
	}
	return data;
}

/**
 * @brief      Packs the payload and sends it over Serial
 * @param      payload Payload as `uint8_t` array
 * @param      len Length of payload
 * @return     The number of bytes sent
 */
int SerialVESC::packSendPayload(uint8_t *payload, int len)
{

	uint16_t crcPayload = crc16(payload, len);
	int count = 0;
	uint8_t messageSend[256];

	if (len <= 256)
	{
		messageSend[count++] = 2;
		messageSend[count++] = len;
	}
	else
	{
		messageSend[count++] = 3;
		messageSend[count++] = (uint8_t)(len >> 8);
		messageSend[count++] = (uint8_t)(len & 0xFF);
	}

	memcpy(&messageSend[count], payload, len);

	count += len;
	messageSend[count++] = (uint8_t)(crcPayload >> 8);
	messageSend[count++] = (uint8_t)(crcPayload & 0xFF);
	messageSend[count++] = 3;
	messageSend[count] = '\0';

	if (serialPort != NULL)
		serialPort->write(messageSend, count);

	return count;
}

/**
 * @brief      Receives a message over Serial
 * @param      payloadReceived Received payload as a `uint8_t` array
 * @return     The number of bytes received within the payload
 */
int SerialVESC::receiveSerialMessage(uint8_t *payloadReceived)
{
	if (serialPort == NULL)
		return -1;

	uint16_t counter = 0;
	uint16_t endMessage = 256;
	bool messageRead = false;
	uint8_t messageReceived[256];
	uint16_t len = 0;

	uint32_t timeout = millis() + 100;

	while (millis() < timeout && messageRead == false)
	{
		while (serialPort->available())
		{
			messageReceived[counter++] = serialPort->read();

			if (counter == 2)
			{
				if (messageReceived[0] == 2)
				{
					endMessage = messageReceived[1] + 5;
					len = messageReceived[1];
				}
			}

			if (counter >= sizeof(messageReceived))
			{
				break;
			}

			if (counter == endMessage && messageReceived[endMessage - 1] == 3)
			{
				messageReceived[endMessage] = 0;
				messageRead = true;
				break;
			}
		}
	}

	bool unpacked = false;

	if (messageRead)
	{
		unpacked = unpackPayload(messageReceived, endMessage, payloadReceived);
	}

	if (unpacked)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief      Verifies the message and extracts the payload
 * @param      message The received message
 * @param      len The length of the message
 * @param      payload The final payload ready to extract data from
 * @return     True if the process was a success
 */
bool SerialVESC::unpackPayload(uint8_t *message, int len, uint8_t *payload)
{

	uint16_t crcMessage = 0;
	uint16_t crcPayload = 0;

	crcMessage = message[len - 3] << 8;
	crcMessage &= 0xFF00;
	crcMessage += message[len - 2];

	memcpy(payload, &message[2], message[1]);

	crcPayload = crc16(payload, message[1]);

	if (crcPayload == crcMessage)
	{

		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief      Extracts the data from the received payload
 * @param      message The payload to extract data from
 * @return     `true` if successful
 */
bool SerialVESC::readPacket(uint8_t *message)
{
	COMM_PACKET_ID packetId;
	int32_t ind = 0;

	packetId = (COMM_PACKET_ID)message[0];
	message++;

	switch (packetId)
	{
		// List of commands - https://github.com/vedderb/bldc/blob/43c3bbaf91f5052a35b75c2ff17b5fe99fad94d1/commands.c#L164
		case COMM_GET_VALUES:
			data.tempFET = buffer_get_float16(message, 10.0, &ind);
			data.tempMTR = buffer_get_float16(message, 10.0, &ind);
			data.avMotorCurrent = buffer_get_float32(message, 100.0, &ind);
			data.avInputCurrent = buffer_get_float32(message, 100.0, &ind);
			data.avID = buffer_get_float32(message, 1000.0, &ind);
			data.avIQ = buffer_get_float32(message, 1000.0, &ind);
			data.duty = buffer_get_float16(message, 1000.0, &ind);
			data.erpm = buffer_get_float32(message, 1.0, &ind);
			data.batteryVoltage = buffer_get_float16(message, 10.0, &ind);
			data.ampHoursUsed = buffer_get_float32(message, 10000.0, &ind);
			data.ampHoursCharged = buffer_get_float32(message, 10000.0, &ind);
			data.wattHoursUsed = buffer_get_float32(message, 10000.0, &ind);
			data.wattHoursCharged = buffer_get_float32(message, 10000.0, &ind);
			data.tacho = buffer_get_int32(message, &ind);
			data.tachoAbs = buffer_get_int32(message, &ind);
			data.error = message[ind++];
			data.PIDPos = buffer_get_float32(message, 1000000.0, &ind);
			data.ID = message[ind++];

			return true;
			break;

		default:
			return false;
	}
	return false;
}
