// Parkinson.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include<string>

#include <iostream>

#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\opencv.hpp> 
#include <opencv2/video/background_segm.hpp>
#include <vector>
#include <ctime> 
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>

using namespace cv;
using namespace std;


void ContoursFinder(int, void*);
Mat MainWindows;
Mat MaskMat;
int thresh = 140, maxVal = 153;
int type = 2, Value = 48;
int HandCount = 0 ,   hullcheck = 0, prevVal = 0;
Mat ContourWind, RectangleWind, or2, Edges, MainWind;

vector<int> statsvecctor;
int main() {

	time_t end = time(NULL) + 9.0;

	Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2(20, 16, true);
	cv::Rect MyRectangle(288, 12, 288, 288);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
	Mat frame,x;
	Mat resizeF;
	VideoCapture cap;
	cap.open(0);
	while (time(NULL) <= end)
	{
		Mat MainWind2;
		
		cap.read(MainWindows);
		
		flip(MainWindows, MainWind, 1);
		rectangle(MainWind, MyRectangle, cv::Scalar(0, 0, 255));
		RectangleWind = MainWind(MyRectangle);
		cvtColor(RectangleWind, ContourWind, CV_RGB2GRAY);
		
		GaussianBlur(ContourWind, ContourWind, Size(23, 23), 0); 
		
		pMOG2->apply(RectangleWind, MaskMat);
		cv::rectangle(MaskMat, MyRectangle, cv::Scalar(0, 0, 255));

		ContoursFinder(0, 0);
		
		imshow("MainWindows", MainWind);
		imshow("GreyScale", MaskMat);
		imshow("contours", ContourWind);
		


		waitKey(10);


		}
	putText(MainWindows, to_string(HandCount), cv::Point(100, 300), 1, 2, CV_RGB(255, 0, 0), 2, 8);
	cap.release();
	//videocap.release(cap)
	
	for (int i = 0; i < statsvecctor.size(); i++)
	{
		if (prevVal>=5)
		{
			if (statsvecctor[i] >= 5)
			{
				HandCount++;
			}
		}
		prevVal = statsvecctor[i];


		
		cout << " Contour count = ";
		cout << statsvecctor[i] << endl;
	}


	if (HandCount >= 0 && HandCount <= 4)
	{
		cout << "UPDRS Level = 4" << endl;
	}
	else if (HandCount>4 && HandCount <= 8)
	{
		cout << "UPDRS Level = 3" << endl;
	}
	else if (HandCount > 8 && HandCount <=11)
	{
		cout << "UPDRS Level = 2"<< endl;
	}
	else 
	{
		cout << "UPDRS Level = 1" << endl;
	}
	

	cout << HandCount << endl;

	system("pause");	
	return 0;
}



void ContoursFinder(int, void*) {
	int count = 0;
	char b[40];
	vector<vector<Point> > contours;

	vector<Vec4i> hierarchy;
	
	GaussianBlur(MaskMat, MaskMat, Size(27, 27), 3.5, 3.5);
	


	Canny(MaskMat, Edges, Value, Value * 2, 3); //OR2
	
	findContours(MaskMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0)); //OR2
	Mat GraphContourWind = Mat::zeros(Edges.size(), CV_8UC3); //Edges.size() or2.size()
	if (contours.size() > 0) {
		size_t indexOfBiggestContour = -1;
		size_t sizeOfBiggestContour = 0;
		for (size_t i = 0; i < contours.size(); i++) {
			if (contours[i].size() > sizeOfBiggestContour) {
				sizeOfBiggestContour = contours[i].size();
				indexOfBiggestContour = i;
			}
		}
		vector<vector<int> >hull(contours.size());
		vector<vector<Point> >hullPoint(contours.size()); //hand polygon	
		vector<vector<Vec4i> > defects(contours.size()); //green dots on my fingertips..multi dimensional matrix
		vector<vector<Point> >defectPoint(contours.size()); //point as finger points x, y
		vector<vector<Point> >contours_poly(contours.size()); //moving rectangle surrounding the hand		
		Point2f rect_point[4];
		vector<RotatedRect>minRect(contours.size());
		vector<Rect> boundRect(contours.size());

		//this code i used from opensource Opencv documentation - Mahander Kelash
		for (size_t i = 0; i<contours.size(); i++) {
			if (contourArea(contours[i])>5000) {
				convexHull(contours[i], hull[i], true);
				convexityDefects(contours[i], hull[i], defects[i]);
				if (indexOfBiggestContour == i) {
					minRect[i] = minAreaRect(contours[i]);
					for (size_t k = 0; k<hull[i].size(); k++) {
						int ind = hull[i][k];
						hullPoint[i].push_back(contours[i][ind]);
					}
					//hullcheck++;
					count = 0;

					for (size_t k = 0; k<defects[i].size(); k++) {
						if (defects[i][k][3]>13 * 256) {
							int p_start = defects[i][k][0];
							int p_end = defects[i][k][1];
							int p_far = defects[i][k][2];
							defectPoint[i].push_back(contours[i][p_far]);
							circle(ContourWind, contours[i][p_end], 3, Scalar(0, 255, 0), 2); //i ydi
							count++;
						}

					}
						statsvecctor.push_back(count);
					
					
					

			
						

					putText(MainWind, b, Point(75, 450), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 3, 8, false);

					drawContours(GraphContourWind, contours, i, Scalar(255, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
					drawContours(GraphContourWind, hullPoint, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
					drawContours(ContourWind, hullPoint, i, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
					approxPolyDP(contours[i], contours_poly[i], 3, false);
					boundRect[i] = boundingRect(contours_poly[i]);
					rectangle(ContourWind, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
					minRect[i].points(rect_point);
					for (size_t k = 0; k<4; k++) {
						line(ContourWind, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 255, 0), 2, 8);
					}

				}
			}

		}

	}

	//to see the graph builted by contours we can uncommment following statement"Mahander Kelash"
	imshow("Contour Windows", GraphContourWind);

}
