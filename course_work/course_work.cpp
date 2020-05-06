#include <opencv2/opencv.hpp>

#include <windows.h>
#include <fstream>
#include <ctime>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace cv;
using namespace std;
using namespace rapidjson;

// Получения набора путей к оцениваемым изображениям
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

			Mat ROI_1(mat_1, Rect(i, j, size_window_y, size_window_x));
			Mat croppedImage_1;
			ROI_1.copyTo(croppedImage_1);

			Mat ROI_2(mat_2, Rect(i, j, size_window_y, size_window_x));
			Mat croppedImage_2;
			ROI_2.copyTo(croppedImage_2);

			result_SSIM += SSIM(croppedImage_1, croppedImage_2);
		}
	}

	result_SSIM = result_SSIM / ((mat_1.cols - size_window_y) * (mat_1.rows - size_window_x));

	double end_time = clock();

	cout << "result SSIM = " << result_SSIM << endl;
	cout << "time of work = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;
	cout << endl;
}

Point2f* Get_image_points(const string& path)
{
	FILE* pFile = fopen(path.c_str(), "rb");
	char buffer[16384];
	FileReadStream is(pFile, buffer, sizeof(buffer));

	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>(is);

	// Получаем координаты четырёхугольника
	Point2f* input_quad = new Point2f[4];

	for (int i = 0; i < 4; i++)
	{
		input_quad[i] = Point2f(document["quad"][i][0].GetInt(), document["quad"][i][1].GetInt());
	}

	return input_quad;
}
Point2f* Get_noise(Point2f*& quad, const int& dx, const int& dy, const int& height, const int& width)
{
	Point2f* result = new Point2f[4];

	for (int i = 0; i < 4; i++)
	{
		result[i].x = quad[i].x + round((rand() / double(RAND_MAX)) * 2 * dx - dx);
		result[i].y = quad[i].y + round((rand() / double(RAND_MAX)) * 2 * dy - dy);

		if (result[i].x < 0)
		{
			result[i].x = 0;
		}
		if (result[i].x >= width)
		{
			result[i].x = width - 1;
		}

		if (result[i].y < 0)
		{
			result[i].y = 0;
		}
		if (result[i].y >= height)
		{
			result[i].y = height - 1;
		}
	}

	return result;
}
Point2f* Get_noise_const(Point2f*& quad, const int& dx, const int& dy, const int& height, const int& width)
{
	Point2f* result = new Point2f[4];

	result[3].x = quad[3].x + dx;
	result[3].y = quad[3].y + dy;
	result[0].x = quad[0].x + dx;
	result[0].y = quad[0].y + dy;
	result[1].x = quad[1].x - dx;
	result[1].y = quad[1].y - dy;
	result[2].x = quad[2].x - dx;
	result[2].y = quad[2].y - dy;

	for (int i = 0; i < 4; i++)
	{
		if (result[i].x < 0)
		{
			result[i].x = 0;
		}
		if (result[i].x >= width)
		{
			result[i].x = width - 1;
		}
		if (result[i].y < 0)
		{
			result[i].y = 0;
		}
		if (result[i].y >= height)
		{
			result[i].y = height - 1;
		}
	}

	return result;
}
Mat Perspective_transform(const Mat& src, const Mat& reference_image, Point2f*& input_quad)
{
	// Определяем координаты прямоугольника
	Point2f* output_square = new Point2f[4];
	output_square[0] = Point2f(0, 0);
	output_square[1] = Point2f(reference_image.cols, 0);
	output_square[2] = Point2f(reference_image.cols, reference_image.rows);
	output_square[3] = Point2f(0, reference_image.rows);

	Mat lambda = Mat::zeros(src.rows, src.cols, src.type());

	lambda = getPerspectiveTransform(input_quad, output_square);

	Mat result_image = Mat(reference_image.rows, reference_image.cols, reference_image.type());
	warpPerspective(src, result_image, lambda, result_image.size());

	return result_image;
}

