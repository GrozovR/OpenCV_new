#include <opencv2\opencv.hpp>
#include <vector>
#include <string>

using namespace cv;

class ImageFiltering
{

public:
	ImageFiltering() {};
	~ImageFiltering() {};
	void show();

	void readIMage(Mat img);
	void LaplacianFilter(int kSize)
	{
		Mat temp;
		temp = m_changedImage;
		int kSize_ = 2 * kSize + 1;

		Laplacian(temp, m_changedImage, CV_16S, kSize_);
	}
	void BilateralFilter(int sigma)
	{
		Mat temp;
		int diameter = 5;

		bilateralFilter(m_changedImage, temp, diameter, sigma, sigma);
		temp.copyTo(m_changedImage);
	}

	void PublicVectorMedianFilter(const int coreSize)
	{
		VectorMedianFilter(coreSize);
		//неправильно, потому что разбирается исходная картинка
		merge(vec_picture_channels, m_changedImage);
	}

	Mat GetChangedImage() const
	{
		return m_changedImage;
	}

private:
	static void on_trackbar_lapl(int kSize, void* param);
	static void on_trackbar_blur(int coreSize, void* param);
	static void on_trackbar_bilateral_sigma(int sigma, void * param);

	void VectorMedianFilter(const int coreSize);
	void getSupportRegion(int x, int y);

	void filterPixel(int x, int y);
	double aggregateDistance(int* pixel);
	double evclid_distance(int* pixel, int numberPixelInCore);

private:
	static String wndTitle;
	Mat img_src;
	Mat m_changedImage;

	int center{ 0 };
	int radius{ 0 };

	std::vector<Mat> in_picture_channels;
	std::vector<Mat> vec_picture_channels;
	std::vector<std::vector<int>> supportRegion;

};

void main(int argc, char* argv[])
{
	String image_path(1 < argc ? argv[1] : "../../../testdata/wSB0i6VBFvw.jpg");
	Mat img_src(imread(image_path));
	ImageFiltering app;
	
	app.readIMage(img_src);


	if (argc > 3) //in this case image save 
	{
		for (int i = 2; i < argc - 1; i += 2)
		{
			String filter{ argv[i] };
			int param{ 0 };

			std::stringstream CharToInt;
			CharToInt.str(argv[i + 1]);
			CharToInt >> param;

			if ("-l" == filter)
			{
				app.LaplacianFilter(param);
			}
			else if ("-v" == filter)
			{
				app.PublicVectorMedianFilter(param);
			}
			else if ("-b" == filter)
			{
				app.BilateralFilter(param);
			}
			else
			{
				std::cout << "error key!\n";
				break;
			}
		}

		//new image create near sourse image
		//new image has name filtered_[sourse image name] 
		std::string newFilePath{ image_path };
		size_t pos = newFilePath.rfind(92);
		++pos;
		
		std::string insertedStr = "filtered_";
		newFilePath.insert(pos, insertedStr);
		
		imwrite(newFilePath, app.GetChangedImage());
	}
	else app.show();


	waitKey();
}

String ImageFiltering::wndTitle("lab02");

void ImageFiltering::show()
{
	namedWindow(wndTitle, 1);
	createTrackbar("Laplacian(kSize):", wndTitle, 0, 10, on_trackbar_lapl, this);
	createTrackbar("Vectorfilter", wndTitle, 0, 1, on_trackbar_blur, this);
	createTrackbar("sigma(bil)", wndTitle, 0, 150, on_trackbar_bilateral_sigma, this);

	imshow(wndTitle, img_src);
}

void ImageFiltering::readIMage(Mat img)
{
	img.copyTo(img_src);
	img.copyTo(m_changedImage);
}

void ImageFiltering::on_trackbar_lapl(int kSize, void* param)
{
	ImageFiltering& app(*reinterpret_cast<ImageFiltering*>(param));
	Mat lap;
	int kSize_ = 2 * kSize + 1;
	Laplacian(app.img_src, lap, CV_16S, kSize_);
	imshow(wndTitle, lap);
}

void ImageFiltering::on_trackbar_blur(const int coreSize, void * param)
{
	ImageFiltering& app(*reinterpret_cast<ImageFiltering*>(param));
	Mat blur_;

	app.VectorMedianFilter(coreSize);
	merge(app.vec_picture_channels, blur_);
	imshow(wndTitle, blur_);
}

void ImageFiltering::on_trackbar_bilateral_sigma(int sigma, void * param)
{
	ImageFiltering& app(*reinterpret_cast<ImageFiltering*>(param));
	Mat bilateral;

	int diameter = 7;

	bilateralFilter(app.img_src, bilateral, diameter, sigma, sigma);
	imshow(wndTitle, bilateral);
}

void ImageFiltering::getSupportRegion(int x, int y)
{
	// fill 'supportRegion' for current mask position
	// pass through the pixels around (x, y) and initialize the vector array
	for (int i = 0; i < radius; i++)
	{
		int xi = x + i - center;
		for (int j = 0; j < radius; j++)
		{
			int yj = y + j - center;

			std::vector<int> temp;
			for (int ch = 0; ch < in_picture_channels.size(); ++ch)
			{
				temp.push_back(in_picture_channels[ch].at<unsigned char>(xi, yj));
			}
			supportRegion.push_back(temp);
		}
	}
}

double ImageFiltering::evclid_distance(int* pixel, int numberPixelInCore)
{
	double sum = 0.0;
	double distance{ 0 };

	for (int ch = 0; ch < in_picture_channels.size(); ch++)
	{
		distance = pixel[ch] - supportRegion[numberPixelInCore][ch];
		sum = sum + pow(distance, 2);
	}
	return sqrt(sum);
}

double ImageFiltering::aggregateDistance(int* pixel)
{
	double distance{ 0 };
	int numberPixelInCore{ 0 };

	for (int i = 0; i < pow(radius, 2); i++)
	{
		distance += evclid_distance(pixel, numberPixelInCore);
		numberPixelInCore++;
	}

	return distance;
}

void ImageFiltering::filterPixel(int x, int y)
{
	int* centralPixel = new int[in_picture_channels.size()];

	supportRegion.clear();
	getSupportRegion(x, y);

	double minDistance = DBL_MAX;
	int jMin = -1;


	for (int j = 0; j < pow(radius, 2); j++)
	{
		int* pixel = new int[in_picture_channels.size()];

		for (int i = 0; i < in_picture_channels.size(); i++)
		{
			pixel[i] = supportRegion[j][i];
		}

		double distance = aggregateDistance(pixel);

		if (distance < minDistance)
		{
			jMin = j;
			minDistance = distance;
		}
	}

	for (int i = 0; i < in_picture_channels.size(); i++)
	{
		vec_picture_channels[i].at<unsigned char>(x, y) = supportRegion[jMin][i];
	}
}

void ImageFiltering::VectorMedianFilter(const int coreSize)
{
	center = coreSize;
	radius = 2 * coreSize + 1;
	// при 
	in_picture_channels.resize(m_changedImage.channels());
	split(m_changedImage, in_picture_channels);
	split(m_changedImage, vec_picture_channels);

	for (double x = center; x < m_changedImage.rows - center; x++)
	{
		for (int y = center; y < m_changedImage.cols - center; y++)
		{
			filterPixel(x, y);
		}
		// to show percent of done 
		std::cout << round(100 * x / img_src.rows) << "%\n";
	}
	std::cout << "Vector Median Filtering done\n";
}
