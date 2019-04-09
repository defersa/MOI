#include "opencv2/highgui.hpp" 
#include "opencv2/imgcodecs.hpp" 
#include "opencv2/imgproc.hpp" 
#include <iostream>

using namespace std; 
using namespace cv;


int otsu(double his[], double sum) {

	double sumF = 0;
	double sumS = sum;

	double pF = 0;
	double pS = 0;

	double max = 0;
	double newMax = 0;

	double mF = 0;
	double mS = 0;

	int result = 0;


	for (int i = 0; i < 256; i++) {

		pF += his[i];
		if (pF == 0)
			continue;

		pS = 1 - pF;
		if (pS == 0) {
			break;
		}
			
		sumF += i * his[i];

		mF = sumF / pF;
		mS = (sumS - sumF) / pS;

		newMax = pF * pS * (mF - mS) * (mF - mS);

		if (newMax > max) {
			max = newMax;
			result = i;
		}
	}

	return result;
}

int main(int argc, char** argv) 
{

	double histo[256];

	Mat inputImage = cv::imread(argv[1]);

	Mat image = Mat::zeros(inputImage.rows, inputImage.cols, CV_8UC3);

	for (int i = 0; i < 256; i++) {
		histo[i] = 0;
	}

	for (int i = 0; i < inputImage.rows; i++) {
		for (int j = 0; j < inputImage.cols; j++) {
			double gray = inputImage.at<Vec3b>(i, j)[2] * 0.2126 + inputImage.at<Vec3b>(i, j)[1] * 0.7152 + inputImage.at<Vec3b>(i, j)[0] * 0.0722;
			int intGray = floor(gray);
			image.at<Vec3b>(i, j)[0] = intGray;
			image.at<Vec3b>(i, j)[1] = intGray;
			image.at<Vec3b>(i, j)[2] = intGray;
			histo[intGray] += 1;
		}
	}

	int pixelCount = image.rows*image.cols;
	double sum = 0;

	for (int i = 0; i < 256; i++) {
		histo[i] = histo[i] / pixelCount;
		sum += histo[i]*i;
	}
	


	imshow("Original", inputImage);

	imshow("Gray", image);

	moveWindow("Gray", 20, 20);
	
	int sep = otsu(histo, sum);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			int intGray = 255;

			if (sep > image.at<Vec3b>(i, j)[0])
				intGray = 0;

			image.at<Vec3b>(i, j)[0] = intGray;
			image.at<Vec3b>(i, j)[1] = intGray;
			image.at<Vec3b>(i, j)[2] = intGray;
		}
	}


	imshow("Otsu", image);
	
	waitKey(0); 
	
	return 0; 
}
