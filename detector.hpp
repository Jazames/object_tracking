#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <vector>

#define BACKGROUND_FRAMES 50

#define THRESHOLD_MAX            100  
#define DETECT_SHADOWS_MAX       1    
#define LEARNING_RATE_MAX        1000 //this gets divided by max in the equation. 
#define MORPH_SHAPE_MAX          2    //enums to a shape
#define EROSION_SIZE_MAX         25   
#define MIN_OBJECT_DIMENSION_MAX 100  


void detectShadowsCallback(int, void* ptr);
void learningRateCallback(int, void* ptr);
void morphShapeCallback(int, void* ptr);
void erosionSizeCallback(int, void* ptr);
void minObjectDimensionCallback(int, void* ptr);

class Detector
{
public:
	Detector();
	void setBackground();
	cv::Mat getNewFrame();
	cv::Mat getForegroundMask(cv::Mat frame, double learning_rate);
	cv::Mat filterMask(cv::Mat fgMask);
	std::vector<cv::Rect> findObjectBoundaries(cv::Mat img);
	std::vector<cv::Point> findObjectCenters(cv::Mat img, int min_dimension);
	std::vector<cv::Point> findObjectBases(cv::Mat img, int min_dimension);
	std::vector<cv::Point> getBasesFromNewFrame();


	//public because it's easier to access them if they're public. 
	int mThreshold;
	bool mDetectShadows;
	int mDetectShadows_int;
	double mLearningRate;
	int mLearningRate_int; //value from 0.0 to 1.0. values < 0 indicate to use a predefined value. 
	cv::MorphShapes mMorphShape;      //Enum
	int mMorphShape_int;
	int mErosionSize;     
	int mMinObjectDimension; 

private:
	cv::VideoCapture mVidCap;
	bool mCameraOpen;
	cv::Ptr<cv::BackgroundSubtractor> mpBackSub;

	void thresholdCallback(int, void*);
	void detectShadowsCallback(int, void*);
	void learningRateCallback(int, void*);
	void morphShapeCallback(int, void*);
	void erosionSizeCallback(int, void*);
	void minObjectDimensionCallback(int, void*);

	//Tuning parameters
	//number of background frames, 

};




#endif
