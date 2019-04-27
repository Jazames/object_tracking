#include <cstdlib>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv2/imgcodecs.hpp>

#include "detector.hpp"





int main()
{
	Detector detector;

	detector.setBackground();
	std::cout << "Trained Background" << std::endl;

	int i = 0;

	//setup tuning interface. 
	std::string tuning_window_name = "Tuning";
	cv::namedWindow(tuning_window_name, cv::WINDOW_AUTOSIZE);

	char threshold_trackbar_name[50];
   	sprintf(threshold_trackbar_name, "Threshold x %d", THRESHOLD_MAX);

	char detect_shadows_trackbar_name[50];
   	sprintf(detect_shadows_trackbar_name, "Detect Shadows x %d", DETECT_SHADOWS_MAX);

	char learning_rate_trackbar_name[50];
   	sprintf(learning_rate_trackbar_name, "Learning Rate x %d", LEARNING_RATE_MAX);

	char morph_shape_trackbar_name[50];
   	sprintf(morph_shape_trackbar_name, "Morph Shape x %d", MORPH_SHAPE_MAX);

	char erosion_size_trackbar_name[50];
   	sprintf(erosion_size_trackbar_name, "Erosion Size x %d", EROSION_SIZE_MAX);

	char min_object_dimension_trackbar_name[50];
   	sprintf(min_object_dimension_trackbar_name, "Min Object Dimension x %d", MIN_OBJECT_DIMENSION_MAX);

   	//variables because is broken. 
   	int threshold;
	int detectShadows_int;
	int learningRate_int; //value from 0.0 to 1.0. values < 0 indicate to use a predefined value. 
	int morphShape_int;
	int erosionSize;     
	int minObjectDimension; 

	cv::createTrackbar(threshold_trackbar_name, tuning_window_name, &threshold, THRESHOLD_MAX, thresholdCallback, &detector);
   	cv::createTrackbar(detect_shadows_trackbar_name, tuning_window_name, &detectShadows_int, DETECT_SHADOWS_MAX, detectShadowsCallback, &detector);
   	cv::createTrackbar(learning_rate_trackbar_name, tuning_window_name, &learningRate_int, LEARNING_RATE_MAX, learningRateCallback, &detector);
   	cv::createTrackbar(morph_shape_trackbar_name, tuning_window_name, &morphShape_int, MORPH_SHAPE_MAX, morphShapeCallback, &detector);
   	cv::createTrackbar(erosion_size_trackbar_name, tuning_window_name, &erosionSize, EROSION_SIZE_MAX, erosionSizeCallback, &detector);
   	cv::createTrackbar(min_object_dimension_trackbar_name, tuning_window_name, &minObjectDimension, MIN_OBJECT_DIMENSION_MAX, minObjectDimensionCallback, &detector);

   	cv::imshow(tuning_window_name);
   	cv::waitKey(25);


	while(true)
	{
		/*
		cv::Mat fgMask = detector.getNewForegroundMask();
		fgMask = detector.filterMask(fgMask);	
	
		std::vector<cv::Rect> boundingBoxes = detector.findObjectBoundaries(fgMask);	
		cv::Scalar color = cv::Scalar(200);

		auto centers = detector.findObjectCenters(fgMask, 32);	
		auto bases = detector.findObjectBases(fgMask, 32);

		for(int i = 0; i < boundingBoxes.size(); i++)
		{
			cv::rectangle(fgMask, boundingBoxes[i].tl(), boundingBoxes[i].br(), color, 2); 
		}
		
		for(auto center : centers) 
		{
			cv::circle(fgMask, center, 8, color);
		}

		for(auto base : bases)
		{
			cv::circle(fgMask, base, 4, color, 4);
		}

		std::cout << "Showing foreground frame: " << i++ << std::endl;
		cv::imshow("Foreground Mask", fgMask);
		char c = (char) cv::waitKey(25); //Get key press and give time to display image.
		if(c == 27) //Code for excape key 
		{
			break;
		}
		*/

		auto bases = detector.getBasesFromNewFrame();
		for(auto base : bases)
		{
			std::cout << "Base found at x=" << base.x << ", y=" << base.y << std::endl;
		}
	}





	return EXIT_SUCCESS;
}
