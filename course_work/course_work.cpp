#include <opencv2/opencv.hpp>

#include <windows.h>
#include <fstream>
#include <ctime>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace cv;
using namespace std;
using namespace rapidjson;

// Сортировка точек по возрастанию значения y
int** Sort_points(int** points)
{
	int** result = new int* [4];

	for (int i = 0; i < 4; i++)
	{
		result[i] = new int[2];
		result[i][0] = points[i][0];
		result[i][1] = points[i][1];
	}

	for (int i = 0; i < 4; i++)
	{
		int min = result[i][1];
		int i_min = i;
		for (int j = i; j < 4; j++)
		{
			if (result[j][1] < min)
			{
				min = result[j][1];
				i_min = j;
			}
		}
		int helper = result[i][0];
		result[i][0] = result[i_min][0];
		result[i_min][0] = helper;

		helper = result[i][1];
		result[i][1] = result[i_min][1];
		result[i_min][1] = helper;
	}

	cout << "Отсортированные точки:" << endl;
	for (int i = 0; i < 4; i++)
	{
		cout << "(" << result[i][0] << ", " << result[i][1] << ")" << endl;
	}
	cout << endl;

	return result;
}
// Решение СЛАУ методом Гаусса
void Gauss_method(const int**& quad, const double**& square)
{
	int i, j;
	int n = 8;
	int m = 8;

	float** matrix = new float* [n];
	for (i = 0; i < n; i++)
	{
		matrix[i] = new float[m];
	}

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			cout << " Element " << "[" << i + 1 << " , " << j + 1 << "]: ";

			matrix[i][j] = 0.0;
		}
	}

	for (i = 0; i < 8; i = i + 2)
	{
		matrix[i][0] = square[i / 2][0];
		matrix[i][1] = square[i / 2][1];
		matrix[i][2] = 1;
		matrix[i][6] = -quad[i / 2][0] * square[i / 2][0];
		matrix[i][7] = -quad[i / 2][0] * square[i / 2][1];

		matrix[i + 1][3] = square[i / 2][0];
		matrix[i + 1][4] = square[i / 2][1];
		matrix[i + 1][5] = 1;
		matrix[i + 1][6] = -quad[i / 2][1] * square[i / 2][0];
		matrix[i + 1][7] = -quad[i / 2][1] * square[i / 2][1];
	}

	//выводим массив
	cout << "matrix: " << endl;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	//Метод Гаусса
	//Прямой ход, приведение к верхнетреугольному виду
	float  tmp;
	int k;
	float* xx = new float[m];

	for (i = 0; i < n; i++)
	{
		tmp = matrix[i][i];
		for (j = n; j >= i; j--)
		{
			matrix[i][j] /= tmp;
		}
		for (j = i + 1; j < n; j++)
		{
			tmp = matrix[j][i];
			for (k = n; k >= i; k--)
			{
				matrix[j][k] -= tmp * matrix[i][k];
			}
		}
	}
	/*обратный ход*/
	xx[n - 1] = matrix[n - 1][n];
	for (i = n - 2; i >= 0; i--)
	{
		xx[i] = matrix[i][n];
		for (j = i + 1; j < n; j++)
		{
			xx[i] -= matrix[i][j] * xx[j];
		}
	}

	//Выводим решения
	for (i = 0; i < n; i++)
	{
		cout << xx[i] << " ";
	}
	cout << endl;

	delete[] matrix;
	system("pause");
}
double* Gauss(double** a, double* y, int n)
{
	double* x, max;
	int k, index;
	const double eps = 0.00001;  // точность
	x = new double[n];
	k = 0;
	while (k < n)
	{
		// Поиск строки с максимальным a[i][k]
		max = abs(a[k][k]);
		index = k;
		for (int i = k + 1; i < n; i++)
		{
			if (abs(a[i][k]) > max)
			{
				max = abs(a[i][k]);
				index = i;
			}
		}
		// Перестановка строк
		if (max < eps)
		{
			// нет ненулевых диагональных элементов
			cout << "Решение получить невозможно из-за нулевого столбца ";
			cout << index << " матрицы A" << endl;
			return 0;
		}
		for (int j = 0; j < n; j++)
		{
			double temp = a[k][j];
			a[k][j] = a[index][j];
			a[index][j] = temp;
		}
		double temp = y[k];
		y[k] = y[index];
		y[index] = temp;
		// Нормализация уравнений
		for (int i = k; i < n; i++)
		{
			double temp = a[i][k];
			if (abs(temp) < eps) continue; // для нулевого коэффициента пропустить
			for (int j = 0; j < n; j++)
				a[i][j] = a[i][j] / temp;
			y[i] = y[i] / temp;
			if (i == k)  continue; // уравнение не вычитать само из себя
			for (int j = 0; j < n; j++)
				a[i][j] = a[i][j] - a[k][j];
			y[i] = y[i] - y[k];
		}
		k++;
	}
	// обратная подстановка
	for (k = n - 1; k >= 0; k--)
	{
		x[k] = y[k];
		for (int i = 0; i < k; i++)
			y[i] = y[i] - a[i][k] * x[k];
	}
	return x;
}
// Вывод в консоль системы уравнений
void sysout(double** a, double* y, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << a[i][j] << "*x" << j;
			if (j < n - 1)
				cout << " + ";
		}
		cout << " = " << y[i] << endl;
	}
	cout << endl;
}
// Поиск обратной матрицы
double** Inversion(double** A, int N)
{
	double temp;

	double** E = new double* [N];

	for (int i = 0; i < N; i++)
		E[i] = new double[N];

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			E[i][j] = 0.0;

			if (i == j)
				E[i][j] = 1.0;
		}

	for (int k = 0; k < N; k++)
	{
		temp = A[k][k];

		for (int j = 0; j < N; j++)
		{
			A[k][j] /= temp;
			E[k][j] /= temp;
		}

		for (int i = k + 1; i < N; i++)
		{
			temp = A[i][k];

			for (int j = 0; j < N; j++)
			{
				A[i][j] -= A[k][j] * temp;
				E[i][j] -= E[k][j] * temp;
			}
		}
	}

	for (int k = N - 1; k > 0; k--)
	{
		for (int i = k - 1; i >= 0; i--)
		{
			temp = A[i][k];

			for (int j = 0; j < N; j++)
			{
				A[i][j] -= A[k][j] * temp;
				E[i][j] -= E[k][j] * temp;
			}
		}
	}

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			A[i][j] = E[i][j];

	for (int i = 0; i < N; i++)
		delete[] E[i];

	delete[] E;

	return A;
}

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

