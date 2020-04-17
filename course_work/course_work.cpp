#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <windows.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>
#include <ctime>

#include "document.h";
#include "filereadstream.h";

using namespace cv;
using namespace std;
using namespace rapidjson;

vector<string> Get_paths(const LPSTR& cPath, const string& extention)
{
	WIN32_FIND_DATA fd;
	HANDLE hFile;
	short len = lstrlen(cPath);
	lstrcat(cPath, "*");
	hFile = FindFirstFile(cPath, &fd);
	cPath[len] = 0;

	vector<string> paths;

	do {
		if (fd.cFileName[0] == '.')continue;
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			lstrcat(cPath, fd.cFileName);
			lstrcat(cPath, "\\");
			Get_paths(cPath, extention);
			cPath[len] = 0;
		}
		else
		{
			char* tmp = strrchr(fd.cFileName, '.');
			if (tmp && !strcmp(tmp, extention.c_str()))
			{
				paths.push_back(fd.cFileName);
			}
		}
	} while (FindNextFile(hFile, &fd));
	FindClose(hFile);

	cout << "Searched files" << endl;
	for (int i = 0; i < paths.size(); i++)
	{
		cout << paths[i] << endl;
	}
	cout << endl;

	return paths;
}

double Get_average(const Mat& mat)
{
	// Ищем среднее по всем каналам
	double average = 0.0;

	for (int i = 0; i < mat.cols; i++)
	{
		for (int j = 0; j < mat.rows; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				average += (double)mat.at<Vec3b>(j, i)[k];
			}
		}
	}

	average = average / (3 * mat.cols * mat.rows);

	//cout << "average = " << average << endl;
	//cout << endl;

	return average;
}
double Get_standart_deviation(const Mat& mat, const double& average)
{
	// Ищем стандартное отклонение
	double sigma = 0.0;

	for (int i = 0; i < mat.cols; i++)
	{
		for (int j = 0; j < mat.rows; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				sigma += ((double)mat.at<Vec3b>(j, i)[k] - average) * ((double)mat.at<Vec3b>(j, i)[k] - average);
			}
		}
	}

	sigma = sigma / (3 * mat.cols * mat.rows);
	sigma = sqrt(sigma);

	//cout << "sigma = " << sigma << endl;
	//cout << endl;

	return sigma;
}
double Get_covariance(const Mat& mat_1, const Mat& mat_2, const double& average_1, const double& average_2)
{
	if ((mat_1.cols != mat_2.cols) || (mat_1.rows != mat_2.rows))
	{
		cout << "Size of images is different" << endl;
		exit(-1);
	}

	double covariance = 0.0;

	for (int i = 0; i < mat_1.cols; i++)
	{
		for (int j = 0; j < mat_1.rows; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				covariance += ((double)mat_1.at<Vec3b>(j, i)[k] - average_1) * ((double)mat_2.at<Vec3b>(j, i)[k] - average_2);
			}
		}
	}

	covariance = covariance / (3 * mat_1.cols * mat_1.rows);

	//cout << "covariance = " << covariance << endl;
	//cout << endl;

	return covariance;
}

double SSIM(const Mat& mat_1, const Mat& mat_2)
{
	double start_time = clock();

	double average_1 = Get_average(mat_1);
	double average_2 = Get_average(mat_2);

	double sigma_1 = Get_standart_deviation(mat_1, average_1);
	double sigma_2 = Get_standart_deviation(mat_2, average_2);

	double covariance = Get_covariance(mat_1, mat_2, average_1, average_2);

	double K_1 = 0.01;
	double K_2 = 0.03;

	double L = 255.0;

	double C_1 = (K_1 * L) * (K_1 * L);
	double C_2 = (K_2 * L) * (K_2 * L);
	double C_3 = C_2 / 2;

	//cout << "C_1 = " << C_1 << endl;
	//cout << "C_2 = " << C_2 << endl;
	//cout << endl;

	double result_SSIM = ((2 * average_1 * average_2 + C_1) * (2 * covariance + C_2)) / ((average_1 * average_1 + average_2 * average_2 + C_1) * (sigma_1 * sigma_1 + sigma_2 * sigma_2 + C_2));

	double end_time = clock();

	cout << "time of work = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;
	cout << "SSIM = " << result_SSIM << endl;
	cout << endl;

	return result_SSIM;
}
void Window_SSIM(const Mat& mat_1, const Mat& mat_2)
{
	double start_time = clock();
	if ((mat_1.cols != mat_2.cols) || (mat_1.rows != mat_2.rows))
	{
		cout << "Size of images is different" << endl;
		exit(-1);
	}

	int size_window_x = 11;
	int size_window_y = 11;

	double result_SSIM = 0.0;

	for (int i = 0; i < mat_1.cols - size_window_y; i++)
	{
		for (int j = 0; j < mat_1.rows - size_window_x; j++)
		{
			//cout << "i = " << i << ", j = " << j << endl;

			Mat ROI_1(mat_1, Rect(i, j, size_window_y, size_window_x));
			Mat croppedImage_1;
			ROI_1.copyTo(croppedImage_1);

			Mat ROI_2(mat_2, Rect(i, j, size_window_y, size_window_x));
			Mat croppedImage_2;
			ROI_2.copyTo(croppedImage_2);

			//imshow("image_1", croppedImage_1);
			//imshow("image_2", croppedImage_2);
			//waitKey(0);

			result_SSIM += SSIM(croppedImage_1, croppedImage_2);
		}
	}

	result_SSIM = result_SSIM / ((mat_1.cols - size_window_y) * (mat_1.rows - size_window_x));

	double end_time = clock();

	cout << "result SSIM = " << result_SSIM << endl;
	cout << "time of work = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;
	cout << endl;
}

