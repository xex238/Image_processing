#include <opencv2/opencv.hpp>

#include <windows.h>
#include <fstream>
#include <ctime>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace cv;
using namespace std;
using namespace rapidjson;

// Получение набора путей к файлам заданного формата
vector<string> Get_filepaths(const LPSTR& cPath, const string& extention)
{
	WIN32_FIND_DATA fd;
	HANDLE hFile;
	short len = lstrlen(cPath);
	lstrcat(cPath, "*");
	hFile = FindFirstFile(cPath, &fd);
	cPath[len] = 0;

	vector<string> paths;

	do 
	{
		if (fd.cFileName[0] == '.')
		{
			continue;
		}
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			lstrcat(cPath, fd.cFileName);
			lstrcat(cPath, "\\");
			Get_filepaths(cPath, extention);
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
// Получение набора путей к папкам в заданной директории
vector<string> Get_directory_paths(const LPSTR& cPath)
{
	WIN32_FIND_DATA fd;
	HANDLE hFile;
	short len = lstrlen(cPath);
	lstrcat(cPath, "*");
	hFile = FindFirstFile(cPath, &fd);
	cPath[len] = 0;

	vector<string> paths;

	do
	{
		if (fd.cFileName[0] == '.')
		{
			continue;
		}
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			LPSTR cPath_copy;
			cPath_copy = cPath;
			lstrcat(cPath_copy, fd.cFileName);
			lstrcat(cPath_copy, "\\");
			paths.push_back(cPath_copy);
			//Get_filepaths(cPath, extention);
			cPath[len] = 0;
		}
		//else
		//{
		//	char* tmp = strrchr(fd.cFileName, '.');
		//	if (tmp && !strcmp(tmp, extention.c_str()))
		//	{
		//		paths.push_back(fd.cFileName);
		//	}
		//}
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
Point2f* Get_noise(Point2f*& quad, const int& d, const int& height, const int& width, double& distorted_x)
{
	Point2f* result = new Point2f[4];
	distorted_x = 0;

	for (int i = 0; i < 4; i++)
	{
		srand(time(NULL) + rand());

		int dx = round(rand() / double(RAND_MAX) * d);
		int dy = round(sqrt(d * d - dx * dx));

		if ((rand() / double(RAND_MAX)) < 0.5)
		{
			dx = -dx;
		}
		if ((rand() / double(RAND_MAX)) < 0.5)
		{
			dy = -dy;
		}

		distorted_x += sqrt(dx * dx + dy * dy);

		result[i].x = quad[i].x + dx;
		result[i].y = quad[i].y + dy;

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

	distorted_x = distorted_x / 4;

	//cout << "Old values:" << endl;
	//for (int i = 0; i < 4; i++)
	//{
	//	cout << "(" << quad[i].x << ", " << quad[i].y << ")" << endl;
	//}
	//cout << endl;

	//cout << "New values:" << endl;
	//for (int i = 0; i < 4; i++)
	//{
	//	cout << "(" << result[i].x << ", " << result[i].y << ")" << endl;
	//}
	//cout << endl;

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

void Dataset_SSIM(const vector<string>& images_paths, const vector<string>& ground_truth_paths, const Mat& reference_image, const string& x_filepath, const string& y_filepath, const int& d, double*& x, double*& y, int& counter)
{
	Mat distorted_image; // Искажённое изображение
	int counter_copy = counter;

	double start_time = clock(); // Засекаем время

	for (int i = 0; i < images_paths.size(); i++)
	{
		cout << "Работаем с изображением № " << i + 1 << endl;

		distorted_image = imread(images_paths[i]);

		Point2f* input_quad = new Point2f[4];
		input_quad = Get_image_points(ground_truth_paths[i]);

		input_quad = Get_noise(input_quad, d, distorted_image.rows, distorted_image.cols, x[counter_copy]);

		Mat result_image = Perspective_transform(distorted_image, reference_image, input_quad);

		y[counter_copy] = SSIM(reference_image, result_image);
		counter_copy++;
	}

	double end_time = clock(); // Получаем итоговое время

	cout << "total running time of the algorithm = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;

	// Записываем результаты в файл
	counter_copy = counter;
	ofstream x_file;
	x_file.open(x_filepath);
	if (x_file.is_open())
	{
		for (int i = 0; i < images_paths.size(); i++)
		{
			x_file << x[counter_copy] << endl;
			counter_copy++;
		}
	}
	x_file.close();

	counter_copy = counter;
	ofstream y_file;
	y_file.open(y_filepath);
	if (y_file.is_open())
	{
		for (int i = 0; i < images_paths.size(); i++)
		{
			y_file << y[counter_copy] << endl;
			counter_copy++;
		}
	}
	y_file.close();

	counter = counter_copy;
}
void Distorted_images_SSIM(const Mat& image, const Point2f* quad, const Mat& reference_image, const string& x_filepath, const string& y_filepath, double*& x, double*& y, int& counter)
{
	Mat temp_distorted_image; // Искажённое изображение
	int counter_copy = counter;

	double start_time = clock(); // Засекаем время

	for (int i = 0; i < 10; i++)
	{
		cout << "Работаем с изображением № " << i + 1 << endl;

		temp_distorted_image = image.clone();
		Point2f* distorted_quad = new Point2f[4];

		for (int j = 0; j < 4; j++)
		{
			distorted_quad[j].x = quad[j].x;
			distorted_quad[j].y = quad[j].y;
		}

		distorted_quad = Get_noise(distorted_quad, (i + 1) * 2, image.rows, image.cols, x[counter_copy]);

		Mat result_image = Perspective_transform(temp_distorted_image, reference_image, distorted_quad);

		y[counter_copy] = SSIM(reference_image, result_image);
		counter_copy++;
	}

	double end_time = clock(); // Получаем итоговое время

	cout << "total running time of the algorithm = " << (end_time - start_time) / CLOCKS_PER_SEC << endl;

	// Записываем результаты в файл
	counter_copy = counter;
	ofstream x_file;
	x_file.open(x_filepath);
	if (x_file.is_open())
	{
		for (int i = 0; i < 10; i++)
		{
			x_file << x[counter_copy] << endl;
			counter_copy++;
		}
	}
	x_file.close();

	counter_copy = counter;
	ofstream file;
	file.open(y_filepath);
	if (file.is_open())
	{
		for (int i = 0; i < 10; i++)
		{
			file << y[counter_copy] << endl;
			counter_copy++;
		}
	}
	file.close();

	counter = counter_copy;
}

double Calculate_corr(const double* x, const double* y, const int& length)
{
	cout << "x = " << endl;
	for (int i = 0; i < length; i++)
	{
		cout << x[i] << endl;
	}
	cout << endl;

	cout << "y = " << endl;
	for (int i = 0; i < length; i++)
	{
		cout << y[i] << endl;
	}
	cout << endl;

	double average_x = 0;
	double average_y = 0;
	for (int i = 0; i < length; i++)
	{
		average_x += x[i];
		average_y += y[i];
	}
	average_x = average_x / (double)length;
	average_y = average_y / (double)length;

	cout << "average_x = " << average_x << endl;
	cout << "average_y = " << average_y << endl;

	double x_x_sum = 0; // ∑(x-x(ср))^2
	double y_y_sum = 0; // ∑(y-y(ср))^2
	double x_y_sum = 0; // ∑(x-x(ср))(y-y(ср))
	for (int i = 0; i < length; i++)
	{
		x_x_sum += pow((x[i] - average_x), 2);
		y_y_sum += pow((y[i] - average_y), 2);
		x_y_sum += (x[i] - average_x) * (y[i] - average_y);
	}

	cout << "∑(x-x(ср))^2 = " << x_x_sum << endl;
	cout << "∑(y-y(ср))^2 = " << y_y_sum << endl;
	cout << "∑(x-x(ср))(y-y(ср)) = " << x_y_sum << endl;

	double sigma_x = sqrt(x_x_sum / (double)length);
	double sigma_y = sqrt(y_y_sum / (double)length);
	double cov = x_y_sum / (double)length;

	double r = cov / (sigma_x * sigma_y);

	cout << "sigma_x = " << sigma_x << endl;
	cout << "sigma_y = " << sigma_y << endl;
	cout << "cov = " << cov << endl;
	cout << "r = " << r << endl;

	return r;
}
double Calculate_corr(const string& x_filepath, const string& y_filepath, const int& length)
{
	string line;
	double* x = new double[length];
	int length_x = 0;
	int length_y = 0;

	ifstream in_x(x_filepath);
	if (in_x.is_open())
	{
		int i = 0;
		while (getline(in_x, line))
		{
			x[i] = atof(line.c_str());
			i++;
		}
		length_x = i;
	}
	in_x.close();

	double* y = new double[length];

	ifstream in_y(y_filepath);
	if (in_y.is_open())
	{
		int i = 0;
		while (getline(in_y, line))
		{
			for (int i = 0; i < line.size(); i++)
			{
				if (line[i] == ',')
				{
					line[i] = '.';
				}
			}
			y[i] = atof(line.c_str());
			i++;
		}
		length_y = i;
	}
	in_y.close();

	if (length_x == length_y)
	{
		return Calculate_corr(x, y, length_x);
	}
	else
	{
		cout << "Different size of txt files" << endl;
		exit(-1);
	}
}

void Const_distortion_level(const vector<string>& images_paths, const vector<string>& ground_truth_paths, const Mat& reference_image, const int& path_number)
{
	double* x = new double[images_paths.size() * 4]; // Значения искажения
	double* y = new double[images_paths.size() * 4]; // Значения SSIM
	int counter = 0;

	CreateDirectory("results\\const_distortion", NULL);
	string s = "results\\const_distortion\\" + to_string(path_number);
	char path[MAX_PATH];
	strcpy(path, s.c_str());
	CreateDirectory(path, NULL);

	for (int i = 0; i <= 9; i = i + 3)
	{
		string x_filepath = "results\\const_distortion\\" + to_string(path_number) + "\\x_" + to_string(i) + ".txt";
		string y_filepath = "results\\const_distortion\\" + to_string(path_number) + "\\y_" + to_string(i) + ".txt";

		Dataset_SSIM(images_paths, ground_truth_paths, reference_image, x_filepath, y_filepath, i, x, y, counter);
	}

	ofstream x_file;
	x_file.open("results\\const_distortion\\" + to_string(path_number) + "\\x.txt");
	if (x_file.is_open())
	{
		for (int i = 0; i < images_paths.size() * 4; i++)
		{
			x_file << x[i] << endl;
		}
	}
	x_file.close();

	ofstream y_file;
	y_file.open("results\\const_distortion\\" + to_string(path_number) + "\\y.txt");
	if (y_file.is_open())
	{
		for (int i = 0; i < images_paths.size() * 4; i++)
		{
			y_file << y[i] << endl;
		}
	}
	y_file.close();

	ofstream r;
	r.open("results\\const_distortion\\" + to_string(path_number) + "\\r.txt");
	if (r.is_open())
	{
		r << Calculate_corr("results\\const_distortion\\" + to_string(path_number) + "\\x.txt", "results\\const_distortion\\" + to_string(path_number) + "\\y.txt", images_paths.size() * 4) << endl;
	}
	r.close();
}
void Const_images(const vector<string>& images_paths, const vector<string>& ground_truth_paths, const Mat& reference_image, const int& path_number)
{
	double* x = new double[40];
	double* y = new double[40];
	int counter = 0;

	CreateDirectory("results\\const_images", NULL);
	string s = "results\\const_images\\" + to_string(path_number);
	char path[MAX_PATH];
	strcpy(path, s.c_str());
	CreateDirectory(path, NULL);

	int value_counter = 0;
	for (int i = 1; i < 5; i++)
	{
		Mat distorted_image = imread(images_paths[i]);
		Point2f* input_quad = new Point2f[4];
		input_quad = Get_image_points(ground_truth_paths[i]);

		string x_filepath = "results\\const_images\\" + to_string(path_number) + "\\x_image_" + to_string(i + 1) + ".txt";
		string y_filepath = "results\\const_images\\" + to_string(path_number) + "\\y_image_" + to_string(i + 1) + ".txt";

		Distorted_images_SSIM(distorted_image, input_quad, reference_image, x_filepath, y_filepath, x, y, counter);
	}

	ofstream x_file;
	x_file.open("results\\const_images\\" + to_string(path_number) + "\\x.txt");
	if (x_file.is_open())
	{
		for (int i = 0; i < 40; i++)
		{
			x_file << x[i] << endl;
		}
	}
	x_file.close();

	ofstream y_file;
	y_file.open("results\\const_images\\" + to_string(path_number) + "\\y.txt");
	if (y_file.is_open())
	{
		for (int i = 0; i < 40; i++)
		{
			y_file << y[i] << endl;
		}
	}
	y_file.close();

	ofstream r;
	r.open("results\\const_images\\" + to_string(path_number) + "\\r.txt");
	if (r.is_open())
	{
		r << Calculate_corr("results\\const_images\\" + to_string(path_number) + "\\x.txt", "results\\const_images\\" + to_string(path_number) + "\\y.txt", 40) << endl;
	}
	r.close();
}

void MIDV500()
{
	char dataset_Path[MAX_PATH] = "MIDV-500\\"; // Путь к папке с искажёнными изображениями
	vector<string> directory_dataset_paths = Get_directory_paths(dataset_Path); // Получение всех основных имеющихся папок в датасете
	cout << directory_dataset_paths[0] << endl;

	char images_Path[MAX_PATH];
	vector<string> images_paths;

	char ground_truth_Path[MAX_PATH];
	vector<string> ground_truth_paths;

	string reference_image_path;

	CreateDirectory("results", NULL);

	for (int i = 0; i < directory_dataset_paths.size(); i++)
	{
		string s = directory_dataset_paths[i] + "images\\TS\\";
		strcpy(images_Path, s.c_str());

		cout << images_Path << endl;

		string image_extention = ".tif";
		images_paths = Get_filepaths(images_Path, image_extention); // Хранятся пути к искажённым изображениям

		s = directory_dataset_paths[i] + "ground_truth\\TS\\";
		strcpy(ground_truth_Path, s.c_str());

		cout << ground_truth_Path << endl;

		string ground_truth_extention = ".json";
		ground_truth_paths = Get_filepaths(ground_truth_Path, ground_truth_extention);

		int first_slash = directory_dataset_paths[i].find_first_of('\\');
		int last_slash = directory_dataset_paths[i].find_last_of('\\');

		s = directory_dataset_paths[i].substr(first_slash + 1, last_slash - first_slash - 1);

		cout << s << endl;

		string reference_image_path = directory_dataset_paths[i] + "\\images\\" + s + ".tif";

		cout << reference_image_path << endl;

		Mat reference_image = imread(reference_image_path); // Эталонное изображение

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

		Const_distortion_level(images_paths, ground_truth_paths, reference_image, i);
		Const_images(images_paths, ground_truth_paths, reference_image, i);
	}
}

int main(int argc, char** argv)
{
	system("chcp 1251");

	MIDV500();
}
