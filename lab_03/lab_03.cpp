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

Mat Image_extention_constant(Mat& mat, int x, int y, int constant)
{
	Mat result(mat.rows + y - 1, mat.cols + x - 1, mat.type(), Scalar(constant));

	// Заполняем основную сцену нового изображения
	int i_original = 0;
	for (int i = round(y / 2); i < result.rows - round(y / 2); i++)
	{
		int j_original = 0;
		for (int j = round(x / 2); j < result.cols - round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = mat.at<uint8_t>(i_original, j_original);
			j_original++;
		}
		i_original++;
	}

	imshow("Result image extention", result);
	waitKey(0);

	return result;
}
Mat Image_extension_average(Mat& mat, int x, int y)
{
	int average = 0;
	for (int i = 0; i < mat.cols; i++)
	{
		for (int j = 0; j < mat.rows; j++)
		{
			average += mat.at<uint8_t>(j, i);
		}
	}

	average = round(average / mat.cols / mat.rows);

	Mat result(mat.rows + y - 1, mat.cols + x - 1, mat.type(), Scalar(average));

	// Заполняем основную сцену нового изображения
	int i_original = 0;
	for (int i = round(y / 2); i < result.rows - round(y / 2); i++)
	{
		int j_original = 0;
		for (int j = round(x / 2); j < result.cols - round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = mat.at<uint8_t>(i_original, j_original);
			j_original++;
		}
		i_original++;
	}

	imshow("Result image extention", result);
	waitKey(0);

	return result;
}
Mat Image_extension_continue_image(Mat& mat, int x, int y)
{
	Mat result(mat.rows + y - 1, mat.cols + x - 1, mat.type(), Scalar(0));

	// Заполняем основную сцену нового изображения
	int i_original = 0;
	for (int i = round(y / 2); i < result.rows - round(y / 2); i++)
	{
		int j_original = 0;
		for (int j = round(x / 2); j < result.cols - round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = mat.at<uint8_t>(i_original, j_original);
			j_original++;
		}
		i_original++;
	}

	// Заполняем края
	for (int i = round(y / 2) - 1; i >= 0; i--)
	{
		for (int j = round(x / 2); j < result.cols - round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i + 1, j);
		}
	}
	for (int i = result.rows - round(y / 2); i < result.rows; i++)
	{
		for (int j = round(x / 2); j < result.cols - round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i - 1, j);
		}
	}
	for (int i = round(y / 2); i < result.rows - round(y / 2); i++)
	{
		for (int j = round(x / 2) - 1; j >= 0; j--)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i, j + 1);
		}
	}
	for (int i = round(y / 2); i < result.rows - round(y / 2); i++)
	{
		for (int j = result.cols - round(x / 2); j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i, j - 1);
		}
	}

	// Заполняем углы
	int value = result.at<uint8_t>(round(y / 2), round(x / 2));
	//cout << value << endl;
	for (int i = 0; i < round(y / 2); i++)
	{
		for (int j = 0; j < round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = value;
		}
	}

	value = result.at<uint8_t>(round(y / 2), result.cols - round(x / 2) - 1);
	//cout << value << endl;
	for (int i = 0; i < round(y / 2); i++)
	{
		for (int j = result.cols - round(x / 2); j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = value;
		}
	}

	value = result.at<uint8_t>(result.rows - round(y / 2) - 1, round(x / 2));
	for (int i = result.rows - round(y / 2); i < result.rows; i++)
	{
		for (int j = 0; j < round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = value;
		}
	}

	value = result.at<uint8_t>(result.rows - round(y / 2) - 1, result.cols - round(x / 2) - 1);
	//cout << value << endl;
	for (int i = result.rows - round(y / 2); i < result.rows; i++)
	{
		for (int j = result.cols - round(x / 2); j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = value;
		}
	}

	imshow("Result image extention", result);
	waitKey(0);

	return result;
}
Mat Image_extension_reflect_image(Mat& mat, int x, int y)
{
	Mat result(mat.rows + y - 1, mat.cols + x - 1, mat.type(), Scalar(0));

	if ((mat.rows * 2 < result.rows) || (mat.cols * 2 < result.cols))
	{
		cout << "Very big values for height or width" << endl;
		exit(-2);
	}

	// Заполняем основную сцену нового изображения
	int i_original = 0;
	for (int i = round(y / 2); i < result.rows - round(y / 2); i++)
	{
		int j_original = 0;
		for (int j = round(x / 2); j < result.cols - round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = mat.at<uint8_t>(i_original, j_original);
			j_original++;
		}
		i_original++;
	}

	// Заполняем края
	int i_value = round(y / 2);
	for (int i = round(y / 2) - 1; i >= 0; i--)
	{
		for (int j = 0; j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i_value, j);
		}
		i_value++;
	}
	i_value = result.rows - round(y / 2) - 1;
	for (int i = result.rows - round(y / 2); i < result.rows; i++)
	{
		for (int j = 0; j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i_value, j);
		}
		i_value--;
	}
	for (int i = 0; i < result.rows; i++)
	{
		int j_value = round(x / 2);
		for (int j = round(x / 2) - 1; j >= 0; j--)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i, j_value);
			j_value++;
		}
	}
	for (int i = 0; i < result.rows; i++)
	{
		int j_value = result.cols - round(x / 2) - 1;
		for (int j = result.cols - round(x / 2); j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i, j_value);
			j_value--;
		}
	}

	imshow("Result image extention", result);
	waitKey(0);

	return result;
}
Mat Image_extension_back_reflect_image(Mat& mat, int x, int y)
{
	Mat result(mat.rows + y - 1, mat.cols + x - 1, mat.type(), Scalar(0));

	if ((mat.rows * 2 < result.rows) || (mat.cols * 2 < result.cols))
	{
		cout << "Very big values for height or width" << endl;
		exit(-2);
	}

	// Заполняем основную сцену нового изображения
	int i_original = 0;
	for (int i = round(y / 2); i < result.rows - round(y / 2); i++)
	{
		int j_original = 0;
		for (int j = round(x / 2); j < result.cols - round(x / 2); j++)
		{
			result.at<uint8_t>(i, j) = mat.at<uint8_t>(i_original, j_original);
			j_original++;
		}
		i_original++;
	}

	// Заполняем края
	int i_value = result.rows - round(y / 2) - 1;
	for (int i = round(y / 2) - 1; i >= 0; i--)
	{
		for (int j = 0; j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i_value, j);
		}
		i_value--;
	}
	i_value = round(y / 2);
	for (int i = result.rows - round(y / 2); i < result.rows; i++)
	{
		for (int j = 0; j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i_value, j);
		}
		i_value++;
	}
	for (int i = 0; i < result.rows; i++)
	{
		int j_value = result.cols - round(x / 2) - 1;
		for (int j = round(x / 2) - 1; j >= 0; j--)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i, j_value);
			j_value--;
		}
	}
	for (int i = 0; i < result.rows; i++)
	{
		int j_value = round(x / 2);
		for (int j = result.cols - round(x / 2); j < result.cols; j++)
		{
			result.at<uint8_t>(i, j) = result.at<uint8_t>(i, j_value);
			j_value++;
		}
	}

	imshow("Result image extention", result);
	waitKey(0);

	return result;
}

