#include "detector.hpp"
#include <iostream>
#include <cstdlib>

Detector::Detector() : mVidCap(), mCameraOpen(false) 
{
	mCameraOpen = mVidCap.open(0);
	if(mCameraOpen) 
	{
		std::cout << "Success Opening Camera." << std::endl;
	}
	else
	{
		std::cout << "Failure Opening Camera." << std::endl;
		exit(EXIT_FAILURE);
	}
	//Might need to select a different kind of background subtractor.
	mpBackSub = cv::createBackgroundSubtractorMOG2();
}

void Detector::setBackground() 
{
	double learning_rate = -1;//Use the chosen learning rate. 

	for(int i = 0; i < BACKGROUND_FRAMES; i++)
	{
		cv::Mat frame, fgMask;
	
		mVidCap.read(frame);//TODO: get this carefully
		//frame now has video data. WOOOOO
	
		mpBackSub->apply(frame, fgMask, learning_rate);
       	
		//And then throw away all the images, because we're just training right now. 
	}
}



cv::Mat Detector::getNewForegroundMask() 
{
	double learning_rate = 0;
	cv::Mat frame, fgMask;
	mVidCap.read(frame);	//TODO: get this carefully
	
	mpBackSub->apply(frame, fgMask, learning_rate);

	return fgMask;
}
