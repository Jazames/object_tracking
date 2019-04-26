#include <cstdlib>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "detector.hpp"

int main()
{
	Detector detector;

	detector.setBackground();
	std::cout << "Trained Background" << std::endl;

	int i = 0;

	while(true)
	{
		cv::Mat fgMask = detector.getNewForegroundMask();
		std::cout << "Showing foreground frame: " << i++ << std::endl;
		cv::imshow("Foreground Mask", fgMask);
	}





	return EXIT_SUCCESS;
}
