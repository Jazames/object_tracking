#include "communicator.hpp"
#include "routines.hpp"
#include <chrono>
#include <cmath>
#include <string>


//I don't really know which ones of these I need, but I'm including them cause sure. 
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */



#define PROXIMITY_RANGE 100

Communicator::Communicator() : mDetector(), mSpraying(false), mNozzleBases(7, cv::Point(0,0)),
	mIncomingMessageState(0), mIncomingMessageIndex(0), mIncomingMessageChecksum(0), mIncomingMessageType(0)
{
	std::string port = "/dev/ttyS0"; //Or something like that. 

	mUARTHandle = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	if(mUARTHandle == -1)
	{
		std::cout << "Warning: Unable to open port " << port << std::endl;
	}
	else
	{
		fcntl(mUARTHandle, F_SETFL, FNDELAY); //Read will return 0 if nothing is available. 
	}
}

void Communicator::setDetector(Detector& detector)
{
	mDetector = detector;
}

void Communicator::disableNozzles();
{
	for(int i=0;i<7;i++)
	{
		disableNozzle(i);
	}
}

void Communicator::disableNozzle(int nozzle);
{
	static uint8_t buf[5];
	mNozzleBuffer[nozzle] = 0;
	buf[0] = (uint8_t) nozzle;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x00;
	writeToSplashpad(buf, 5);
}

void Communicator::enableNozzle(int nozzle);
{
	static uint8_t buf[5];
	mNozzleBuffer[nozzle] = 1;
	buf[0] = (uint8_t) nozzle;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x01;
	writeToSplashpad(buf, 5);
}

void Communicator::setNozzlePosition(int nozzle, cv::Point point); 
{
	mNozzleBases[nozzle] = point;
}

void Communicator::writeToSplashpad(uint8_t message_type, uint8_t* buffer, int payload_length)
{
	//Write out the nozzle buffer. 
	int length = payload_length + 5;
	static uint8_t message[length];
	message[0] = 0xF0;
	message[1] = 0x0D;
	message[2] = message_type;

	//Because this is easier than looking up how to use mem copy
	for(int i = 0; i < payload_length; i++)
	{
		message[i + 3] = buffer[i];
	}
	insertChecksum(message, length - 2);
	write(mUARTHandle, message, length);
}

uint16_t Communicator::calculateFletcher16(uint8_t* data, int length)
{
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;
   int index;

   for(index = 0; index < length; index++)
   {
      sum1 = (sum1 + data[index]) % 0xFF;
      sum2 = (sum2 + sum1) % 0xFF;
   }

   return (sum2 << 8) | sum1;
}

void Communicator::insertChecksum(uint8_t* buffer, int length)
{
	uint16_t checksum = calculateFletcher16(buffer, length);
	buffer[length] = (checksum >> 8) & 0xFF;
	buffer[length + 1] = (checksum)  & 0xFF;
}

void Communicator::runCommunicator()
{
	while(true)
	{
		processSerialInput();
		generateOutput();
	}
}

void Communicator::processSerialInput()
{
	uint8_t byte;
	while(read(mUARTHandle, &byte, 1) > 0) //This assumes read is non-blocking. 
	{
		parseSerial(byte);
	}
}

