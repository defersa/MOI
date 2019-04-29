#include "opencv2/highgui.hpp" 
#include "opencv2/imgcodecs.hpp" 
#include "opencv2/imgproc.hpp" 
#include <iostream>

using namespace std; 
using namespace cv;

const int sliderMax = 2;
int slider;

Mat original;
Mat changed;
Mat histogram;

void stepOtsu(vector<int> &s, int level, int levels, vector<double> &his, vector<int> &t, vector<double> &W, vector<double> &M, vector<double> &MK, double MT, double curMax, double *max, vector<double> &sumMax) {
	for (; s[level] < 255; s[level]++) {
		int i = s[level];
		W[level] += his[i];
		MK[level] += i * his[i];
		M[level] = MK[level] / W[level];

		s[level + 1] = i + 1;
		if (level < levels - 1) {
			MK[level + 1] = 0;
			W[level + 1] = 0;

			stepOtsu(s, level + 1, levels, his, t, W, M, MK, MT, curMax, &*max, sumMax);
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
			for (int k = 0; k < levels; k++) {
				if (sumMax[s[k]] < curMax) {
					sumMax[s[k]] = curMax;
				}
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

vector<int> multiOtsu(vector<double> &his, int levels, vector<double> &maxd) {

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
	stepOtsu(s, 0, levels, his, thresholds, W, M, MK, MT, curMax, &max, maxd);

	// normalize
	for (int i = 0; i < 256; i++) {
		maxd[i] = maxd[i]*200/ max;
	}

	return thresholds;
}

vector<vector<double>> makeLines(vector<int> th, vector<double> &his, int levels) {

	vector<vector<double>> lines((levels + 1), vector<double>(256, 0));
	double MT = 0;

	for (int i = 0; i < 256; i++) {
		MT += i * his[i];
	}

	for (int i = 0; i < levels; i++) {

		int start = 0;
		if (i != 0)
			start = th[i - 1] + 1;

		int end = 255;
		if (i != levels - 1)
			end = th[i + 1];


		double W = 0;
		double MK = 0;

		double Wmax = 0;
		double MKmax = 0;
	
		for (int j = start; j <= end; j++) {
			Wmax += his[j];
			MKmax += j * his[j];
		}
	
		double sum = 1/0;
		int nth = 0;

		for (int j = start; j <= end; j++) {
			
			W += his[j];
			MK += j * his[j];

			Wmax += -his[j];
			MKmax += -j * his[j];

			double d = 0;
			double M = MK / W;

			double dmax = 0;
			double Mmax = MKmax / Wmax;


			for (int k = start; k <= j; k++) {
				d += (k - M)*(k - M)*his[k]/W;
			}
			for (int k = j + 1; k <= end; k++) {
				dmax += (k - Mmax)*(k - Mmax)*his[k] / Wmax;
			}
			if (W == 0) {
				M = 0;
				d = 0;
			}
			if (Wmax == 0) {
				Mmax = 0;
				dmax = 0;
			}


			lines[i + 1][j] += d;

			if(lines[i][j] > dmax || lines[i][j] == 0)
				lines[i][j] = dmax;

			cout << j << "	" << d*W << "	" << dmax*Wmax << "	" << (d*W + dmax*Wmax) << endl;
		}
		cout << endl;
	}
	cout << endl;

	// normalize
	double max = 0;
	for (int i = 0; i < lines.size(); i++) {
		for (int j = 0; j < 256; j++) {
			if (lines[i][j] > max) {
				max = lines[i][j];
			}
		}
	}
	for (int i = 0; i < lines.size(); i++) {
		for (int j = 0; j < 256; j++) {
			if(lines[i][j] != 0)
				lines[i][j] = lines[i][j] * 200 / max;
		}
	}

	return lines;
}



void createHisto(vector<double> &his, double MN, vector<int> &rs, vector<double> dmax, vector<vector<double>> disp) {

	histogram = Mat(256, 256, CV_8UC3, Scalar(255, 255, 255));
	for (int i = 0; i < 256; i++) {
		int height = (int)his[i] / MN * 256;
		rectangle(
			histogram,
			Point(i, 256),
			Point(1 + i, 256 - height),
			Scalar(128, 128, 128),
			-1
		);
	}
	for (int i = 0; i < rs.size(); i++) {
		line(
			histogram,
			Point(rs[i], 256),
			Point(rs[i], 1),
			Scalar(0, 0, 0),
			1
		);
	}
	for (int i = 0; i < dmax.size() - 1; i++) {
		line(
			histogram,
			Point(i, 256 - (int)dmax[i]),
			Point(i + 1, 256 - (int)dmax[i + 1]),
			Scalar(0, 215, 255),
			1
		);
	}
	vector<Scalar> colors = { Scalar(255, 0, 0), Scalar(255, 255, 0), Scalar(255, 0, 255), Scalar( 0, 255,  0), Scalar(0, 255,  255) };
	for (int i = 0; i < disp.size(); i++) {
		for (int j = 0; j < 255; j++) {
			line(
				histogram,
				Point(j, 255 - (int)disp[i][j]),
				Point(j + 1, 255 - (int)disp[i][j + 1]),
				colors[i],
				1
			);
		}
	}
}

void on_trackbar(int, void*)
{
	int lvl = (int) slider + 1;

	changed = Mat::zeros(original.rows, original.cols, CV_8U);

	vector<double> histo(256, 0);
	vector<double> dmax(256, 0);
	int pixelCount = changed.rows*changed.cols;


	for (int i = 0; i < changed.rows; i++) {
		for (int j = 0; j < changed.cols; j++) {
			int intGray = floor(original.at<Vec3b>(i, j)[2] * 0.2126 + original.at<Vec3b>(i, j)[1] * 0.7152 + original.at<Vec3b>(i, j)[0] * 0.0722);
			changed.at<uchar>(i, j) = intGray;
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

	vector<int> realStep = multiOtsu(histo, lvl, dmax);
	vector<int> colorOfGray(lvl, 0);
	double step = 255 / lvl;
	for (int i = 0; i < lvl; i++) {
		colorOfGray[i] = (int)(step * (i + 1));
	}

	for (int i = 0; i < changed.rows; i++) {
		for (int j = 0; j < changed.cols; j++) {
			int intGray = 0;

			for (int k = 0; k < lvl; k++) {
				if (changed.at<uchar>(i, j) < realStep[k])
					break;
				intGray = colorOfGray[k];
			}
			changed.at<uchar>(i, j) = intGray;
		}
	}
	
	vector<vector<double>> disp = makeLines(realStep, histo, lvl);

	createHisto(histoImg, maxnumber, realStep, dmax, disp);

	imshow("Otsu", changed);
	imshow("Histo", histogram);
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
	namedWindow("Otsu", 1);
	slider = 0;

	/// Create Trackbars
	char TrackbarName[50];
	sprintf(TrackbarName, "Levels x %d", sliderMax + 1);

	createTrackbar(TrackbarName, "Histo", &slider, sliderMax, on_trackbar);
	on_trackbar(slider, 0);



	moveWindow("Histo", 40, 200);
	moveWindow("Otsu", 300, 200);

	waitKey(0);	
	return 0; 
}
