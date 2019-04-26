#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#define BACKGROUND_FRAMES 50



class Detector
{
public:
	Detector();
	void setBackground();
	cv::Mat getNewForegroundMask();
private:
	cv::VideoCapture mVidCap;
	bool mCameraOpen;
	cv::Ptr<cv::BackgroundSubtractor> mpBackSub;	
};

#endif
