//http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html#imread

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
  int retValue(0);
  if (2 != argc)
  {
  }
  else
  {
    const string imgIn(argv[1]);
    const string imgOut(argv[2]);
    Mat image(imread(imgIn, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR));
    if (0 == image.data)
    {
      cerr << "Failed read image " << imgIn;
      retValue = -1;
    }
    else
    {
      imwrite(imgOut, image);
    }
   }
   return retValue;
}