double* Dataset_SSIM(const vector<string>& images_paths, const vector<string>& ground_truth_paths, const Mat& reference_image, const string& file_name, const int& dx, const int& dy)
{
	double* SSIM_results = new double[images_paths.size()]; // Результаты сравнения изображений
	Mat distorted_image; // Искажённое изображение

	double start_time = clock(); // Засекаем время

	for (int i = 0; i < images_paths.size(); i++)
	{
		cout << "Работаем с изображением № " << i + 1 << endl;

		distorted_image = imread(images_paths[i]);

		Point2f* input_quad = new Point2f[4];
		input_quad = Get_image_points(ground_truth_paths[i]);

		input_quad = Get_noise_const(input_quad, dx, dy, distorted_image.rows, distorted_image.cols); // "Зашумляем" координаты четырёхугольника

		Mat result_image = Perspective_transform(distorted_image, reference_image, input_quad);

		SSIM_results[i] = SSIM(reference_image, result_image);
	}

	double end_time = clock(); // Получаем итоговое время

	cout << "total running time of the algorithm = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;

	// Записываем результаты в файл
	ofstream file;
	file.open(file_name);
	if (file.is_open())
	{
		for (int i = 0; i < images_paths.size(); i++)
		{
			file << SSIM_results[i] << endl;
		}
	}
	file.close();

	return SSIM_results;
}
double* Distorted_images_SSIM(const Mat& image, const Point2f* quad, const Mat& reference_image, const string& file_name)
{
	double* SSIM_results = new double[20]; // Результаты сравнения изображений

	Mat temp_distorted_image; // Искажённое изображение

	double start_time = clock(); // Засекаем время

	for (int i = 0; i < 20; i++)
	{
		cout << "Работаем с изображением № " << i + 1 << endl;

		temp_distorted_image = image.clone();
		Point2f* distorted_quad = new Point2f[4];

		for (int j = 0; j < 4; j++)
		{
			distorted_quad[j].x = quad[j].x;
			distorted_quad[j].y = quad[j].y;
		}

		distorted_quad = Get_noise_const(distorted_quad, (i + 1) * 4, (i + 1) * 4, image.rows, image.cols);

		Mat result_image = Perspective_transform(temp_distorted_image, reference_image, distorted_quad);

		SSIM_results[i] = SSIM(reference_image, result_image);
	}

	double end_time = clock(); // Получаем итоговое время

	cout << "total running time of the algorithm = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;

	// Записываем результаты в файл
	ofstream file;
	file.open(file_name);
	if (file.is_open())
	{
		for (int i = 0; i < 20; i++)
		{
			file << SSIM_results[i] << endl;
		}
	}
	file.close();

	return SSIM_results;
}

void MIDV500()
{
	char images_Path[MAX_PATH] = "MIDV-500\\01_alb_id\\images\\TS\\"; // Путь к папке с искажёнными изображениями

	string image_extention = ".tif";
	vector<string> images_paths = Get_paths(images_Path, image_extention); // Хранятся пути к искажённым изображениям

	char ground_truth_Path[MAX_PATH] = "MIDV-500\\01_alb_id\\ground_truth\\TS\\"; // Путь к папке с json файлами

	string ground_truth_extention = ".json";
	vector<string> ground_truth_paths = Get_paths(ground_truth_Path, ground_truth_extention); // Хранятся пути к json файлам

	Mat reference_image = imread("MIDV-500\\01_alb_id\\images\\01_alb_id.tif"); // Эталонное изображение

	if (images_paths.size() != ground_truth_paths.size())
	{
		cout << "Количество изображений в папках не совпадает с количеством json файлов" << endl;
		exit(-1);
	}

	cout << "Всего найдено изображений - " << images_paths.size() << endl;
	cout << endl;

	for (int i = 0; i < images_paths.size(); i++)
	{
		images_paths[i] = images_Path + images_paths[i];
		ground_truth_paths[i] = ground_truth_Path + ground_truth_paths[i];
	}

	int dx = 0;
	int dy = 0;
	double* SSIM_results = Dataset_SSIM(images_paths, ground_truth_paths, reference_image, "results\\SSIM_results.txt", dx, dy);

	dx = 20;
	dy = 20;
	double* SSIM_results_20 = Dataset_SSIM(images_paths, ground_truth_paths, reference_image, "results\\SSIM_results_20_new.txt", dx, dy);

	dx = 50;
	dy = 50;
	double* SSIM_results_50 = Dataset_SSIM(images_paths, ground_truth_paths, reference_image, "results\\SSIM_results_50_new.txt", dx, dy);

	dx = 100;
	dy = 100;
	double* SSIM_results_100 = Dataset_SSIM(images_paths, ground_truth_paths, reference_image, "results\\SSIM_results_100_new.txt", dx, dy);

	for (int i = 1; i < 5; i++)
	{
		Mat distorted_image = imread(images_paths[i]);
		Point2f* input_quad = new Point2f[4];
		input_quad = Get_image_points(ground_truth_paths[i]);

		string file_path = "results\\SSIM_results_image_" + to_string(i + 1) + ".txt";

		double* SSIM_results = Distorted_images_SSIM(distorted_image, input_quad, reference_image, file_path);
	}
}

int main(int argc, char** argv)
{
	system("chcp 1251");

	MIDV500();
}
