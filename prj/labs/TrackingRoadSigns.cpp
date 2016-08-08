#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <vector>
#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;



struct params
{
	params() = default;

	Mat backproj;
	Mat hist;
	Rect trackWindow;
	RotatedRect trackBox;

	TermCriteria tc{ TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1) };
};


Mat image;
//vector<RotatedRect> trackBox;
vector<params> paramsOfTrackObj;

bool backprojMode = false;
bool selectObject = false;
int trackObject = 0;
Point origin;
Rect selection;
int vmin = 10, vmax = 256, smin = 30;

static void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);

		selection &= Rect(0, 0, image.cols, image.rows);
	}

	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		selectObject = true;
		break;
	case EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
		{
			trackObject = -1; //then params get, but don't processed
			params newTrackObj;
			newTrackObj.trackWindow = selection;
			paramsOfTrackObj.push_back(newTrackObj);
		}

		break;
	}

}

int main(int argc, const char** argv)
{
	VideoCapture cap;

	int hsize = 16;
	float hranges[] = { 0,180 };
	const float* phranges = hranges;
	

	String path = "D:\\grozov\\revers.avi";
	cap.open(path);

	//cap.open(0);

	namedWindow("CamShift", 0);
	setMouseCallback("CamShift", onMouse, 0);

	bool paused = false;
	Mat frame, hsv, hue, mask;
	int savingFrameName{ 0 };
	std::string frameName;

	for (;;)
	{
		if (!paused)
		{
			cap >> frame;
			if (frame.empty())
				break;
		}
		frame.copyTo(image);


		if (!paused)
		{
			cvtColor(image, hsv, COLOR_BGR2HSV);

			if (trackObject)
			{

				int _vmin = vmin, _vmax = vmax;

				inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
					Scalar(180, 256, MAX(_vmin, _vmax)), mask);
				int ch[] = { 0, 0 };
				hue.create(hsv.size(), hsv.depth());
				mixChannels(&hsv, 1, &hue, 1, ch, 1);

				//trackBox.resize(paramsOfTrackObj.size());

				if (trackObject < 0) // -1 означает, что объекты выбраны, но не обработаны
				{
					for (int i = 0; i < paramsOfTrackObj.size(); i++)
					{
						Mat roi(hue, paramsOfTrackObj[i].trackWindow), maskroi(mask, paramsOfTrackObj[i].trackWindow);
						calcHist(&roi, 1, 0, maskroi, paramsOfTrackObj[i].hist, 1, &hsize, &phranges);
						normalize(paramsOfTrackObj[i].hist, paramsOfTrackObj[i].hist, 0, 255, NORM_MINMAX);

					}
					trackObject = paramsOfTrackObj.size();
				}

				for (int i{ 0 }; i < paramsOfTrackObj.size(); i++)
				{

					calcBackProject(&hue, 1, 0, paramsOfTrackObj[i].hist, paramsOfTrackObj[i].backproj, &phranges);
					paramsOfTrackObj[i].backproj &= mask;

					double previousArea = paramsOfTrackObj[i].trackBox.size.area();

					paramsOfTrackObj[i].trackBox = CamShift(paramsOfTrackObj[i].backproj
						, paramsOfTrackObj[i].trackWindow
						, paramsOfTrackObj[i].tc);

					// проверка на потерю отслеживаемой области( площадь начинает увеличивается)
					if ((paramsOfTrackObj[i].trackBox.size.area() > 1.2 * previousArea) && (previousArea != 0))
					{
						paramsOfTrackObj.erase(paramsOfTrackObj.begin() + i);
						i = 0;
						continue;
					}

					//std::cout << trackBox[i].size.area() << std::endl;
					// проверка отслеживаемой области на удаленность (по площади)
					if (paramsOfTrackObj[i].trackBox.size.area() < 400)
					{
						paramsOfTrackObj.erase(paramsOfTrackObj.begin() + i);
						i = 0;
						continue;
					}

					if (paramsOfTrackObj[i].trackWindow.area() <= 1)
					{
						int cols = paramsOfTrackObj[i].backproj.cols;
						int rows = paramsOfTrackObj[i].backproj.rows;
						int r = (MIN(cols, rows) + 5) / 6;

						paramsOfTrackObj[i].trackWindow = Rect(
							paramsOfTrackObj[i].trackWindow.x - r,
							paramsOfTrackObj[i].trackWindow.y - r,
							paramsOfTrackObj[i].trackWindow.x + r,
							paramsOfTrackObj[i].trackWindow.y + r) &
							Rect(0, 0, cols, rows);
					}

					if (backprojMode)
						cvtColor(paramsOfTrackObj[i].backproj, image, COLOR_GRAY2BGR);
					ellipse(image, paramsOfTrackObj[i].trackBox, Scalar(0, 0, 255), 3, LINE_AA);
					//rectangle(image, trackBox[i], Scalar(0, 0, 255), 3, LINE_AA);
					//rectangle(image, &trackBox[i].boundingRect, Scalar(0, 0, 255), 3, LINE_AA);
				}

			}
			//frameName = "D:\\grozov\\saveFrames\\" + std::to_string(savingFrameName) + ".png";
			//imwrite(frameName, image);
			//savingFrameName++;
		}


		if (selectObject && selection.width > 0 && selection.height > 0)
		{
			Mat roi(image, selection);
			bitwise_not(roi, roi);
		}

		imshow("CamShift", image);
					

		char c = (char)waitKey(10);
		if (c == 27)
			break;
		switch (c)
		{
		case 'c':
			trackObject = 0;
			paramsOfTrackObj.clear();
			break;
		case 'p':
			paused = !paused;
			break;
		case 's':
			//frameName = "D:\\grozov\\saveFrames\\" + std::to_string(savingFrameName) + ".png";
			//imwrite(frameName, image);
			//savingFrameName++;
			break;
		default:
			;
		}
	}

	return 0;
}