#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
  int retValue(0);
  if (1 != argc)
  {
  }
  else
  {
    const string imgIn(argv[1]);
    Mat image(imread(imgIn, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR));
    if (!image.data)
    {
      cerr << "Failed read image " << imgIn;
      retValue = -1;
    }
    else
    {
      // image.cols;
      cout << image;
    }
   }
   return retValue;
}