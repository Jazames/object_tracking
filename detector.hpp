#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <vector>

#define BACKGROUND_FRAMES 50



class Detector
{
public:
	Detector();
	void setBackground();
	cv::Mat getNewFrame();
	cv::Mat getNewForegroundMask(cv::Mat frame, double learning_rate = 0.001);
	cv::Mat filterMask(cv::Mat fgMask);
	std::vector<cv::Rect> findObjectBoundaries(cv::Mat img);
	std::vector<cv::Point> findObjectCenters(cv::Mat img, int min_dimension);
	std::vector<cv::Point> findObjectBases(cv::Mat img, int min_dimension);
	std::vector<cv::Point> getBasesFromNewFrame();
private:
	cv::VideoCapture mVidCap;
	bool mCameraOpen;
	cv::Ptr<cv::BackgroundSubtractor> mpBackSub;	
};

#endif
