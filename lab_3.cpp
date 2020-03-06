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


	//Mat hsv_mat;
	//cvtColor(src_mat, hsv_mat, CV_BGR2HSV);
	//MatND HSV_histogram;
	//int histSize[] = { 240, 240 };
	//float h_ranges[] = { 0, 255 };
	//float s_ranges[] = { 0, 180 };
	//const float* ranges[] = { h_ranges, s_ranges };
	//int channels[] = { 0, 1 };
	//calcHist(&hsv_mat, 1, channels, Mat(), HSV_histogram, 2, histSize, ranges, true, false);
	//normalize(HSV_histogram, HSV_histogram, 0, 1, NORM_MINMAX, -1, Mat());
}



//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <ostream>
//#include "plot.h"
//
//using namespace cv;
//
//int main(int argc, char** argv)
//{
//    // Plot data must be a 1xN or Nx1 matrix.
//    // Plot data type must be double (CV_64F)
//    Mat data(30, 1, CV_64F);
//    randu(data, 0, 500); // random values
//
//    Mat plot_result;
//
//    Ptr<plot::Plot2d> plot = plot::Plot2d::create(data);
//    plot->setPlotBackgroundColor(Scalar(50, 50, 50)); // i think it is not implemented yet
//    plot->setPlotLineColor(Scalar(50, 50, 255));
//    plot->render(plot_result);
//
//    imshow("plot", plot_result);
//    waitKey();
//
//    plot->setPlotLineColor(Scalar(50, 255, 255));
//    data = data / 3;
//    plot->render(plot_result);
//
//    imshow("plot", plot_result);
//    waitKey();
//    plot->setGridLinesNumber(4);
//    plot->setPlotGridColor(Scalar(255, 0, 255));
//    data = data * 4;
//    plot->render(plot_result);
//
//    imshow("plot", plot_result);
//    waitKey();
//
//    plot->setShowGrid(false);
//    plot->setShowText(false);
//    plot->setPlotTextColor(Scalar(255, 0, 0));
//    randu(data, 100, 400);
//    plot->render(plot_result);
//
//    imshow("plot", plot_result);
//    waitKey();
//
//    return 0;
//}