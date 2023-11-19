#include <iostream>
#include <queue>
#include <stack>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <fstream>
using namespace std;
using namespace cv;
Mat imgHSV, mask;
int hmin = 0, smin = 0, vmin = 126;
int hmax = 111, smax = 300, vmax = 2555;
float w = 600, h = 600;
Mat matrix, imgWarp;
int standardize(int x)
{
	if (x % 100 >= 40 && x % 100 <= 60) x = (x / 100) * 100 + 50;
	else if (x % 100 >= 90) x = (x + 10) / 100 * 100;
	else if (x % 100 <= 10) x = x / 100 * 100;
	return x;
}
struct locationPoint {
	int row;
	int col;
	locationPoint(int x, int y) {
		this->row = x;
		this->col = y;
	}

	bool operator!=(const locationPoint& rhs) {
		if (this->row != rhs.row || this->col != rhs.col)
			return true;
		return false;
	}

	bool operator==(const locationPoint& rhs) const {
		if (this->row == rhs.row && this->col == rhs.col)
			return true;
		return false;
	}
};
locationPoint getAdjacentNotVisitedNode(int** mark, locationPoint point, int m, int n, locationPoint endP) {
	locationPoint resP(-1, -1);

	if (point.row - 1 >= 0) {
		if (mark[point.row - 1][point.col] == 0 || mark[point.row][point.col] + 1 < mark[point.row - 1][point.col]) {
			resP.row = point.row - 1;
			resP.col = point.col;
			return resP;
		}
	}
	if (point.col + 1 < n) {
		if (mark[point.row][point.col + 1] == 0 || mark[point.row][point.col] + 1 < mark[point.row][point.col + 1])
		{
			resP.row = point.row;
			resP.col = point.col + 1;
			return resP;
		}
	}
	if (point.row + 1 < m) {
		if (mark[point.row + 1][point.col] == 0 || mark[point.row][point.col] + 1 < mark[point.row + 1][point.col]) {
			resP.row = point.row + 1;
			resP.col = point.col;
			return resP;
		}
	}
	if (point.col - 1 >= 0) {
		if (mark[point.row][point.col - 1] == 0 || mark[point.row][point.col] + 1 < mark[point.row][point.col - 1]) {
			resP.row = point.row;
			resP.col = point.col - 1;
			return resP;
		}
	}
	return resP;
}
void mazePath(void* maze, int m, int n, locationPoint& startP, locationPoint endP, stack<locationPoint>& pointStack, vector<locationPoint>& vecPath)
{
	int** maze2d = new int* [m];
	for (int i = 0; i < m; ++i)
	{
		maze2d[i] = (int*)maze + i * n;
	}
	if (maze2d[startP.row][startP.col] == -1 || maze2d[endP.row][endP.col] == -1) {
		return;
	}
	int** mark = new int* [m];
	for (int i = 0; i < m; ++i) {
		mark[i] = new int[n];
	}
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			mark[i][j] = *((int*)maze + i * n + j);
		}
	}
	if (startP == endP) {
		vecPath.push_back(startP);
		return;
	}
	vector<locationPoint> visitedEndPointPreNodeVec;
	pointStack.push(startP);
	mark[startP.row][startP.col] = 1;
	while (pointStack.empty() == false) {
		locationPoint adjacentNotVisitedNode = getAdjacentNotVisitedNode(mark, pointStack.top(), m, n, endP);
		if (adjacentNotVisitedNode.row == -1) {
			pointStack.pop();
			continue;
		}
		if (adjacentNotVisitedNode == endP) {
			mark[adjacentNotVisitedNode.row][adjacentNotVisitedNode.col] = mark[pointStack.top().row][pointStack.top().col] + 1;
			pointStack.push(endP);
			stack<locationPoint> pointStackTemp = pointStack;
			vecPath.clear();
			while (pointStackTemp.empty() == false) {
				vecPath.push_back(pointStackTemp.top());
				pointStackTemp.pop();
			}
			pointStack.pop();
			continue;
		}
		mark[adjacentNotVisitedNode.row][adjacentNotVisitedNode.col] = mark[pointStack.top().row][pointStack.top().col] + 1;
		pointStack.push(adjacentNotVisitedNode);
	}
}
int maze[6][6] = { 0 };
int main() {
	char path_manip[100];
	char send_buf[100];
	char recv_buf[100];
	int pathnum = 0;
	VideoCapture cap(0);
	Mat img;
	cap >> img;
	Point2f src[4] = { {185, 75}, {565, 35}, {220, 420}, {570, 410} };
	Point2f dst[4] = { {0.0f, 0.0f}, {w, 0.0f}, {0.0f, h}, {w, h} };
	matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));
	cvtColor(imgWarp, imgHSV, COLOR_BGR2HSV);
	Scalar lower(hmin, smin, vmin);
	Scalar upper(hmax, smax, vmax);
	inRange(imgHSV, lower, upper, mask);
	Mat imBin;
	threshold(mask, imBin, 125, 255, THRESH_BINARY);
	vector<vector<Point>> contours;
	findContours(imBin, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); ++i) {
		double area = contourArea(contours[i]);
		double length = arcLength(contours[i], true);
		if (length > 300 && length < 1000)
		{
			drawContours(imBin, contours, i, Scalar(255, 0, 255), 2, 2);
			Moments mu = moments(contours[i]);
			Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);
			maze[(standardize((int)(mu.m01 / mu.m00)) - 50) / 100][(standardize((int)(mu.m10 / mu.m00)) - 50) / 100] = 1;
		}
	}
	maze[0][0] = 0;
	maze[5][5] = 0;
	maze[4][5] = 1;
	locationPoint startP(5, 5);
	locationPoint endP(0, 0);
	stack<locationPoint>  pointStack;
	vector<locationPoint> vecPath;
	mazePath(maze, 6, 6, startP, endP, pointStack, vecPath);
	if (vecPath.empty() == true) {
		cout << "no right path" << endl;
	}
	else {
		cout << "shortest path:";
		for (auto i = vecPath.rbegin(); i != vecPath.rend(); ++i) {
			circle(mask, Point(100 * (i->col) + 50, 100 * (i->row) + 50), 0, Scalar(255, 0, 0), 40);
			cout << '(' << i->row << ',' << i->col << ')' << ' ';
		}
		for (auto i = vecPath.rbegin(); i + 1 != vecPath.rend(); ++i)
		{
			if ((i + 1)->col - i->col == -1)
			{
				path_manip[pathnum] = '1';
				pathnum += 1;
				path_manip[pathnum] = ' ';
				pathnum += 1;
				path_manip[pathnum] = '0';
				pathnum += 1;
				path_manip[pathnum] = ' ';
				pathnum += 1;
			}
			if ((i + 1)->col - i->col == 1)
			{
				path_manip[pathnum] = '5';
				pathnum += 1;
				path_manip[pathnum] = ' ';
				pathnum += 1;
				path_manip[pathnum] = '0';
				pathnum += 1;
				path_manip[pathnum] = ' ';
				pathnum += 1;
			}
			if ((i + 1)->row - i->row == 1)
			{
				path_manip[pathnum] = '3';
				pathnum += 1;
				path_manip[pathnum] = ' ';
				pathnum += 1;
				path_manip[pathnum] = '0';
				pathnum += 1;
				path_manip[pathnum] = ' ';
				pathnum += 1;
			}
			if ((i + 1)->row - i->row == -1)
			{
				path_manip[pathnum] = '7';
				pathnum += 1;
				path_manip[pathnum] = ' ';
				pathnum += 1;
				path_manip[pathnum] = '0';
				pathnum += 1;
				path_manip[pathnum] = ' ';
				pathnum += 1;
			}
		}
	}
	cout << endl;
	string input;
	for (int i = 0; i < pathnum; i++)
	{
		if (i == 0) input = path_manip[i];
		else input += path_manip[i];
	}
	for (int i = 0; i < pathnum; i++)
	{
		send_buf[i] = path_manip[i];
		cout << send_buf[i];
	}
	ofstream outfile("C:/opencv/files/manip.txt");
	outfile << input << endl;
	return 0;
} 