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

Mat Step_average_binarization(const Mat& bw_mat)
{
	Mat result_bw_mat = bw_mat.clone();

	for (int i = 0; i < result_bw_mat.cols; i++)
	{
		for (int j = 0; j < result_bw_mat.rows; j++)
		{
			if (result_bw_mat.at<uint8_t>(j, i) < 128)
			{
				result_bw_mat.at<uint8_t>(j, i) = 0;
			}
			else
			{
				result_bw_mat.at<uint8_t>(j, i) = 255;
			}
		}
	}

	return result_bw_mat;
}
Mat Step_median_binarization(const Mat& bw_mat)
{
	Mat result_bw_mat = bw_mat.clone();

	int median = 0;

	for (int i = 0; i < result_bw_mat.rows; i++)
	{
		for (int j = 0; j < result_bw_mat.cols; j++)
		{
			median = median + result_bw_mat.at<uint8_t>(i, j);
		}
	}

	median = round(median / (result_bw_mat.rows * result_bw_mat.cols));
	cout << "median = " << median << endl;

	for (int i = 0; i < result_bw_mat.cols; i++)
	{
		for (int j = 0; j < result_bw_mat.rows; j++)
		{
			if (result_bw_mat.at<uint8_t>(j, i) < median)
			{
				result_bw_mat.at<uint8_t>(j, i) = 0;
			}
			else
			{
				result_bw_mat.at<uint8_t>(j, i) = 255;
			}
		}
	}

	return result_bw_mat;
}
Mat Step_Otsu_binarization(const Mat& bw_mat)
{
	Mat result_bw_mat = bw_mat.clone();

	double* channel = new double[256];

	for (int i = 0; i < 256; i++)
	{
		channel[i] = 0;
	}

	for (int i = 0; i < result_bw_mat.cols; i++)
	{
		for (int j = 0; j < result_bw_mat.rows; j++)
		{
			channel[result_bw_mat.at<uint8_t>(j, i)]++;
		}
	}

	int N = result_bw_mat.cols * result_bw_mat.rows;

	double* p = new double[256];

	for (int i = 0; i < 256; i++)
	{
		p[i] = (double)channel[i] / (double)N;
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

	for (int i = 0; i < result_bw_mat.cols; i++)
	{
		for (int j = 0; j < result_bw_mat.rows; j++)
		{
			if (result_bw_mat.at<uint8_t>(j, i) < max_k)
			{
				result_bw_mat.at<uint8_t>(j, i) = 0;
			}
			else
			{
				result_bw_mat.at<uint8_t>(j, i) = 255;
			}
		}
	}

	return result_bw_mat;
}
Mat Global_binarization(const Mat& bw_mat)
{
	//Mat result = Step_average_binarization(result_bw_mat);
	Mat bw_mat_1 = Step_median_binarization(bw_mat);

	// Реализовать метод Отцу глобальной бинаризации
	Mat bw_mat_2 = Step_Otsu_binarization(bw_mat);
	return bw_mat_2;
}

Mat Image_extention_constant(const Mat& mat, const int& x, const int& y, const int& constant)
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
Mat Image_extension_average(const Mat& mat, const int& x, const int& y)
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
Mat Image_extension_continue_image(const Mat& mat, const int& x, const int& y)
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
Mat Image_extension_reflect_image(const Mat& mat, const int& x, const int& y)
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
Mat Image_extension_back_reflect_image(const Mat& mat, const int& x, const int& y)
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
Mat Nibleck_binarization(const Mat& mat, const int square_x, const int square_y, const double k)
{
	if ((square_x % 2 == 0) || (square_y % 2 == 0))
	{
		cout << "Bad values of window" << endl;
		exit(-1);
	}
	// Сначала необходимо "расширить" картинку одним из известных методов
	//Image_extention_constant(bw_mat_copy, square_x, square_y, 128);
	//Image_extension_average(bw_mat_copy, square_x, square_y);
	//Mat extension_image = Image_extension_continue_image(bw_mat_copy, square_x, square_y);
	//Mat extension_image = Image_extension_reflect_image(bw_mat_copy, square_x, square_y);
	Mat extension_image = Image_extension_back_reflect_image(mat, square_x, square_y);

	// Создаём матрицу со значениями порога для каждого пикселя
	double** step_values = new double* [mat.cols];
	//vector<vector<double>> step_values(bw_mat_copy.cols, vector<double>(bw_mat_copy.rows));

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
			//cout << "value = " << m + k * S << endl;

			step_values[i_original][j_original] = m + k * S;
			//cout << "value = " << step_values[i_original][j_original] << endl;
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

	Mat binary_mat = mat.clone();

	// Процесс бинаризации
	for (int i = 0; i < mat.cols; i++)
	{
		for (int j = 0; j < mat.rows; j++)
		{
			//cout << (double)bw_mat_copy.at<uint8_t>(j, i) << " " << step_values[i][j] << endl; // Работает
			if ((double)mat.at<uint8_t>(j, i) < step_values[i][j])
			{
				binary_mat.at<uint8_t>(j, i) = (uint8_t)0;
				//cout << "Yes";
			}
			else
			{
				binary_mat.at<uint8_t>(j, i) = (uint8_t)255;
				//cout << "No";
			}
		}
	}

	cout << "All completed" << endl;
	cout << "Count of anomalous values - " << count_of_anomalous_values << endl;

	//imshow("Binary cutie cat", bw_mat_copy);
	//waitKey(0);

	return binary_mat;
}
Mat Local_binarization(const Mat& bw_mat)
{
	// Реализовать метод Ниблека локальной бинаризации
	Mat binary_mat = Nibleck_binarization(bw_mat, 5, 5, 0.2);
	return binary_mat;
}

// Морфологические преобразования - эрозия и дилатация
Mat Erosion(const Mat& mat, const int& erosion_element, const int& erosion_size)
{
	int erosion_type = 0;
	if (erosion_element == 0)
	{
		erosion_type = MORPH_RECT;
	}
	else if (erosion_element == 1)
	{
		erosion_type = MORPH_CROSS;
	}
	else if (erosion_element == 2)
	{
		erosion_type = MORPH_ELLIPSE;
	}

	Mat element = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat erosion_result;

	erode(mat, erosion_result, element);
	//imshow("Before erosion", bw_mat_copy);
	//imshow("After erosion", erosion_result);
	//waitKey(0);

	return erosion_result;
}
Mat Dilatation(const Mat& mat, const int& dilatation_element, const int& dilatation_size)
{
	int dilatation_type = 0;
	if (dilatation_element == 0)
	{
		dilatation_type = MORPH_RECT;
	}
	else if (dilatation_element == 1)
	{
		dilatation_type = MORPH_CROSS;
	}
	else if (dilatation_element == 2)
	{
		dilatation_type = MORPH_ELLIPSE;
	}

	Mat element = getStructuringElement(dilatation_type, Size(2 * dilatation_size + 1, 2 * dilatation_size + 1), Point(dilatation_size, dilatation_size));
	Mat dilatation_result;

	dilate(mat, dilatation_result, element);
	//imshow("Before dilatation", bw_mat_copy);
	//imshow("After dilatation", dilatation_result);
	//waitKey(0);

	return dilatation_result;
}

void Alpha_blending(const Mat& bw_mat, const Mat& binary_mat)
{
	Mat bw_mat_copy = bw_mat.clone();
	Mat binary_mat_copy = binary_mat.clone();

	double alpha = 0.5;
	double beta;
	double input;

	cin >> input;

	if ((input >= 0) && (input <= 1))
	{
		alpha = input;
	}

	beta = 1 - alpha;

	Mat result_image;
	addWeighted(bw_mat_copy, alpha, binary_mat_copy, beta, 0.0, result_image);

	imshow("Alpha blending image", result_image);
	waitKey(0);
}

int main()
{
	Mat mat = imread("Cutie_cat.jpg");

	Mat bw_mat;
	cvtColor(mat, bw_mat, COLOR_BGR2GRAY);

	//imshow("Black-white image", bw_mat);

	//Mat binary_mat_copy = Local_binarization(bw_mat);
	Mat binary_mat = Global_binarization(bw_mat);

	cout << mat.type() << endl;
	cout << binary_mat.type() << endl;

	// Работа с CLAHE (https://answers.opencv.org/question/12024/use-of-clahe/)
	Mat CLAHE_mat = bw_mat.clone();

	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(4); // Задание первого параметра ("лимит обрезаний")
	clahe->setTilesGridSize(Size(4, 4)); // Второй параметр (размер окна)

	Mat dst;
	clahe->apply(CLAHE_mat, dst);

	imshow("CLAHE image", dst);
	waitKey(0);

	// Конец работы с CLAHE

	// Морфологические преобразования
	Mat result_image;

	// Медианный фильтр (https://techcave.ru/posts/65-filtry-v-opencv-medianblur-i-bilateral.html)
	medianBlur(binary_mat, result_image, 5);
	imshow("Median filter", result_image);
	waitKey(0);

	// Двухстороннее сглаживание
	bilateralFilter(binary_mat, result_image, 3, 6, 6);
	imshow("Bilateral filter", result_image);
	waitKey(0);

	// Эрозия и дилатация
	Mat erosion_dilatation_result;
	Mat dilatation_erosion_result;

	erosion_dilatation_result = Erosion(binary_mat, 2, 1);
	erosion_dilatation_result = Dilatation(binary_mat, 2, 1);

	dilatation_erosion_result = Dilatation(binary_mat, 2, 1);
	dilatation_erosion_result = Erosion(binary_mat, 2, 1);

	imshow("Binary bw_mat", binary_mat);
	imshow("Erosion and dilatation result", erosion_dilatation_result);
	imshow("Dilatation and erosion result", dilatation_erosion_result);
	waitKey(0);
	// Конец морфологического преобразования

	// Альфа-блендинг
	Alpha_blending(bw_mat, binary_mat);
	// Конец альфа-блендинга

	return 0;
}
