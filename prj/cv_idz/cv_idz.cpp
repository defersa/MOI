#include "opencv2/highgui.hpp" 
#include "opencv2/imgcodecs.hpp" 
#include "opencv2/imgproc.hpp" 
#include <iostream>

using namespace std; 
using namespace cv;

const int sliderMax = 3;
int slider;

Mat original;
Mat changed;

void stepOtsu(vector<int> &s, int level, int levels, vector<double> &his, vector<int> &t, vector<double> &W, vector<double> &M, vector<double> &MK, double MT, double curMax, double *max) {
	for (; s[level] < 255; s[level]++) {
		int i = s[level];
		W[level] += his[i];
		MK[level] += i * his[i];
		M[level] = MK[level] / W[level];

		s[level + 1] = i + 1;
		if (level < levels - 1) {
			MK[level + 1] = 0;
			W[level + 1] = 0;

			stepOtsu(s, level + 1, levels, his, t, W, M, MK, MT, curMax, &*max);
		}
		else {
			double Wsum = 0;
			double MKsum = 0;

			for (int k = 0; k < levels; k++) {
				Wsum += W[k];
				MKsum += MK[k];
			}

			W[level + 1] = 1 - Wsum;
			MK[level + 1] = MT - MKsum;

			if (MK[level + 1] <= 0) break;

			M[level + 1] = MK[level + 1] / W[level + 1];

			curMax = 0;
			for (int k = 0; k < levels + 1; k++) {
				curMax += W[k] * (M[k] - MT)*(M[k] - MT);
			}
			if (*max < curMax) {
				*max = curMax;
				for (int k = 0; k < levels; k++) {
					t[k] = s[k];
				}
			}
		}

	}
}

vector<int> multiOtsu(vector<double> &his, int levels) {

	vector<int> thresholds(levels, 0);
	vector<int> s(levels + 1, 0);

	vector<double> W(levels + 1, 0);
	vector<double> M(levels + 1, 0);
	vector<double> MK(levels + 1, 0);

	double MT = 0;
	double curMax = 0;
	double max = 0;

	for (int i = 0; i < 256; i++) {
		MT += i * his[i];
	}
	stepOtsu(s, 0, levels, his, thresholds, W, M, MK, MT, curMax, &max);

	return thresholds;
}



void on_trackbar(int, void*)
{
	int lvl = (int) slider + 1;

	changed = original.clone();

	vector<double> histo(256, 0);
	int pixelCount = changed.rows*changed.cols;


	for (int i = 0; i < changed.rows; i++) {
		for (int j = 0; j < changed.cols; j++) {
			double gray = changed.at<Vec3b>(i, j)[2] * 0.2126 + changed.at<Vec3b>(i, j)[1] * 0.7152 + changed.at<Vec3b>(i, j)[0] * 0.0722;
			int intGray = floor(gray);
			changed.at<Vec3b>(i, j)[0] = intGray;
			changed.at<Vec3b>(i, j)[1] = intGray;
			changed.at<Vec3b>(i, j)[2] = intGray;
			histo[intGray] += 1;
		}
	}
	
	vector<double> histoImg = histo;
	double maxnumber = 0;

	for (int i = 0; i < 256; i++) {
		histo[i] = histo[i] / pixelCount;
		if (histoImg[i] > maxnumber) {
			maxnumber = histoImg[i];
		}
	}

	vector<int> realStep = multiOtsu(histo, lvl);

	vector<int> colorOfGray(lvl, 0);
	double step = 255 / lvl;

	for (int i = 0; i < lvl; i++) {
		colorOfGray[i] = (int)(step * (i + 1));
	}

	for (int i = 0; i < changed.rows; i++) {
		for (int j = 0; j < changed.cols; j++) {
			int intGray = 0;

			for (int k = 0; k < lvl; k++) {
				if (changed.at<Vec3b>(i, j)[0] < realStep[k])
					break;
				intGray = colorOfGray[k];
			}
			changed.at<Vec3b>(i, j)[0] = intGray;
			changed.at<Vec3b>(i, j)[1] = intGray;
			changed.at<Vec3b>(i, j)[2] = intGray;
		}
	}

	Mat histogram(256, 256, CV_8UC3, Scalar(255, 255, 255));
	for (int i = 0; i < 256; i++) {
		int height = (int)histoImg[i] / maxnumber * 256;
		rectangle(
			histogram,
			Point(i, 256),
			Point(1 + i, 256 - height),
			Scalar(128, 128, 128),
			-1
		);
	}
	for (int i = 0; i < lvl; i++) {
		rectangle(
			histogram,
			Point(realStep[i], 256),
			Point(1 + realStep[i], 1),
			Scalar(0, 0, 0),
			-1
		);
	}

	imshow("Otsu", changed);
	imshow("Histo", histogram);

	moveWindow("Histo", 40, 200);
	moveWindow("Otsu", 300, 200);
}

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

	original = cv::imread(argv[1]);

	/// Create Windows
	namedWindow("Histo", 1);
	slider = 1;

	/// Create Trackbars
	char TrackbarName[50];
	sprintf(TrackbarName, "Levels x %d", sliderMax + 1);

	createTrackbar(TrackbarName, "Histo", &slider, sliderMax, on_trackbar);
	on_trackbar(slider, 0);


	waitKey(0);	
	return 0; 
}
