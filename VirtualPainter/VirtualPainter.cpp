#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//// Virtual Painter ////

Mat img;
vector<vector<int>> newPoints;

// hmin, smin, vmin, hmax, smax, vmax for Green
vector<int> myGreenColor{ 33, 42, 21, 99, 255, 255 }; // Green

// Color value for Green
Scalar greenColorValue(0, 102, 51); // Green

Point getContours(Mat imgDil)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    Point myPoint(0, 0);

    for (int i = 0; i < contours.size(); i++)
    {
        int area = contourArea(contours[i]);
        cout << area << endl;

        if (area > 1000)
        {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            boundRect[i] = boundingRect(conPoly[i]);
            myPoint.x = boundRect[i].x + boundRect[i].width / 2; // Middle of the rectangle
            myPoint.y = boundRect[i].y; // Top of the rectangle
        }
    }
    return myPoint;
}

vector<vector<int>> findGreen(Mat img)
{
    Mat imgHSV;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);

    Scalar lower(myGreenColor[0], myGreenColor[1], myGreenColor[2]);
    Scalar upper(myGreenColor[3], myGreenColor[4], myGreenColor[5]);
    Mat mask;
    inRange(imgHSV, lower, upper, mask);
    Point myPoints = getContours(mask);

    if (myPoints.x != 0 && myPoints.y != 0)
    {
        newPoints.push_back({ myPoints.x, myPoints.y });
    }
    return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, Scalar greenColorValue)
{
    for (int i = 0; i < newPoints.size(); i++)
    {
        circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, greenColorValue, FILLED);
    }
}

void main()
{
    VideoCapture cap(0);

    while (true)
    {
        cap.read(img);
        newPoints = findGreen(img);
        drawOnCanvas(newPoints, greenColorValue);

        imshow("Image", img);
        waitKey(1);
    }
}
