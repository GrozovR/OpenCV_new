//http://docs.opencv.org/modules/core/doc/drawing_functions.html#void line(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness, int lineType, int shift)

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace cv;

int main()
{
   Mat image(400, 300, CV_8U, Scalar(255));
   circle(image, Point(20, 20), 30, Scalar(100));
   circle(image, Point(60, 20), 30, Scalar(100), 1, 4);
   circle(image, Point(40, 40), 15, Scalar(50), 4, 8);
   circle(image, Point(60, 60), 24, Scalar(0), 1, 4);
   imshow("cv::circle", image);
   waitKey(0);
    
   return 0;
}