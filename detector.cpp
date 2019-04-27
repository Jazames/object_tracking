#include "detector.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <functional>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
			


void detectShadowsCallback(int, void* ptr)
{
	Detector* that = (Detector*) ptr;
	that->mDetectShadows = that->mDetectShadows_int == 1;
}

void learningRateCallback(int, void* ptr)
{
	Detector* that = (Detector*) ptr;
	that->mLearningRate = (double)that->mLearningRate_int / (double) LEARNING_RATE_MAX;
}

void morphShapeCallback(int, void* ptr)
{
	Detector* that = (Detector*) ptr;
	switch(that->mMorphShape_int)
	{
		case 0:
			that->mMorphShape = cv::MORPH_RECT;
			break;
		case 1:
			that->mMorphShape = cv::MORPH_ELLIPSE;
			break;
		case 2:
			that->mMorphShape = cv::MORPH_CROSS;
			break;
		default: 
			std::cout << "Something broken happened, morph shape is default." << std::endl;
			that->mMorphShape = cv::MORPH_CROSS;
	}
}

void erosionSizeCallback(int, void* ptr)
{
	Detector* that = (Detector*) ptr;
}

void minObjectDimensionCallback(int, void* ptr)
{
	Detector* that = (Detector*) ptr;
}




Detector::Detector() : mVidCap(), mCameraOpen(false), mThreshold(32), mDetectShadows(1), 
	mLearningRate(-1), mMorphShape(cv::MORPH_CROSS), mErosionSize(1), mMinObjectDimension(32)
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
	mpBackSub = cv::createBackgroundSubtractorMOG2(BACKGROUND_FRAMES, mThreshold, mDetectShadows);
}

void Detector::setBackground() 
{
	double learning_rate = -1;//Use the chosen learning rate. 

	for(int i = 0; i < BACKGROUND_FRAMES; i++)
	{
		cv::Mat frame, fgMask;
	
		//mVidCap.read(frame);//TODO: get this carefully
		frame = getNewFrame();
		//frame now has video data. WOOOOO
	
		mpBackSub->apply(frame, fgMask, learning_rate);
       	
		//And then throw away all the images, because we're just training right now. 
	}
}

cv::Mat Detector::getNewFrame()
{
	cv::Mat frame;
	mVidCap.read(frame);
	if(frame.empty())
	{
		std::cout << "Warning, frame failed. Figure out something to do" << std::endl;
	}
	return frame;
}

cv::Mat Detector::getForegroundMask(cv::Mat frame, double learning_rate) 
{
	cv::Mat fgMask;
	
	mpBackSub->apply(frame, fgMask, learning_rate);

	return fgMask;
}

cv::Mat Detector::filterMask(cv::Mat fgMask)
{
	cv::MorphShapes shape = mMorphShape;//cross, rect, ellipse
	cv::MorphTypes morph_type = cv::MORPH_OPEN;
	int erosion_size = mErosionSize;
	cv::Mat element = cv::getStructuringElement(shape, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size));
	cv::Mat newImg;
	cv::morphologyEx(fgMask, newImg, morph_type, element);
	morph_type = cv::MORPH_DILATE;
	cv::morphologyEx(newImg, newImg, morph_type, element);
	return newImg;

}

std::vector<cv::Point> Detector::findObjectCenters(cv::Mat img, int min_dimension)
{
	int mode = cv::RETR_EXTERNAL; 
	int method = cv::CHAIN_APPROX_TC89_L1;
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(img, contours, mode, method, cv::Point());
	
	std::vector<cv::Point> centers;
	for(int i = 0; i < contours.size(); i++) 
	{
		cv::Rect box = cv::boundingRect(contours[i]);
		if(box.width < min_dimension || box.height < min_dimension)
		{
			//Skip this round, the thing is too small;
			continue;
		}
		cv::Moments mu = cv::moments(contours[i]);
		int x, y;
		if(mu.m00 > 0)
		{
			x = mu.m10 / mu.m00;
			y = mu.m01 / mu.m00;
		}
 		else
		{
			x = img.rows;
			y = img.rows;
		}		
		centers.push_back(cv::Point(x,y));
	}
	return centers;
}

