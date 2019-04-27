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
