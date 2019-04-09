#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp" 
#include "opencv2/imgcodecs.hpp" 
#include "opencv2/imgproc.hpp" 
#include <math.h>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;


/// Global Variables
const int alpha_slider_max = 200;
int alpha_slider;
double alpha;
double beta;


/// Matrices to store images
Mat notResized;
Mat original;
Mat changed;
Mat graph;
Mat dst;

/**
 * @function on_trackbar
 * @brief Callback for trackbar
 */
void Contrast(int step)
{
	std::vector<Mat> rgb;
	cv::split(changed, rgb);
	Mat lut(1, 256, CV_8UC1);
	double contrastLevel = double(0 + step) / 100;
	uchar* p = lut.data;
	double d;
	for (int i = 0; i < 256; i++)
	{
		d = ((double(i) / 255 - 0.5)*contrastLevel + 0.5) * 255;
		if (d > 255)
			d = 255;
		if (d < 0)
			d = 0;
		p[i] = d;
	}
	LUT(rgb[0], lut, rgb[0]);
	LUT(rgb[1], lut, rgb[1]);
	LUT(rgb[2], lut, rgb[2]);
	cv::merge(rgb, changed);
}

void Graph(int step) {

	Mat outputImage(266, 266, CV_8UC3, Scalar(255, 255, 255));

	rectangle(
		outputImage,
		Point(5, 5),
		Point(6, 261),
		Scalar(0, 0, 0),
		-1
	);
	rectangle(
		outputImage,
		Point(5, 261),
		Point(261, 260),
		Scalar(0, 0, 0),
		-1
	);
	if (step > 100) {
		double level = (step - 100) * 128 / 100;
		line(
			outputImage,
			Point(level + 5, 261),
			Point(261 - level, 5),
			Scalar(255, 0, 0)
		);
	} else {
		double level = (100 - step) * 128 / 100;
		line(
			outputImage,
			Point(5, 261 - level),
			Point(261, level + 5),
			Scalar(0, 0, 255)
		);
	}

	graph = outputImage.clone();
}

void on_trackbar(int, void*)
{
	int step = (int)alpha_slider;
	
	changed = original.clone();
	
	Contrast(step);
	Graph(step);

	hconcat(original, changed, dst);

	imshow("Linear Blend", dst);

	imshow("Graph", graph);

	moveWindow("Graph", 40, 450);
}


int main(int argc, char** argv)
{
	/// Read image ( same size, same type )
	notResized = cv::imread(argv[1]);

	if (!notResized.data) { printf("Error loading notResized \n"); return -1; }

	original = Mat::zeros(400, 300, CV_8UC3);
	resize(notResized, original, Size(400, 300) );

	/// Initialize values
	alpha_slider = 100;

	/// Create Windows
	namedWindow("Linear Blend", 1);

	/// Create Trackbars
	char TrackbarName[50];
	sprintf(TrackbarName, "Alpha x %d", alpha_slider_max);

	createTrackbar(TrackbarName, "Linear Blend", &alpha_slider, alpha_slider_max, on_trackbar);

	/// Show some stuff
	on_trackbar(alpha_slider, 0);

	/// Wait until user press some key
	waitKey(0);
	return 0;
}