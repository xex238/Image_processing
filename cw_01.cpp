#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <conio.h> // Нужен для _kbhit() и _getch()
#include <Windows.h> // Нужен для Sleep()

#include <iostream>
#include <ostream>
#include <vector>

using namespace cv;
using namespace std;

int main()
{
	Mat image(200, 300, CV_8UC1, Scalar(0));

	int value_1 = 0;
	int value_2 = 127;
	int value_3 = 255;

	// Делаем фон
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			image.at<uint8_t>(i, j) = (uint8_t)0;
		}
	}
	for (int i = 0; i < 100; i++)
	{
		for (int j = 100; j < 200; j++)
		{
			image.at<uint8_t>(i, j) = (uint8_t)127;
		}
	}
	for (int i = 0; i < 100; i++)
	{
		for (int j = 200; j < 300; j++)
		{
			image.at<uint8_t>(i, j) = (uint8_t)255;
		}
	}

	for (int i = 100; i < 200; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			image.at<uint8_t>(i, j) = (uint8_t)127;
		}
	}
	for (int i = 100; i < 200; i++)
	{
		for (int j = 100; j < 200; j++)
		{
			image.at<uint8_t>(i, j) = (uint8_t)255;
		}
	}
	for (int i = 100; i < 200; i++)
	{
		for (int j = 200; j < 300; j++)
		{
			image.at<uint8_t>(i, j) = (uint8_t)0;
		}
	}

	int r = 25; // Радиус кружочка

	// Рисуем кружочки
	// [1; 1]
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			if ((double)sqrt((i - 50) * (i - 50) + (j - 50) * (j - 50)) < 25)
			{
				image.at<uint8_t>(i, j) = (uint8_t)127;
			}
		}
	}
	// [1; 2]
	for (int i = 0; i < 100; i++)
	{
		for (int j = 100; j < 200; j++)
		{
			if ((double)sqrt((i - 50) * (i - 50) + (j - 150) * (j - 150)) < 25)
			{
				image.at<uint8_t>(i, j) = (uint8_t)255;
			}
		}
	}
	// [1; 3]
	for (int i = 0; i < 100; i++)
	{
		for (int j = 200; j < 300; j++)
		{
			if ((double)sqrt((i - 50) * (i - 50) + (j - 250) * (j - 250)) < 25)
			{
				image.at<uint8_t>(i, j) = (uint8_t)0;
			}
		}
	}
	// [2; 1]
	for (int i = 100; i < 200; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			if ((double)sqrt((i - 150) * (i - 150) + (j - 50) * (j - 50)) < 25)
			{
				image.at<uint8_t>(i, j) = (uint8_t)255;
			}
		}
	}
	// [2; 2]
	for (int i = 100; i < 200; i++)
	{
		for (int j = 100; j < 200; j++)
		{
			if ((double)sqrt((i - 150) * (i - 150) + (j - 150) * (j - 150)) < 25)
			{
				image.at<uint8_t>(i, j) = (uint8_t)0;
			}
		}
	}
	// [2; 3]
	for (int i = 100; i < 200; i++)
	{
		for (int j = 200; j < 300; j++)
		{
			if ((double)sqrt((i - 150) * (i - 150) + (j - 250) * (j - 250)) < 25)
			{
				image.at<uint8_t>(i, j) = (uint8_t)127;
			}
		}
	}

	Mat image_1 = image.clone();
	Mat image_2 = image.clone();
	Mat image_3 = image.clone();

	for (int i = 1; i < 199; i++)
	{
		for (int j = 1; j < 299; j++)
		{
			// [-1024; 1024] -> нормирование
			image_1.at<uint8_t>(i, j) = (uint8_t)(((double)(image.at<uint8_t>(i - 1, j - 1) + 2 * (double)image.at<uint8_t>(i, j - 1) + (double)image.at<uint8_t>(i + 1, j - 1) - (double)image.at<uint8_t>(i - 1, j + 1) - 2 * (double)image.at<uint8_t>(i, j + 1) - (double)image.at<uint8_t>(i + 1, j + 1))) / 8 + 127);
		}
	}

	for (int i = 1; i < 199; i++)
	{
		for (int j = 1; j < 299; j++)
		{
			// [-1024; 1024] -> нормирование
			image_2.at<uint8_t>(i, j) = (uint8_t)(((double)(image.at<uint8_t>(i - 1, j - 1) + 2 * (double)image.at<uint8_t>(i - 1, j) + (double)image.at<uint8_t>(i - 1, j + 1) - (double)image.at<uint8_t>(i + 1, j - 1) - 2 * (double)image.at<uint8_t>(i + 1, j) - (double)image.at<uint8_t>(i + 1, j + 1))) / 8 + 127);
		}
	}

	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 300; j++)
		{
			// [0; sqrt(2) * 255] -> нормирование
			image_3.at<uint8_t>(i, j) = (uint8_t)(sqrt((double)image_1.at<uint8_t>(i, j) * (double)image_1.at<uint8_t>(i, j) + (double)image_2.at<uint8_t>(i, j) * (double)image_2.at<uint8_t>(i, j)) / sqrt(2));
		}
	}

	imshow("My image after filter 1", image_1);
	waitKey(0);

	imshow("My image after filter 2", image_2);
	waitKey(0);

	imshow("My image after filter 2", image_3);
	waitKey(0);

	return 0;
}