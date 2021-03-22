#include <iostream>
#include <fstream>
#include <string>

#include <opencv/cv.h>
#include <opencv2/imgproc.hpp>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdlib.h>
#include <stdio.h>

#include <ctime>
#include <windows.h>
#include <math.h>

#include <vector>
#include <algorithm>

using namespace std;
using namespace cv;

typedef unsigned char uchar;
typedef unsigned int uint;

void invmask(Mat left, Mat right);
void histimg(Mat img, string road, int count);

int main(int argc, char* argv[])
{
	//просто для понимания, в дальнейшем можно использовать просто string file
	string R_origin = "new/";
	string N_origin = "_cutDSC"; //	NUMBER_cutDSC

	string R_mask = "newfx/";
	string N_mask = "cutDSCfx_"; //	cutDSCfx_NUMBER

	string D_png = ".png"; //		DONT FORGET .png!
	string R_folder = "histo/"; //	histo/NUMBER/

	int count = 1;
	while (count <= 44)
	{
		Mat br_origin = imread(R_origin + to_string(count) + N_origin + D_png);
		Mat br_mask = imread(R_mask + N_mask + to_string(count) + D_png);

		invmask(br_origin, br_mask);
		imwrite("orgmsk.png", br_origin);
		
		string road = R_folder + to_string(count) + "/graph.png";
		histimg(br_origin, road, count);

		count++;
	}

	system("pause");
	return 0;
}

void histimg(Mat img, string road, int count)
{
	int i, j = 0;
	int R, G, B = 0;

	int rHist[256];
	int gHist[256];
	int bHist[256];

	for (int x = 0; x < 256; x++) {
		rHist[x] = 0;
		gHist[x] = 0;
		bHist[x] = 0;
	}

	double MOP = 0;

	for (i = 0; i < img.cols; i++) {
		for (j = 0; j < img.rows; j++) {
			Vec3b pxl = img.at<Vec3b>(j, i);
			R = pxl[2];
			G = pxl[1];
			B = pxl[0];

			rHist[R]++;
			gHist[G]++;
			bHist[B]++;

			if (rHist[R] > MOP)
				MOP = rHist[R];
			if (gHist[G] > MOP)
				MOP = gHist[G];
			if (bHist[B] > MOP)
				MOP = bHist[B];
		}
	}	cout << "MOP: " << MOP << endl;

	double pCompressor = MOP / MOP;
	int newWh = round(MOP * pCompressor) + 10;

	Mat graph(newWh, 255, CV_8UC3, Scalar(255, 255, 255));

	int newMOP = round(MOP * pCompressor);

	ofstream out;
	out.open("histo/" + to_string(count) + "/graphRGB.txt");
	out << "Block number " << count << endl;
	out << "----------------- Red:" << endl;
	for (i = 0; i < 255; i++) {
		if (rHist[i] != 0) {
			out << "[" << i << "]: " << rHist[i] << endl;
		}
	}		
	out << "--------------- Green:" << endl;
	for (i = 0; i < 255; i++) {
		if (gHist[i] != 0) {
			out << "[" << i << "]: " << gHist[i] << endl;
		}
	}
	out << "---------------- Blue:" << endl;
	for (i = 0; i < 255; i++)
	{
		if (bHist[i] != 0) {
			out << "[" << i << "]: " << bHist[i] << endl;
		}
	}
	out.close();

	for (i = 1; i < 255; i++)
	{
		int Rcoord = round(rHist[i] * pCompressor);
		int RcoordN = round(rHist[i+1] * pCompressor);
		int Gcoord = round(gHist[i] * pCompressor);
		int GcoordN = round(gHist[i + 1] * pCompressor);
		int Bcoord = round(bHist[i] * pCompressor);
		int BcoordN = round(bHist[i + 1] * pCompressor);
		//cout << Ycoord << endl;

		line(graph, Point(i, newMOP - Rcoord), Point(i + 1, newMOP - RcoordN), Scalar(0, 0, 255), 1, 8, 0);
		line(graph, Point(i, newMOP - Gcoord), Point(i + 1, newMOP - GcoordN), Scalar(0, 255, 0), 1, 8, 0);
		line(graph, Point(i, newMOP - Bcoord), Point(i + 1, newMOP - BcoordN), Scalar(255, 0, 0), 1, 8, 0);
	}

	imwrite(road, graph);
}

void invmask(Mat org, Mat msk)
{
	int i, j = 0;
	int R, G, B = 0;

	for (i = 0; i < msk.cols; i++)	{
		for (j = 0; j < msk.rows; j++)	{
			Vec3b pxl = msk.at<Vec3b>(j, i);
			R = pxl[2];
			G = pxl[1];
			B = pxl[0];
			//cout << "R: " << R << ", G: " << G << ", B: " << B << endl;

			if ((R + G + B) == 0) //if black
				org.at<Vec3b>(j, i) = pxl;
		}
	}
}