std::vector<cv::Rect> Detector::findObjectBoundaries(cv::Mat img)
{
	int mode = cv::RETR_EXTERNAL; 
	int method = cv::CHAIN_APPROX_TC89_L1;
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(img, contours, mode, method, cv::Point());
	
	std::vector<cv::Rect> boundingRect(contours.size());
	for(int i = 0; i < contours.size(); i++) 
	{
		boundingRect[i] = cv::boundingRect(contours[i]);
	}
	return boundingRect;
}



std::vector<cv::Point> Detector::findObjectBases(cv::Mat img, int min_dimension)
{
	int mode = cv::RETR_EXTERNAL; 
	int method = cv::CHAIN_APPROX_TC89_L1;
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(img, contours, mode, method, cv::Point());
	
	std::vector<cv::Point> bases;
	for(int i = 0; i < contours.size(); i++) 
	{
		cv::Rect box = cv::boundingRect(contours[i]);
		if(box.width < min_dimension || box.height < min_dimension)
		{
			//Skip this round, the thing is too small;
			continue;
		}
		cv::Moments mu = cv::moments(contours[i]);
		int x, y;
		if(mu.m00 > 0)
		{
			x = mu.m10 / mu.m00;
			//y = mu.m01 / mu.m00;
		}
 		else
		{
			x = img.rows;
			//y = img.rows;
		}	
		y = box.y + box.height;	
		bases.push_back(cv::Point(x,y));
	}
	return bases;
}


std::vector<cv::Point> Detector::getBasesFromNewFrame()
{	
	//Perform computations. 
	cv::Mat img = getNewFrame();
	cv::Mat fgMask = getForegroundMask(img, mLearningRate);
	fgMask = filterMask(fgMask);
	std::vector<cv::Point> bases = findObjectBases(fgMask, mMinObjectDimension);

	cv::Scalar color = cv::Scalar(0,200,100);

	for(auto base : bases)
	{
		cv::circle(img, base, 4, color, 4);
	}

	//Show stuff for debugging/tuning. 
	cv::imshow("Image", img);
	cv::imshow("Mask", fgMask);

   	//exit window if something. 
	char c = (char) cv::waitKey(1); //Get key press and give time to display image.
	if(c == 27) //Code for excape key 
	{
		exit(EXIT_SUCCESS);
	}

	//Return 
	return bases;
}

/*
//Params for tuning
	int mThreshold
	bool mDetectShadows;
	double mLearningRate; //value from 0.0 to 1.0. values < 0 indicate to use a predefined value. 
	int mMorphShape;      //Enum
	int mErosionSize;     
	int mMinObjectDimension; 
	*/

void Detector::thresholdCallback(int, void*)
{
	mpBackSub = cv::createBackgroundSubtractorMOG2(BACKGROUND_FRAMES, mThreshold, mDetectShadows);
}

void Detector::detectShadowsCallback(int, void*)
{
	mDetectShadows = mDetectShadows_int == 1;
}

void Detector::learningRateCallback(int, void*)
{
	mLearningRate = (double)mLearningRate_int / (double) LEARNING_RATE_MAX;
}

void Detector::morphShapeCallback(int, void*)
{
	switch(mMorphShape_int)
	{
		case 0:
			mMorphShape = cv::MORPH_RECT;
			break;
		case 1:
			mMorphShape = cv::MORPH_ELLIPSE;
			break;
		case 2:
			mMorphShape = cv::MORPH_CROSS;
			break;
		default: 
			std::cout << "Something broken happened, morph shape is default." << std::endl;
			mMorphShape = cv::MORPH_CROSS;
	}
}

void Detector::erosionSizeCallback(int, void*)
{
	
}

void Detector::minObjectDimensionCallback(int, void*)
{
	
}



