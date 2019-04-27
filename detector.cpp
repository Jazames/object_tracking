#include "detector.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>

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
	mpBackSub = cv::createBackgroundSubtractorMOG2(BACKGROUND_FRAMES, 64, true);
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

cv::Mat Detector::getNewForegroundMask(cv::Mat frame, double learning_rate=0.001) 
{
	cv::Mat fgMask;
	
	mpBackSub->apply(frame, fgMask, learning_rate);

	return fgMask;
}

cv::Mat Detector::filterMask(cv::Mat fgMask)
{
	cv::MorphShapes shape = cv::MORPH_CROSS;//cross, rect, ellipse
	cv::MorphTypes morph_type = cv::MORPH_OPEN;
	int erosion_size = 2;
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
	//Variables. 
	double learning_rate = 0.001;
	int min_dimension = 32;
	
	//Perform computations. 
	cv::Mat img = getNewFrame();
	cv::Mat fgMask = getForegroundMask(img, learning_rate);
	fgMask = filterMask(fgMask);
	std::vector<cv::Point> bases = findObjectBases(fgMask, min_dimension);

	//Show stuff for debugging. 
	cv::imgshow("Image", img);
	cv::imgshow("Mask", fgMask);


	//Return 
	return bases;
}
