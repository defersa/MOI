#include "opencv2/highgui.hpp" 
#include "opencv2/imgcodecs.hpp" 
#include "opencv2/imgproc.hpp" 
#include <iostream>

using namespace std; 
using namespace cv;

int main(int argc, char** argv) 
{ 
	Mat output2, output3;

	Mat image = Mat::zeros(400, 400, CV_8UC3); 
	
	const float kernelData0[] = {
		1, 0,
		0, -1
	}; 

	const float kernelData[] = {
		-1, -2, -1,
		0, 0, 0,
		1, 2, 1
	}; 
	
	const Mat kernel0(2, 2, CV_32FC1, (float *)kernelData0); 
	const Mat kernel(3, 3, CV_32FC1, (float *)kernelData); 

	rectangle(image, Point(50, 150), Point(150, 250), Scalar(127, 127, 127), -1, 8); 
	circle(image, Point(100, 200), 32.0, Scalar(0, 0, 0), -1, 8); 
	rectangle(image, Point(150, 150), Point(250, 250), Scalar(255, 255, 255), -1, 8); 
	circle(image, Point(200, 200), 32.0, Scalar(127, 127, 127), -1, 8); 
	rectangle(image, Point(50, 50), Point(150, 150), Scalar(255, 255, 255), -1, 8); 
	circle(image, Point(100, 100), 32.0, Scalar(0, 0, 0), -1, 32); 
	rectangle(image, Point(150, 50), Point(250, 150), Scalar(0, 0, 0), -1, 8); 
	circle(image, Point(200, 100), 32.0, Scalar(127, 127, 127), -1, 8); 
	rectangle(image, Point(50, 250), Point(150, 350), Scalar(0, 0, 0), -1, 8); 
	circle(image, Point(100, 300), 32.0, Scalar(255, 255, 255), -1, 8); 
	rectangle(image, Point(150, 250), Point(250, 350), Scalar(127, 127, 127), -1, 8); 
	circle(image, Point(200, 300), 32.0, Scalar(255, 255, 255), -1, 8); 
	
	imshow("Image", image); 
	
	filter2D(image, output2, -1, kernel0, cv::Point(-1, -1), 128, cv::BORDER_REPLICATE);
	filter2D(image, output3, -1, kernel, cv::Point(-1, -1), 128, cv::BORDER_REPLICATE);
	
	imshow("result 2X2 filter", output2);
	imshow("result 3X3 filter", output3);
	
	moveWindow("Image", 40, 240);
	moveWindow("result 2X2 filter", 440, 240);
	moveWindow("result 3X3 filter", 840, 240);

	waitKey(0); 
	return 0; 
}