Mat Transform_image_cropping(const Mat& mat)
{
	string path = "MIDV-500\\01_alb_id\\ground_truth\\TS\\TS01_01.json";
	
	FILE* pFile = fopen(path.c_str(), "rb");
	char buffer[16384];
	FileReadStream is(pFile, buffer, sizeof(buffer));

	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>(is);

	// Получаем координаты четырёхугольника
	cout << "Координаты четырёхугольника:" << endl;
	int** quad = new int* [4];
	for (int i = 0; i < 4; i++)
	{
		quad[i] = new int[2];
		quad[i][0] = document["quad"][i][0].GetInt();
		quad[i][1] = document["quad"][i][1].GetInt();

		cout << "(" << quad[i][0] << ", " << quad[i][1] << ")" << endl;
	}
	cout << endl;

	// Определяем координаты прямоугольника
	int** square = new int* [4]; // (x, y) - координаты прямоугольника на исходной картинке
	for (int i = 0; i < 4; i++)
	{
		square[i] = new int[2];
	}

	square[0][0] = 0;
	square[0][1] = 0;
	square[1][0] = quad[2][0] - quad[0][0];
	square[1][1] = 0;
	square[2][0] = quad[2][0] - quad[0][0];
	square[2][1] = quad[2][1] - quad[0][1];
	square[3][0] = 0;
	square[3][1] = quad[2][1] - quad[0][1];

	// Вывод координат прямоугольника на экран
	cout << "Координаты прямоугольника" << endl;
	for (int i = 0; i < 4; i++)
	{
		cout << "(" << square[i][0] << ", " << square[i][1] << ")" << endl;
	}
	cout << endl;

	// Решение СЛАУ
	int n = 8;
	double** matrix = new double* [n]; // Матрица 8*8 для нахождения коэффициентов a, b, c..
	for (int i = 0; i < n; i++)
	{
		matrix[i] = new double[n];
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			matrix[i][j] = 0;
		}
	}

	double* y = new double[n]; // Вектор для поиска решения СЛАУ

	// Заполнение матрицы для решения СЛАУ
	for (int i = 0; i < 8; i = i + 2)
	{
		matrix[i][0] = square[i / 2][0];
		matrix[i][1] = square[i / 2][1];
		matrix[i][2] = 1;
		matrix[i][6] = -(double)quad[i / 2][0] * square[i / 2][0];
		matrix[i][7] = -(double)quad[i / 2][0] * square[i / 2][1];

		matrix[i + 1][3] = square[i / 2][0];
		matrix[i + 1][4] = square[i / 2][1];
		matrix[i + 1][5] = 1;
		matrix[i + 1][6] = -(double)quad[i / 2][1] * square[i / 2][0];
		matrix[i + 1][7] = -(double)quad[i / 2][1] * square[i / 2][1];

		y[i] = quad[i / 2][0];
		y[i + 1] = quad[i / 2][1];
	}

	sysout(matrix, y, n); // Вывод СЛАУ на экран

	double* mas_result = Gauss(matrix, y, n); // Результат решения СЛАУ

	// Вывод результата решения СЛАУ на экран
	cout << "Результат решения СЛАУ:" << endl;
	for (int i = 0; i < n; i++)
	{
		cout << mas_result[i] << endl;
	}
	cout << endl;

	// Преобразование решения СЛАУ (a, b, c..) в матрицу (массив -> матрица)
	double** matrix_result = new double* [3];
	for (int i = 0; i < 3; i++)
	{
		matrix_result[i] = new double[3];
	}

	matrix_result[0][0] = mas_result[0];
	matrix_result[0][1] = mas_result[1];
	matrix_result[0][2] = mas_result[2];
	matrix_result[1][0] = mas_result[3];
	matrix_result[1][1] = mas_result[4];
	matrix_result[1][2] = mas_result[5];
	matrix_result[2][0] = mas_result[6];
	matrix_result[2][1] = mas_result[7];
	matrix_result[2][2] = 1;

	// Проверка границ на правильность преобразования
	cout << "Проверка границ на правильность преобразования" << endl;
	for (int i = 0; i < 4; i++)
	{
		double W = mas_result[6] * square[i][0] + mas_result[7] * square[i][1] + 1;
		int x = round(mas_result[0] * square[i][0] + mas_result[1] * square[i][1] + mas_result[2]); // + 188
		int y = round(mas_result[3] * square[i][0] + mas_result[4] * square[i][1] + mas_result[5]); // + 735

		cout << "(" << square[i][0] << ", " << square[i][1] << ") -> " << "(" << x << ", " << y << ")" << endl;
	}
	cout << endl;
	exit(0);

	Mat square_mat(square[2][1], square[2][0], mat.type());

	// Более простой способ
	for (int i = 0; i < square_mat.cols; i++)
	{
		for (int j = 0; j < square_mat.rows; j++)
		{
			double W = mas_result[6] * i + mas_result[7] * j + 1;
			int x = round(mas_result[0] * i + mas_result[1] * j + mas_result[2]);
			int y = round(mas_result[3] * i + mas_result[4] * j + mas_result[5]);

			cout << "(x, y) = (" << x << ", " << y << ")" << endl;

			for (int k = 0; k < 3; k++)
			{
				square_mat.at<Vec3b>(i, j)[k] = mat.at<Vec3b>(x, y)[k];
			}
		}
		cout << endl;
	}

	imshow("Result_image", square_mat);
	waitKey(0);

	exit(0);

	return square_mat;
}
Mat Image_cropping(const Mat& mat)
{
	string path = "MIDV-500\\01_alb_id\\ground_truth\\TS\\TS01_01.json";
	//ifstream my_stream(path);

	FILE* pFile = fopen(path.c_str(), "rb");
	char buffer[16384];
	FileReadStream is(pFile, buffer, sizeof(buffer));

	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>(is);

	// Получаем координаты четырёхугольника
	int** quad = new int* [4];
	for (int i = 0; i < 4; i++)
	{
		quad[i] = new int[2];
		quad[i][0] = document["quad"][i][0].GetInt();
		quad[i][1] = document["quad"][i][1].GetInt();
	}

	int start_x = quad[0][0];
	int start_y = quad[0][1];
	int width = quad[2][0] - quad[0][0];
	int height = quad[2][1] - quad[0][1];

	Mat ROI(mat, Rect(start_x, start_y, width, height));
	Mat result_image;
	ROI.copyTo(result_image);

	//imshow("Cropped image", result_image);
	//waitKey(0);

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
	char cPath[MAX_PATH] = "MIDV-500\\01_alb_id\\images\\TS\\"; // Путь к папке с искажёнными изображениями

	string image_extention = ".tif";
	vector<string> paths = Get_paths(cPath, image_extention); // Хранятся пути к искажённым изображениям

	Mat distorted_image; // Искажённое изображение
	Mat reference_image = imread("MIDV-500\\01_alb_id\\images\\01_alb_id.tif"); // Эталонное изображение

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
		Mat cropped_distorted_image = Transform_image_cropping(distorted_image); // Обрезаем изображение
		//Mat cropped_distorted_image = Image_cropping(distorted_image); // Обрезаем изображение

		cout << "cropped_image size = " << cropped_distorted_image.size() << endl;
		cout << "reference image size = " << reference_image.size() << endl;

		Mat reference_image_clone = reference_image.clone();
		Mat resize_reference_image;

		double resize_coef_cols = (double)cropped_distorted_image.cols / reference_image.cols;
		double resize_coef_rows = (double)cropped_distorted_image.rows / reference_image.rows;

		cout << "resize_coef_cols = " << resize_coef_cols << endl;
		cout << "resize_coef_rows = " << resize_coef_rows << endl;

		resize(reference_image_clone, resize_reference_image, Size(), resize_coef_cols, resize_coef_rows);
		
		cout << "resize image size = " << resize_reference_image.size() << endl;

		//imshow("Result", resize_reference_image);
		//waitKey(0);

		SSIM_results[i] = SSIM(resize_reference_image, cropped_distorted_image); // Находим значение SSIM
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
	system("chcp 1251");

	//TID2013();
	MIDV500();
}
