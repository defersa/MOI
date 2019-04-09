#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
	Mat inputImage = cv::imread(argv[1]);

	int color_r[256];
	int color_g[256];
	int color_b[256];
	int max = 0;

	for (int i = 0; i < 256; i++) {
		color_r[i] = 0;
		color_g[i] = 0;
		color_b[i] = 0;
	}


	for (int i = 0; i < inputImage.rows; i++) {
		for (int j = 0; j < inputImage.cols; j++) {
			color_b[inputImage.at<Vec3b>(i, j)[0]] += 1;
			color_g[inputImage.at<Vec3b>(i, j)[1]] += 1;
			color_r[inputImage.at<Vec3b>(i, j)[2]] += 1;
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

	Mat outputImage(550, 550, CV_8UC3, Scalar(0, 0, 0));


	// lines for separating
	rectangle(
		outputImage,
		Point(18, 0),
		Point(19, 550),
		Scalar( 255, 255, 255),
		-1
	);
	rectangle(
		outputImage,
		Point(0, 532),
		Point(550, 531),
		Scalar(255, 255, 255),
		-1
	);

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
				Point(22 + i * 2, 528),
				Point(23 + i * 2, 528 - val * 2),
				Scalar( scal[0], scal[1], scal[2]),
				-1
			);
		}
		
	}

	imshow("Histogram of image", outputImage);
	waitKey(0);
	return 0;
}