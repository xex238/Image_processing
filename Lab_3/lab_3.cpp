#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <iostream>
#include <ostream>

using namespace cv;
using namespace std;


int main()
{
	Mat mat = imread("Cutie_cat.jpg");

	vector<int> compression_params;
	compression_params.push_back(IMWRITE_JPEG_QUALITY);
	compression_params.push_back(10);

	try
	{
		imwrite("Cutie_cat_10.jpg", mat, compression_params);

		compression_params.pop_back();
		compression_params.push_back(60);
		imwrite("Cutie_cat_60.jpg", mat, compression_params);
	}
	catch(const cv::Exception& ex)
	{
		cout << "Exception converting image to PNG format : " << endl;
	}
}