void Communicator::parseSerial(uint8_t byte)
{
	//uint8_t mIncomingMessageBuffer[100];
	//int mIncomingMessageIndex;
	//int mIncomingMessageState;

	switch(mIncomingMessageState)
	{
		case 0:
			if(byte == 0xF0)
			{
				mIncomingMessageState = 1;
				mIncomingMessageIndex = 0;
				mIncomingMessageBuffer[mIncomingMessageIndex] = byte;
				mIncomingMessageIndex++;
			}
			break;
		case 1: 
			if(byte == 0x0D)
			{
				mIncomingMessageState = 2;
				mIncomingMessageIndex = 0;
			}
			else
			{
				mIncomingMessageState = 0;
				mIncomingMessageBuffer[mIncomingMessageIndex] = byte;
				mIncomingMessageIndex++;
			}
			break;
		case 2:
			mIncomingMessageType = byte;
			mIncomingMessageBuffer[mIncomingMessageIndex] = byte;
			mIncomingMessageIndex++;
			mIncomingMessageState = 3;
			break;
		case 3:
			mIncomingMessageBuffer[mIncomingMessageIndex] = byte;
			mIncomingMessageIndex++;
			if(mIncomingMessageIndex >= getMessageLength(mIncomingMessageType) - 1)
			{
				mIncomingMessageState = 4;
			}
			break;
		case 4: 
			mIncomingMessageChecksum = ((uint16_t)byte << 8);
			break;
		case 5: 
			mIncomingMessageChecksum = mIncomingMessageChecksum | byte;
			uint16_t calcChecksum = calculateFletcher16(mIncomingMessageBuffer, getMessageLength(mIncomingMessageType))
			if(calcChecksum == mIncomingMessageChecksum)
			{
				interpretMessage(mIncomingMessageBuffer); 
			}
			mIncomingMessageState = 0;
			mIncomingMessageIndex = 0;
			break;
		default:
			mIncomingMessageState = 0;
			mIncomingMessageIndex = 0;
	}
}

int getMessageLength(uint8_t type)
{
  switch(type)
  {
    case MESSAGE_TYPE_CALIBRATION:
      return 3 + 1;
      break;
    case MESSAGE_TYPE_SET_NOZZLE:
      return 3 + 5;
      break;
    case MESSAGE_TYPE_SET_COLOR:
      return 3 + 7;
      break;

    default:
      return 0;
  }
}

void Communicator::interpretMessage(uint8_t* buffer)
{
	switch(mIncomingMessageType)
	{
		case MESSAGE_TYPE_SET_NOZZLE:
			//do nothing, I don't want this kind of message here. 
			break;
		case MESSAGE_TYPE_SET_COLOR:
			//do nothing, I don't want this kind of message here. 
			break;
		case MESSAGE_TYPE_CALIBRATION:
			interpretCalibrationMessage(buffer);
			break;
		default:
		//do nothing. 
	}
}

void Communicator::interpretCalibrationMessage(uint8_t* buffer)
{
	uint8_t code = buffer[3]; //check the code. 
	if(code == START_CALIBRATION_COMMAND)
	{
		calibrateObjectTracker(mDetector);
	}
}

void Communicator::sendCalibrationComplete()
{
	static uint8_t command = CALIBRATION_COMPLETE_COMMAND;
	writeToSplashpad(&command, 1);
}

void Communicator::generateOutput();
{
	static auto cooldown_start = std::chrono::system_clock::now();
	auto current_time = std::chrono::system_clock::now();

	std::chrono::duration<double> duration = current_time - cooldown_start;
	std::chrono::milliseconds spray_time(SPRAY_TIME);

	if(duration > spray_time && mSpraying)
	{
		disableNozzles();
		mSpraying = false; 	
		cooldown_start = std::chrono::system_clock::now();
		return;
	}
	else if(duration < spray_time && !mSpraying)
	{
		//Need to wait for everything to settle before checking for objects.
		return;
	}
	else if(mSpraying)
	{
		//Currently hosing something, should return.
		return; 
	}

	auto object_bases = mDetector.getBasesFromNewFrame();
	for(auto base : object_bases)
	{
		//Check if any of the points are near a nozzle. 
		for(int i = 0; i < 7; i++)
		{
			if(isInRangeOfNozzle(i, base))
			{
				enableNozzle(i);
				mSpraying = true;
				cooldown_start = std::chrono::system_clock::now();
			}
		}
	}
	writeToSplashpad();
}

bool Communicator::isInRangeOfNozzle(int nozzle, cv::Point point)
{
	cv::Point nozzle_point = mNozzleBases[nozzle];
	int x1 = nozzle_point.x;
	int y1 = nozzle_point.y;
	int x2 = point.x;
	int y2 = point.y;

	int distance = std::sqrt((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));
	return distance < PROXIMITY_RANGE;
}