Mat Image_cropping(const Mat& mat)
{
	string path = "01_alb_id\\01_alb_id\\ground_truth\\TS\\TS01_01.json";
	//ifstream my_stream(path);
	
	FILE* pFile = fopen(path.c_str(), "rb");
	char buffer[65536];
	FileReadStream is(pFile, buffer, sizeof(buffer));

	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>(is);

	assert(document.IsObject());
	//assert(document.HasMember("hello")); // Нет такого поля - ошибка
	assert(document.HasMember("quad")); // Есть такое поле - ошибки нет
	assert(document["quad"].IsArray()); // Верно
	assert(document["quad"][0].IsArray()); // Верно
	//assert(document["quad"][0][0].IsDouble()); // Неверно
	assert(document["quad"][0][0].IsInt()); // Верно

	int value = document["quad"][0][0].GetInt();
	cout << "value = " << value << endl;

	int start_x = document["quad"][0][0].GetInt();
	int start_y = document["quad"][0][1].GetInt();
	int width = document["quad"][2][0].GetInt() - document["quad"][0][0].GetInt();
	int height = document["quad"][2][1].GetInt() - document["quad"][0][1].GetInt();

	Mat ROI(mat, Rect(start_x, start_y, width, height));
	Mat result_image;
	ROI.copyTo(result_image);

	imshow("Cropped image", result_image);
	waitKey(0);

	return result_image;
}

void TID2013()
{
	char cPath[MAX_PATH] = "tid2013\\distorted_images\\i01\\"; // Путь к папке с искажёнными изображениями

	string image_extention = ".bmp";
	vector<string> paths = Get_paths(cPath, image_extention); // Хранятся пути к искажённым изображениям

	Mat distorted_image; // Искажённое изображение
	Mat reference_image = imread("tid2013\\reference_images\\I01.bmp"); // Эталонное изображение

	for (int i = 0; i < paths.size(); i++)
	{
		paths[i] = cPath + paths[i];
	}

	double* SSIM_results = new double[paths.size()]; // Результаты сравнения изображений

	double start_time = clock(); // Засекаем время

	for (int i = 0; i < paths.size(); i++)
	{
		distorted_image = imread(paths[i]);
		SSIM_results[i] = SSIM(reference_image, distorted_image);
	}

	double end_time = clock(); // Получаем итоговое время

	cout << "total running time of the algorithm = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;

	// Записываем результаты в файл
	ofstream file;
	file.open("SSIM_results.txt");
	if (file.is_open())
	{
		for (int i = 0; i < paths.size(); i++)
		{
			file << SSIM_results[i] << endl;
		}
	}
	file.close();
}
void MIDV500()
{
	char cPath[MAX_PATH] = "01_alb_id\\01_alb_id\\images\\TS\\"; // Путь к папке с искажёнными изображениями

	string image_extention = ".tif";
	vector<string> paths = Get_paths(cPath, image_extention); // Хранятся пути к искажённым изображениям

	Mat distorted_image; // Искажённое изображение
	Mat reference_image = imread("01_alb_id\\01_alb_id\\images\\01_alb_id.tif"); // Эталонное изображение

	for (int i = 0; i < paths.size(); i++)
	{
		paths[i] = cPath + paths[i];
	}

	double* SSIM_results = new double[paths.size()]; // Результаты сравнения изображений

	double start_time = clock(); // Засекаем время

	// 1) Обрезаем изображение
	// 2) Приводим к одному размеру
	// 3) Находим значение SSIM
	for (int i = 0; i < paths.size(); i++)
	{
		distorted_image = imread(paths[i]);
		Mat cropped_image = Image_cropping(distorted_image); // Обрезаем изображение
		SSIM_results[i] = SSIM(reference_image, cropped_image); // Находим значение SSIM
	}

	double end_time = clock(); // Получаем итоговое время

	cout << "total running time of the algorithm = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;

	// Записываем результаты в файл
	ofstream file;
	file.open("SSIM_results.txt");
	if (file.is_open())
	{
		for (int i = 0; i < paths.size(); i++)
		{
			file << SSIM_results[i] << endl;
		}
	}
	file.close();
}

int main(int argc, char** argv)
{
	//TID2013();
	MIDV500();
}
