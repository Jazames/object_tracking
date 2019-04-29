#include "routines.hpp"
#include "communicator.hpp"
#include <thread>
#include <chrono>
#include <vector> 
#include <opencv2/core.hpp>

#define NUM_DATA_POINTS 50

void calibrateObjectTracker(Detector& detector) //Maybe this can never be null? 
{ 
	if(detector == nullptr)
	{
		return;
	}
	Communicator communicator = Communicator::getInstance();
	
	//Turn everything off. 
	communicator.disableNozzles();
	std::this_thread::sleep_for(std::chrono::milliseconds(STATE_CHANGE_TIME));

	detector.setBackground();

	for(int i=0; i<7; i++)
	{
		//Turn on a nozzle.  
		communicator.enableNozzle(i);
		//communicator.writeToSplashpad();
		std::this_thread::sleep_for(std::chrono::milliseconds(STATE_CHANGE_TIME));

		//get the location of the nozzle. 
		std::vector<cv::Point> points;
		for(int j=0; j<NUM_DATA_POINTS; j++)
		{
			auto bases = detector.getBasesFromNewFrame();
			for(auto base : bases)
			{
				points.push_back(base);
			}
		}
		//Average the points. 
		int x_sum = 0;
		int y_sum = 0;
		int num_data = points.size();
		for(auto point : points)
		{
			x_sum += point.x;
			y_sum += point.y;
		}
		cv::Point nozzle_base = cv::Point(x_sum/num_data, y_sum/num_data);

		//set the location of the nozzle. 
		communicator.setNozzlePosition(i, nozzle_base);
		communicator.disableNozzle(i);
		//communicator.writeToSplashpad();
		std::this_thread::sleep_for(std::chrono::milliseconds(STATE_CHANGE_TIME));
	}

	//I guess it's all calibrated now. 
	communicator.sendCalibrationComplete();
}