#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <iostream>
#include <ostream>

using namespace cv;
using namespace std;

Mat Step_average_binarization(Mat& bw_mat)
{
	for (int i = 0; i < bw_mat.cols; i++)
	{
		for (int j = 0; j < bw_mat.rows; j++)
		{
			if (bw_mat.at<uint8_t>(j, i) < 128)
			{
				bw_mat.at<uint8_t>(j, i) = 0;
			}
			else
			{
				bw_mat.at<uint8_t>(j, i) = 255;
			}
		}
	}

	return bw_mat;
}

Mat Step_median_binarization(Mat& bw_mat)
{
	int median = 0;

	for (int i = 0; i < bw_mat.rows; i++)
	{
		for (int j = 0; j < bw_mat.cols; j++)
		{
			median = median + bw_mat.at<uint8_t>(i, j);
		}
	}

	median = round(median / (bw_mat.rows * bw_mat.cols));
	cout << "median = " << median << endl;

	for (int i = 0; i < bw_mat.cols; i++)
	{
		for (int j = 0; j < bw_mat.rows; j++)
		{
			if (bw_mat.at<uint8_t>(j, i) < median)
			{
				bw_mat.at<uint8_t>(j, i) = 0;
			}
			else
			{
				bw_mat.at<uint8_t>(j, i) = 255;
			}
		}
	}

	return bw_mat;
}

Mat Global_binarization(Mat& bw_mat)
{
	//Mat result = Step_average_binarization(bw_mat);
	Mat result = Step_median_binarization(bw_mat);
	// Реализовать метод Отцу глобальной бинаризации
	return result;
}

Mat Local_binarization(Mat& bw_mat)
{
	// Реализовать метод Ниблека локальной бинаризации
}

int main()
{
	Mat mat = imread("Cutie_cat.jpg");

	Mat bw_mat;
	cvtColor(mat, bw_mat, COLOR_BGR2GRAY);
	Mat binary_mat = bw_mat.clone();

	Mat result = Global_binarization(binary_mat);

	imshow("Cutie cat", mat);
	waitKey(0);

	imshow("Black-white cutie cat", bw_mat);
	waitKey(0);

	imshow("Binarization cutie cat", result);
	waitKey(0);
}