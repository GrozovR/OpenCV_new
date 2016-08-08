#include <opencv2//opencv.hpp>

#include <vector>
#include <iostream>
#include <string>

using namespace cv;

class ChanelBinarizationApp
{
public:
	ChanelBinarizationApp() {};
	~ChanelBinarizationApp() {};
	void show(Mat img);
	void save_result(Mat img, int iChanel, int iTreshold, String outFile);
private:
	ChanelBinarizationApp(const ChanelBinarizationApp&);
	//ChanelBinarizationApp& operator=(const ChanelBinarizationApp&);
private:
	static void on_trackbar_ch(int iChanel, void* param);
	static void on_trackbar_threshold(int iTreshold, void* param);	
private:
	static String wndTitle;
	Mat img_src;
	Mat src_chanel;
	Mat dst;
	int number_of_chanel = 0;
	std::vector<Mat> chanels;
};

void main(int argc, char* argv[])
{
	String image_path(1 < argc ? argv[1] : "../../../testdata/1.jpg");
	

	Mat img_src(imread(image_path));
	ChanelBinarizationApp app;
	
	if (argc == 5)
	{
		//if (test) check valid of params

		app.save_result(img_src, atoi(argv[2]), atoi(argv[3]), argv[4]);
	}
	else
	{
		app.show(img_src);
		waitKey(0);
	}
}

String ChanelBinarizationApp::wndTitle("lab01");

void ChanelBinarizationApp::show(Mat img)
{
	img.copyTo(img_src);
	chanels.resize(img_src.channels());

	namedWindow(wndTitle, 1);

	createTrackbar("Chanel", wndTitle, 0, static_cast<int>(chanels.size()), on_trackbar_ch, this);
	createTrackbar("Treshold", wndTitle, 0, 255, on_trackbar_threshold, this);
	
	imshow(wndTitle, img_src);
}

void  ChanelBinarizationApp::on_trackbar_threshold(int iTreshold, void* param)
{
	ChanelBinarizationApp& app(*reinterpret_cast<ChanelBinarizationApp*>(param));
	if (app.number_of_chanel) //if != 0
	{
		threshold(app.src_chanel, app.chanels.at(app.number_of_chanel - 1), iTreshold, 255, THRESH_BINARY);
		merge(app.chanels, app.dst);
		imshow(app.wndTitle, app.dst);
	}
}

void ChanelBinarizationApp::save_result(Mat img, int iChanel, int iTreshold, String filename)
{
	img.copyTo(img_src);
	chanels.resize(img_src.channels());

	split(img_src, chanels);

	chanels[iChanel].copyTo(src_chanel);
	threshold(src_chanel, chanels.at(iChanel), iTreshold, 255, THRESH_BINARY);
	merge(chanels, dst);
	imwrite(filename, dst);
}

void  ChanelBinarizationApp::on_trackbar_ch(int iChanel, void* param)
{
	ChanelBinarizationApp& app(*reinterpret_cast<ChanelBinarizationApp*>(param));
	split(app.img_src, app.chanels);
	app.number_of_chanel = iChanel;

	if (0 == iChanel)
	{
		imshow(wndTitle, app.img_src);
	}
	else
	{
		app.chanels[iChanel - 1].copyTo(app.src_chanel);
		app.on_trackbar_threshold(getTrackbarPos("Treshold", wndTitle), &app);
	}
}

