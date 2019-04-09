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
Mat histogramm;
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

void Histo() {
	int color_r[256];
	int color_g[256];
	int color_b[256];
	int max = 0;

	for (int i = 0; i < 256; i++) {
		color_r[i] = 0;
		color_g[i] = 0;
		color_b[i] = 0;
	}


	for (int i = 0; i < changed.rows; i++) {
		for (int j = 0; j < changed.cols; j++) {
			color_b[changed.at<Vec3b>(i, j)[0]] += 1;
			color_g[changed.at<Vec3b>(i, j)[1]] += 1;
			color_r[changed.at<Vec3b>(i, j)[2]] += 1;
		}
	}
	for (int i = 0; i < 256; i++) {
		if (max < color_r[i])
			max = color_r[i];
		if (max < color_g[i])
			max = color_g[i];
		if (max < color_b[i])
			max = color_b[i];
	}
	for (int i = 0; i < 256; i++) {
		color_r[i] = ceil(255.0 * color_r[i] / max);
		color_g[i] = ceil(255.0 * color_g[i] / max);
		color_b[i] = ceil(255.0 * color_b[i] / max);
	}

	Mat outputImage(512, 512, CV_8UC3, Scalar(0, 0, 0));



	// histogram
	for (int i = 0; i < 256; i++) {
		int buf[] = { color_b[i], color_g[i], color_r[i] };
		int scal[] = { 0, 0, 0 };

		for (int j = 0; j < 3; j++) {

			int val = 0;
			int ind = 0;

			for (int k = 0; k < 3; k++) {
				if (val < buf[k]) {
					val = buf[k];
					ind = k;
				}
			}
			buf[ind] = 0;
			scal[ind] = 255;

			rectangle(
				outputImage,
				Point(i * 2, 512),
				Point(i * 2, 512 - val * 2),
				Scalar(scal[0], scal[1], scal[2]),
				-1
			);
		}
	}
	histogramm = outputImage.clone();
}

void on_trackbar(int, void*)
{
	int step = (int)alpha_slider;
	
	changed = original.clone();
	
	Contrast(step);
	Histo();

//	addWeighted(newimage, alpha, newimage, beta, 0.0, dst);

	hconcat(original, changed, dst);

	imshow("Linear Blend", dst);

	imshow("Histo", histogramm);

	moveWindow("Histo", 40, 450);
}


int main(int argc, char** argv)
{
	/// Read image ( same size, same type )
	notResized = cv::imread(argv[1]);

	if (!notResized.data) { printf("Error loading src1 \n"); return -1; }

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