// square_x - длина прямоугольного окна
// square_y - ширина прямоугольного окна
// k - заранее подобранный коэффициент, определяющий значимость стандартного отклонения
void Nibleck_binarization(Mat& mat, int square_x, int square_y, double k)
{
	if ((square_x % 2 == 0) || (square_y % 2 == 0))
	{
		cout << "Bad values of window" << endl;
		exit(-1);
	}
	// Сначала необходимо "расширить" картинку одним из известных методов
	//Image_extention_constant(mat, square_x, square_y, 128);
	//Image_extension_average(mat, square_x, square_y);
	//Mat extension_image = Image_extension_continue_image(mat, square_x, square_y);
	//Mat extension_image = Image_extension_reflect_image(mat, square_x, square_y);
	Mat extension_image = Image_extension_back_reflect_image(mat, square_x, square_y);

	// Создаём матрицу со значениями порога для каждого пикселя
	double** step_values = new double* [mat.cols];
	//vector<vector<double>> step_values(mat.cols, vector<double>(mat.rows));


	for (int i = 0; i < mat.cols; i++)
	{
		step_values[i] = new double[mat.rows];
	}

	int count_of_anomalous_values = 0;
	int i_original = 0;
	for (int i = round(square_y / 2); i < extension_image.rows - round(square_y / 2); i++)
	{
		int j_original = 0;
		cout << "step = " << i << endl;
		for (int j = round(square_x / 2); j < extension_image.cols - round(square_x / 2); j++)
		{
			int m = 0; // Среднее значение яркости пикселей в пределах данного окна

			for (int ii = i - (square_y - 1) / 2; ii <= i + (square_y - 1) / 2; ii++)
			{
				for (int jj = j - (square_x - 1) / 2; jj <= j + (square_x - 1) / 2; jj++)
				{
					m += extension_image.at<uint8_t>(ii, jj);
				}
			}

			m = round(m / (square_x * square_y));

			double S = 0; // Стандартное отклонение

			for (int ii = i - (square_y - 1) / 2; ii <= i + (square_y - 1) / 2; ii++)
			{
				for (int jj = j - (square_x - 1) / 2; jj <= j + (square_x - 1) / 2; jj++)
				{
					S += ((double)extension_image.at<uint8_t>(ii, jj) - m) * ((double)extension_image.at<uint8_t>(ii, jj) - m);
				}
			}

			S = sqrt(S / ((double)square_x * (double)square_y));
			//cout << "S = " << S << endl;

			step_values[i_original][j_original] = m + k * S;
			if ((step_values[i_original][j_original] < 0) || (step_values[i_original][j_original] > 255))
			{
				count_of_anomalous_values++;
			}

			j_original++;
		}
		i_original++;
	}

	cout << "All completed" << endl;
	cout << "Count of anomalous values - " << count_of_anomalous_values << endl;

	//for (int i = 0; mat.cols; i++)
	//{
	//	for (int j = 0; mat.rows; j++)
	//	{
	//		cout << step_values[i][j] << " ";
	//	}
	//	cout << endl;
	//}

	//// Процесс бинаризации
	//for (int i = 0; i < mat.cols; i++)
	//{
	//	for (int j = 0; j < mat.rows; j++)
	//	{
	//		if (mat.at<uint8_t>(j, i) < step_values[i][j])
	//		{
	//			mat.at<uint8_t>(j, i) = 0;
	//		}
	//		else
	//		{
	//			mat.at<uint8_t>(j, i) = 255;
	//		}
	//	}
	//}

	//imshow("Binary cutie cat", mat);
	//waitKey(0);
}
void Local_binarization(Mat& bw_mat)
{
	// Реализовать метод Ниблека локальной бинаризации
	Nibleck_binarization(bw_mat, 5, 5, 0.2);
}

int main()
{
	Mat mat = imread("Cutie_cat.jpg");

	Mat bw_mat;
	cvtColor(mat, bw_mat, COLOR_BGR2GRAY);
	Mat binary_mat = bw_mat.clone();

	imshow("Black-white cat", binary_mat);
	waitKey(0);

	cout << "Mini-result " << round(5 / 2) << endl;

	Local_binarization(binary_mat);

	//Mat result = Global_binarization(binary_mat);

	//imshow("Cutie cat", mat);
	//waitKey(0);

	//imshow("Black-white cutie cat", bw_mat);
	//waitKey(0);

	//imshow("Binarization cutie cat", result);
	//waitKey(0);

	return 0;
}
