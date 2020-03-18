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

void Print_circles(Mat& image)
{
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
}
void Print_circles_method(Mat& image)
{
	circle(image, Point(50, 50), 25, Scalar(127), -1);
	circle(image, Point(150, 50), 25, Scalar(255), -1);
	circle(image, Point(250, 50), 25, Scalar(0), -1);
	circle(image, Point(50, 150), 25, Scalar(255), -1);
	circle(image, Point(150, 150), 25, Scalar(0), -1);
	circle(image, Point(250, 150), 25, Scalar(127), -1);
}
void Print_round_method(Mat& image)
{
	circle(image, Point(50, 50), 25, Scalar(127), 3);
	circle(image, Point(150, 50), 25, Scalar(255), 3);
	circle(image, Point(250, 50), 25, Scalar(0), 3);
	circle(image, Point(50, 150), 25, Scalar(255), 3);
	circle(image, Point(150, 150), 25, Scalar(0), 3);
	circle(image, Point(250, 150), 25, Scalar(127), 3);
}

void Filter_1(Mat& image, Mat& image_1, Mat& image_2, Mat& image_3)
{
	// Преобразование ручками
	for (int i = 0; i < 199; i++)
	{
		for (int j = 0; j < 299; j++)
		{
			// [-255; 255] -> нормирование
			image_1.at<uint8_t>(i, j) = (uint8_t)(((double)(image.at<uint8_t>(i, j) - (double)image.at<uint8_t>(i + 1, j + 1))) / 2 + 127);
		}
	}

	for (int i = 0; i < 199; i++)
	{
		for (int j = 1; j < 299; j++)
		{
			// [-255; 255] -> нормирование
			image_2.at<uint8_t>(i, j) = (uint8_t)(((double)(image.at<uint8_t>(i, j + 1) - (double)image.at<uint8_t>(i + 1, j))) / 2 + 127);
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
}
void Filter_2(Mat& image, Mat& image_1, Mat& image_2, Mat& image_3)
{
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
}

void Filter_1_method(Mat& image, Mat& image_1, Mat& image_2, Mat& image_3)
{
	float kernelData_1[] = { 1.0f, 0.0f,
	0.0f, -1.0f };
	Mat kernel_1(2, 2, CV_32FC1, (float*)kernelData_1);

	float kernelData_2[] = { 0.0f, 1.0f,
	-1.0f, 0.0f };
	Mat kernel_2(2, 2, CV_32FC1, (float*)kernelData_2);

	filter2D(image, image_1, -1, kernel_1);
	filter2D(image, image_2, -1, kernel_2);

	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 300; j++)
		{
			// [0; sqrt(2) * 255] -> нормирование
			image_3.at<float>(i, j) = (sqrt(image_1.at<float>(i, j) * image_1.at<float>(i, j) + image_2.at<float>(i, j) * image_2.at<float>(i, j)) / sqrt(2));
		}
	}
}
void Filter_2_method(Mat& image, Mat& image_1, Mat& image_2, Mat& image_3)
{
	// Преобразование через метод filter2D
	float kernelData_1[] = { 1.0f, 2.0f, 1.0f,
	0.0f, 0.0f, 0.0f,
	-1.0f, -2.0f, -1.0f };
	Mat kernel_1(3, 3, CV_32FC1, (float*)kernelData_1);

	float kernelData_2[] = { 1.0f, 0.0f, -1.0f,
	2.0f, 0.0f, -2.0f,
	1.0f, 0.0f, -1.0f };
	Mat kernel_2(3, 3, CV_32FC1, (float*)kernelData_2);

	//filter2D(image, image_1, -1, kernel_1);
	filter2D(image, image_1, -1, kernel_1);
	filter2D(image, image_2, -1, kernel_2);

	//cout << image_1 << endl;
	cout << image_1.at<float>(25, 250) << endl;

	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 300; j++)
		{
			// [0; sqrt(2) * 255] -> нормирование
			image_1.at<float>(i, j) = image_1.at<float>(i, j) / 8.0f + 127.0f;
			image_2.at<float>(i, j) = image_2.at<float>(i, j) / 8.0f + 127.0f;
		}
	}

	//cout << image_1 << endl;

	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 300; j++)
		{
			// [0; sqrt(2) * 255] -> нормирование
			image_3.at<float>(i, j) = (sqrt(image_1.at<float>(i, j) * image_1.at<float>(i, j) + image_2.at<float>(i, j) * image_2.at<float>(i, j)) / sqrt(2));
		}
	}

	//cout << image_3 << endl;
}

void Filter2d_Method_from_internet()
{
	// константы для определения названия окон 
	const char* initialWinName = "Initial Image ",
		* resultWinName = "Filter2D ";
	// константы для хранения ядра фильтра 
	const float kernelData[] = { -0.1f, 0.2f, -0.1f,
		0.2f, 3.0f, 0.2f,
		-0.1f, 0.2f, -0.1f };
	const Mat kernel(3, 3, CV_32FC1, (float*)kernelData);
	// объекты для хранения исходного 
	// и результирующего изображений 
	Mat src, dst;
	// загрузка изображения 
	src = imread("Cutie_cat.jpg", 1);
	// применение фильтра 
	filter2D(src, dst, -1, kernel);

	// отображение исходного изображения и 
	// результата применения фильтра 
	namedWindow(initialWinName, WINDOW_AUTOSIZE);
	imshow(initialWinName, src);
	namedWindow(resultWinName, WINDOW_AUTOSIZE);
	imshow(resultWinName, dst);
	waitKey();

	// закрытие окон 
	destroyAllWindows();
	// освобождение ресурсов 
	src.release();
	dst.release();
}

int main()
{
	Mat image(200, 300, CV_8UC1, Scalar(0));

	int value_1 = 0;
	int value_2 = 127;
	int value_3 = 255;

	// Делаем фон
	// Тип uint8_t
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

	Print_circles_method(image);

	imshow("Ready_image", image);
	waitKey(0);

	Mat image_float;
	image.convertTo(image_float, CV_32FC1);

	Mat image_1_float(200, 300, CV_32FC1);
	Mat image_2_float(200, 300, CV_32FC1);
	Mat image_3_float(200, 300, CV_32FC1);

	Mat image_1 = image.clone();
	Mat image_2 = image.clone();
	Mat image_3 = image.clone();

	//Filter2d_Method_from_internet();

	//Filter_1_method(image_float, image_1_float, image_2_float, image_3_float);
	Filter_2_method(image_float, image_1_float, image_2_float, image_3_float);

	//Filter_1(image, image_1, image_2, image_3);
	//Filter_2(image, image_1, image_2, image_3);

	imshow("Image_1_float", image_1_float);
	imshow("Image_2_float", image_2_float);
	imshow("Image_3_float", image_3_float);
	waitKey(0);

	image_1_float.convertTo(image_1, CV_8UC1);
	image_2_float.convertTo(image_2, CV_8UC1);
	image_3_float.convertTo(image_3, CV_8UC1);

	imshow("My image after filter 1", image_1);
	imshow("My image after filter 2", image_2);
	imshow("My image after filter 3", image_3);
	waitKey(0);

	return 0;
}
