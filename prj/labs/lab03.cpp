#include <opencv2/opencv.hpp>

using namespace cv;

const String keys =
"{help h ?  | | print help	}"
"{a| | call a		}"
"{bab b| | call b		}";


class CApp
{
public:
	CApp() = default;
	~CApp() = default;


public:
	void GenerateImage();
	void Show();

public:
	void CalcDerX();
	void CalcDerY();
	void CalcGradient();
private:
	static String m_wndTitle;
	//!sourse image
	Mat m_imageSrc{ Mat::zeros(360,360,CV_8UC1) };
	//!deriviateX
	Mat m_deriviateImageX{ Mat::zeros(360,360,CV_8UC1) };
	//!deriviateY
	Mat m_deriviateImageY{ Mat::zeros(360,360,CV_8UC1) };
	Mat m_gradientImage{ Mat::zeros(360,360,CV_8UC1) };
};


void help()
{
	std::cout << keys << std::endl;
}

void a()
{
	std::cout << "A()" << std::endl;
}

void b()
{
	std::cout << "B()" << std::endl;
}

String CApp::m_wndTitle("lab03");


//function generate new image into m_imageSrc
void CApp::GenerateImage()
{

	int lineWidth{ m_imageSrc.rows / 3 };
	//first layer (3 horizontal lines 255,176,0)
	for (int x{ 0 }; x < m_imageSrc.rows; x++)
	{
		for (int y{ 0 }; y < m_imageSrc.cols; y++)
		{
			if (x < lineWidth) m_imageSrc.at<uchar>(x, y) = 255;
			else if (x >= lineWidth && x < 2 * lineWidth) m_imageSrc.at<uchar>(x, y) = 176;
			else m_imageSrc.at<uchar>(x, y) = 0;
		}
	}

	//second layer (2 horizontal line 255,0)
	lineWidth = m_imageSrc.rows / 4;
	int lineLength{ m_imageSrc.cols / 3 };

	for (int x{ lineWidth }; x < (3 * lineWidth); x++)
	{
		for (int y{ lineLength }; y < 2 * lineLength; y++)
		{
			if (x < 2 * lineWidth) m_imageSrc.at<uchar>(x, y) = 0;
			else m_imageSrc.at<uchar>(x, y) = 255;
		}
	}
}

void CApp::Show()
{
	imshow(m_wndTitle, m_imageSrc);
}

void CApp::CalcDerX()
{
	std::vector<std::vector<int>> operatorX = { {1,0,-1},{ 1,0,-1 },{ 1,0,-1 } };


	for (int x{ 1 }; x < m_imageSrc.rows - 1; x++)
	{
		for (int y{ 1 }; y < m_imageSrc.cols - 1; y++)
		{
			double summ{ 0 };

			for (int i{ 0 }; i < operatorX.size(); i++)
			{
				for (int j{ 0 }; j < operatorX[i].size(); j++)
				{
					summ += operatorX[i][j] * m_imageSrc.at<uchar>(x - 1 + i, y - 1 + j);
				}
			}

			m_deriviateImageX.at<uchar>(x, y) = sqrt(summ*summ) / 3;

		}
	}
	imshow("derX", m_deriviateImageX);
}

void CApp::CalcDerY()
{
	std::vector<std::vector<int>> operatorX = { { 1,1,1 },{ 0,0,0 },{ -1,-1,-1 } };


	for (int x{ 1 }; x < m_imageSrc.rows - 1; x++)
	{
		for (int y{ 1 }; y < m_imageSrc.cols - 1; y++)
		{
			double summ{ 0 };

			for (int i{ 0 }; i < operatorX.size(); i++)
			{
				for (int j{ 0 }; j < operatorX[i].size(); j++)
				{
					summ += operatorX[i][j] * m_imageSrc.at<uchar>(x - 1 + i, y - 1 + j);
				}
			}

			m_deriviateImageY.at<uchar>(x, y) = sqrt(summ*summ) / 3;

		}
	}
	imshow("derY", m_deriviateImageY);
}

void CApp::CalcGradient()
{
	for (int x = 0; x < m_imageSrc.rows; x++)
	{
		for (int y = 0; y < m_imageSrc.cols; y++)
		{
			int derX = m_deriviateImageX.at<uchar>(x, y);
			int derY = m_deriviateImageY.at<uchar>(x, y);

			m_gradientImage.at<uchar>(x, y) = sqrt(derX*derX + derY*derY);
		}
	}
	imshow("gradient", m_gradientImage);
}


int main(int argc, char* argv[])
{
	CommandLineParser parser(argc, argv, keys);
	if (parser.has("help"))
	{
		help();
	}
	if (parser.has("a"))
	{
		a();
	}
	if (parser.has("bab"))
	{
		b();
	}

	CApp app;
	app.GenerateImage();
	app.CalcDerX();
	app.CalcDerY();
	app.CalcGradient();
	app.Show();


	waitKey();
	return 0;
}

