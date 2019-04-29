#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include "detector.hpp"
#include <opencv2/core.hpp>
#include <vector>

#define SPRAY_TIME 3000

#define MESSAGE_TYPE_SET_NOZZLE  0X01
#define MESSAGE_TYPE_SET_COLOR   0x02
#define MESSAGE_TYPE_CALIBRATION 0x03

#define START_CALIBRATION_COMMAND 0x01
#define CALIBRATION_COMPLETE_COMMAND 0x02

#define MODE_ON        0x00000001
#define MODE_OFF       0x00000000
#define MODE_RANDOM    0xFF000002
#define MODE_CALIBRATE 0xFF000000
#define MODE_OB_TRACK  0xFF000001

class Communicator
{
public:
	static Communicator& getInstance() {static Communicator instance; return instance;}
	void setDetector(Detector& detector);
	void disableNozzles();
	void disableNozzle(int nozzle);
	void enableNozzle(int nozzle);
	void setNozzlePosition(int nozzle, cv::Point point); 
	void writeToSplashpad(uint8_t message_type, uint8_t* buffer, int payload_length);
	uint16_t calculateFletcher16(uint8_t* data, int length);
	void insertChecksum(uint8_t* buffer, int length);
	void runCommunicator();
	void processSerialInput();
	void parseSerial(uint8_t byte);
	int getMessageLength(uint8_t type);
	void interpretMessage(uint8_t* buffer);
	void interpretCalibrationMessage(uint8_t* buffer);
	void sendCalibrationComplete();
	void generateOutput();
	bool isInRangeOfNozzle(int nozzle, cv::Point point);
private:
	Communicator();
	~Communicator() {}

	Detector& mDetector;
	bool mSpraying;
	std::vector<cv::Point> mNozzleBases;

	int mUARTHandle;
	//uint8_t mNozzleBuffer[98];
	uint32_t mNozzleModes[7];
	uint8_t mIncomingMessageBuffer[100];
	int mIncomingMessageIndex;
	int mIncomingMessageState;
	uint8_t mIncomingMessageType;
	uint16_t mIncomingMessageChecksum; 
};

#endif


/*
Communication Protocol Wumbo. 

TAG [2 bytes]
0xF0
0x0D
MESSAGE Type [1 byte] 
{0x01 = set nozzle, 0x02 set color, 0x03 = calibration}
PAYLOAD [bytes]
CHECKSUM [2 bytes]
0xNN
0xNN

set nozzle payload:
NOZZLE NUM [1 byte]
NOZZLE MODE [4 bytes]

set color payload: 
NOZZLE NUM [1 byte]
RED [2 bytes]
GREEN [2 bytes]
BLUE [2 bytes]

calibration payload:
CODE [1 byte] {0x01 = start calibration, 0x02 = calibration complete}


*/
