#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <conio.h> // Нужен для _kbhit() и _getch()
#include <Windows.h> // Нужен для Sleep()

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
Mat Step_Otsu_binarization(Mat& bw_mat)
{
	double* channel = new double[256];

	for (int i = 0; i < 256; i++)
	{
		channel[i] = 0;
	}

	for (int i = 0; i < bw_mat.cols; i++)
	{
		for (int j = 0; j < bw_mat.rows; j++)
		{
			channel[bw_mat.at<uint8_t>(j, i)]++;
		}
	}

	//cout << "channel[i] = " << endl;
	//for (int i = 0; i < 256; i++)
	//{
	//	cout << channel[i] << endl;
	//}

	int N = bw_mat.cols * bw_mat.rows;

	double* p = new double[256];

	//cout << "p[i] = " << endl;
	for (int i = 0; i < 256; i++)
	{
		p[i] = (double)channel[i] / (double)N;
		//cout << p[i] << endl;
	}

	// Вычисляем дисперсию всего изображения
	double M_x = 0;
	double M_x2 = 0;

	for (int i = 0; i < 256; i++)
	{
		M_x = M_x + channel[i] * p[i];
		M_x2 = M_x2 + channel[i] * channel[i] * p[i];
	}

	double sigma_general = M_x2 - (M_x * M_x);

	//cout << "M_x = " << M_x << endl;
	//cout << "M_x2 = " << M_x2 << endl;
	//cout << "sigma_general = " << sigma_general << endl << endl;

	// Ищем максимальное значение nu
	int L = 256;
	double nu = 0;
	int max_k = 0;

	for (int k = 1; k < L - 1; k++)
	{
		//cout << "k = " << k << endl << endl;

		double w0 = 0;

		for (int i = 0; i < k; i++)
		{
			w0 = w0 + p[i];
		}

		double w1 = 1 - w0;

		//cout << "w0 = " << w0 << endl;
		//cout << "w1 = " << w1 << endl;

		double mu0 = 0;

		for (int i = 0; i < k; i++)
		{
			mu0 = mu0 + (i * p[i] / w0);
		}

		double mu1 = 0;

		for (int i = k; i < L; i++)
		{
			mu1 = mu1 + (i * p[i] / w1);
		}

		//cout << "mu0 = " << mu0 << endl;
		//cout << "mu1 = " << mu1 << endl;

		double sigma_class = w0 * w1 * (mu1 - mu0) * (mu1 - mu0);

		//cout << "sigma_class = " << sigma_class << endl;
		//cout << "sigma_general = " << sigma_general << endl;
		//cout << "nu = " << sigma_class / sigma_general << endl << endl;

		if (sigma_class / sigma_general > nu)
		{
			nu = sigma_class / sigma_general;
			max_k = k;
		}
	}

	cout << "nu = " << nu << endl;
	cout << "max_k = " << max_k << endl << endl;

	for (int i = 0; i < bw_mat.cols; i++)
	{
		for (int j = 0; j < bw_mat.rows; j++)
		{
			if (bw_mat.at<uint8_t>(j, i) < max_k)
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
	Mat bw_mat_1 = bw_mat.clone();
	bw_mat_1 = Step_median_binarization(bw_mat_1);

	// Реализовать метод Отцу глобальной бинаризации
	Mat bw_mat_2 = bw_mat.clone();
	bw_mat_2 = Step_Otsu_binarization(bw_mat_2);
	return bw_mat_2;
}

void Local_binarization(Mat& bw_mat)
{
	// Реализовать метод Ниблека локальной бинаризации
}

int main()
{
	Mat mat = imread("Cutie_cat.jpg");

	Mat bw_mat;
	cvtColor(mat, bw_mat, COLOR_BGR2GRAY);
	Mat binary_mat = bw_mat.clone();

	imshow("Black-white cat", binary_mat);
	waitKey(0);

	Mat result = Global_binarization(binary_mat);

	imshow("Cutie cat", mat);
	waitKey(0);

	imshow("Black-white cutie cat", bw_mat);
	waitKey(0);

	imshow("Binarization cutie cat", result);
	waitKey(0);
}
