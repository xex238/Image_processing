## Работа 2. Визуализация искажений jpeg-сжатия
автор: Полевой Д.В.

исполнитель: Новицкий Д.А.

дата: 22.04.20


### Задание
Для исходного изображения (сохраненного без потерь) создать jpeg версии с двумя уровнями качества (например, 95 и 65). Вычислить и визуализировать на одной “мозаике” исходное изображение, результаты сжатия, поканальные и яркостные различия.

### Результаты

![](https://github.com/xex238/Image_processing/blob/master/lab_02/Cutie_cat.jpg)

Рис 1. Исходное изображение

![](https://github.com/xex238/Image_processing/blob/master/lab_02/Cutie_cat_90.jpg)

Рис 2. Исходное изображение с уровнем качества 90.

![](https://github.com/xex238/Image_processing/blob/master/lab_02/Cutie_cat_60.jpg)

Рис 3. Исходное изображение с уровнем качества 60.

![](https://github.com/xex238/Image_processing/blob/master/lab_02/Black-white_channel_differents.jpg)

Рис 4. Поканальные различия чёрно-белого изображения с уровнями качества 100, 90, 60 и 10 соответственно.

![](https://github.com/xex238/Image_processing/blob/master/lab_02/RGB_channel_differents.jpg)

Рис 5. Поканальные различия цветного изображения с уровнями качества 100 и 10 соответственно.

### Текст программы

```c++
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat Draw_gistograms_vertical(Mat& rotated_gistogram_R, Mat& rotated_gistogram_G, Mat& rotated_gistogram_B)
{
	// Соединяем гистограммы в общую картинку
	int height = 3 * rotated_gistogram_R.rows;
	double width = rotated_gistogram_R.cols;

	Mat final_mat = Mat(height, width, rotated_gistogram_R.type());
	Rect image_ROI = Rect(0, 0, rotated_gistogram_R.cols, rotated_gistogram_R.rows);

	image_ROI.x = 0;
	image_ROI.y = 0;
	rotated_gistogram_R.copyTo(final_mat(image_ROI));

	image_ROI.x = 0;
	image_ROI.y = rotated_gistogram_R.rows;
	rotated_gistogram_G.copyTo(final_mat(image_ROI));

	image_ROI.x = 0;
	image_ROI.y = 2 * rotated_gistogram_R.rows;
	rotated_gistogram_B.copyTo(final_mat(image_ROI));

	// Выводим гистограммы на экран
	//imshow("My_gistograms", final_mat);
	//waitKey(0);

	return final_mat;
}
Mat Draw_gistograms_horizontal(Mat& rotated_gistogram_R, Mat& rotated_gistogram_G, Mat& rotated_gistogram_B)
{
	// Соединяем гистограммы в общую картинку
	int height = rotated_gistogram_R.rows;
	double width = 3 * rotated_gistogram_R.cols + 10;

	Mat line(height, 5, rotated_gistogram_B.type(), Scalar(128));

	Mat final_mat = Mat(height, width, rotated_gistogram_R.type());
	Rect image_ROI = Rect(0, 0, rotated_gistogram_R.cols, rotated_gistogram_R.rows);
	Rect line_ROI = Rect(0, 0, line.cols, line.rows);

	image_ROI.x = 0;
	image_ROI.y = 0;
	rotated_gistogram_R.copyTo(final_mat(image_ROI));

	image_ROI.x = rotated_gistogram_R.cols;
	image_ROI.y = 0;
	line.copyTo(final_mat(line_ROI));

	image_ROI.x = rotated_gistogram_R.cols + 5;
	image_ROI.y = 0;
	rotated_gistogram_G.copyTo(final_mat(image_ROI));

	image_ROI.x = 2 * rotated_gistogram_R.cols + 5;
	image_ROI.y = 0;
	line.copyTo(final_mat(line_ROI));

	image_ROI.x = 2 * rotated_gistogram_R.cols + 10;
	image_ROI.y = 0;
	rotated_gistogram_B.copyTo(final_mat(image_ROI));

	// Выводим гистограммы на экран
	//imshow("My_gistograms", final_mat);
	//waitKey(0);

	return final_mat;
}

Mat General_gistogram(Mat& mat)
{
	// Вычисляем общую гистограмму яркости
	double* channel = new double[256];

	for (int i = 0; i < 256; i++)
	{
		channel[i] = 0;
	}

	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			channel[mat.at<Vec3b>(i, j)[0]]++;
			channel[mat.at<Vec3b>(i, j)[1]]++;
			channel[mat.at<Vec3b>(i, j)[2]]++;
		}
	}

	// Ищем максимум и минимум
	double min = channel[0];
	double max = channel[0];

	for (int i = 0; i < 256; i++)
	{
		if (channel[i] > max)
		{
			max = channel[i];
		}
		if (channel[i] < min)
		{
			min = channel[i];
		}
	}

	// Нормируем значения
	if (max != min)
	{
		for (int i = 0; i < 256; i++)
		{
			channel[i] = round(((channel[i] - min) / (max - min)) * 255);
		}
	}

	// Заполняем гистограмму
	int height = 256;
	int width = 256;

	Mat gistogram(height, width, CV_8UC1, Scalar(0));

	for (int i = 0; i < gistogram.cols; i++)
	{
		for (int j = 0; j < channel[i]; j++)
		{
			gistogram.at<uchar>(j, i) = 255;
		}
	}

	// Поворачиваем гисторгамму
	Mat rotated_gistogram;

	Point2f pc(gistogram.cols / 2., gistogram.rows / 2.);
	Mat r = getRotationMatrix2D(pc, -180, 1.0);

	warpAffine(gistogram, rotated_gistogram, r, gistogram.size());

	// Выводим гистограмму на экран
	//imshow("My_gistogram", rotated_gistogram);
	//waitKey(0);

	return rotated_gistogram;
}
Mat Channel_gistogram(Mat& mat)
{
	// Вычисляем поканальную гистограмму яркости
	double* R = new double[256];
	double* G = new double[256];
	double* B = new double[256];

	for (int i = 0; i < 256; i++)
	{
		R[i] = 0;
		G[i] = 0;
		B[i] = 0;
	}

	for (int i = 0; i < mat.cols / 3; i++)
	{
		for (int j = 0; j < mat.rows; j++)
		{
			R[mat.at<Vec3b>(i, j)[0]]++;
			G[mat.at<Vec3b>(i, j)[1]]++;
			B[mat.at<Vec3b>(i, j)[2]]++;
		}
	}

	// Ищем максимум и минимум
	double max_R = R[0];
	double min_R = R[0];
	double max_G = G[0];
	double min_G = G[0];
	double max_B = B[0];
	double min_B = B[0];

	for (int i = 0; i < 256; i++)
	{
		if (R[i] > max_R)
		{
			max_R = R[i];
		}
		if (R[i] < min_R)
		{
			min_R = R[i];
		}
		if (G[i] > max_G)
		{
			max_G = G[i];
		}
		if (G[i] < min_G)
		{
			min_G = G[i];
		}
		if (B[i] > max_B)
		{
			max_B = B[i];
		}
		if (B[i] < min_B)
		{
			min_B = B[i];
		}
	}

	// Нормируем значения
	if (max_R != min_R)
	{
		for (int i = 0; i < 256; i++)
		{
			R[i] = round(((R[i] - min_R) / (max_R - min_R)) * 255);
		}
	}
	if (max_G != min_G)
	{
		for (int i = 0; i < 256; i++)
		{
			G[i] = round(((G[i] - min_G) / (max_G - min_G)) * 255);
		}
	}
	if (max_B != min_B)
	{
		for (int i = 0; i < 256; i++)
		{
			B[i] = round(((B[i] - min_B) / (max_B - min_B)) * 255);
		}
	}

	// Заполняем гистограмму
	int height = 256;
	int width = 256;

	Mat gistogram_R(height, width, CV_8UC1, Scalar(0));
	Mat gistogram_G(height, width, CV_8UC1, Scalar(0));
	Mat gistogram_B(height, width, CV_8UC1, Scalar(0));

	for (int i = 0; i < gistogram_R.cols; i++)
	{
		for (int j = 0; j < R[i]; j++)
		{
			gistogram_R.at<uchar>(j, i) = 255;
		}
	}
	for (int i = 0; i < gistogram_G.cols; i++)
	{
		for (int j = 0; j < G[i]; j++)
		{
			gistogram_G.at<uchar>(j, i) = 255;
		}
	}
	for (int i = 0; i < gistogram_B.cols; i++)
	{
		for (int j = 0; j < B[i]; j++)
		{
			gistogram_B.at<uchar>(j, i) = 255;
		}
	}

	// Поворачиваем гисторгамму
	Mat rotated_gistogram_R;
	Mat rotated_gistogram_G;
	Mat rotated_gistogram_B;

	Point2f pc(gistogram_R.cols / 2., gistogram_R.rows / 2.);
	Mat r = getRotationMatrix2D(pc, -180, 1.0);

	warpAffine(gistogram_R, rotated_gistogram_R, r, gistogram_R.size());
	warpAffine(gistogram_G, rotated_gistogram_G, r, gistogram_G.size());
	warpAffine(gistogram_B, rotated_gistogram_B, r, gistogram_B.size());

	// Соединяем гистограммы в общую картинку
	//Draw_gistograms_vertical(rotated_gistogram_R, rotated_gistogram_G, rotated_gistogram_B);
	Mat result_mat = Draw_gistograms_horizontal(rotated_gistogram_R, rotated_gistogram_G, rotated_gistogram_B);
	return result_mat;
}
void Channel_gistogram_mas(Mat& mat)
{
	double** RGB_channels = new double* [3];

	for (int i = 0; i < 3; i++)
	{
		RGB_channels[i] = new double[256];
		for (int j = 0; j < 256; j++)
		{
			RGB_channels[i][j] = 0;
		}
	}

	for (int i = 0; i < mat.cols / 3; i++)
	{
		for (int j = 0; j < mat.rows; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				RGB_channels[mat.at<Vec3b>(i, j)[k]]++;
			}
		}
	}

	// Ищем максимум и минимум
	double* max = new double[3]{ RGB_channels[0][0], RGB_channels[1][0], RGB_channels[2][0] };
	double* min = new double[3]{ RGB_channels[0][0], RGB_channels[1][0], RGB_channels[2][0] };

	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (RGB_channels[j][i] > max[j])
			{
				max[j] = RGB_channels[j][i];
			}
			if (RGB_channels[j][i] < min[j])
			{
				min[j] = RGB_channels[j][i];
			}
		}
	}

	// Нормируем значения
	for (int i = 0; i < 3; i++)
	{
		if (max[i] != min[i])
		{
			for (int j = 0; j < 256; j++)
			{
				RGB_channels[i][j] = round(((RGB_channels[i][j] - min[i]) / (max[i] - min[i])) * 255);
			}
		}
	}

	// Заполняем гистограмму
	int height = 256;
	int width = 256;

	Mat* gistograms = new Mat[3];

	for (int i = 0; i < 3; i++)
	{
		gistograms[i] = Mat(height, width, CV_8UC1, Scalar(0));
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < gistograms[i].cols; j++)
		{
			for (int k = 0; k < RGB_channels[i][j]; k++)
			{
				gistograms[i].at<uchar>(j, i) = 255;
			}
		}
	}

	// Поворачиваем гисторгамму
	Mat* rotated_gistograms = new Mat[3];

	Point2f pc(gistograms[0].cols / 2., gistograms[0].rows / 2.);
	Mat r = getRotationMatrix2D(pc, -180, 1.0);

	for (int i = 0; i < 3; i++)
	{
		warpAffine(gistograms[i], rotated_gistograms[i], r, gistograms[i].size());
	}

	// Соединяем гистограммы в общую картинку
	height = 3 * gistograms[0].rows;
	width = gistograms[0].cols;

	Mat final_mat = Mat(height, width, rotated_gistograms[0].type());
	Rect image_ROI = Rect(0, 0, rotated_gistograms[0].cols, rotated_gistograms[0].rows);

	image_ROI.x = 0;
	image_ROI.y = 0;
	rotated_gistograms[0].copyTo(final_mat(image_ROI));

	image_ROI.x = 0;
	image_ROI.y = rotated_gistograms[1].rows;
	rotated_gistograms[1].copyTo(final_mat(image_ROI));

	image_ROI.x = 0;
	image_ROI.y = 2 * rotated_gistograms[2].rows;
	rotated_gistograms[2].copyTo(final_mat(image_ROI));

	// Выводим гистограммы на экран
	imshow("My_gistograms", final_mat);
	waitKey(0);
}

void Create_gistogram_RGB(Mat& mat, Mat& mat_90, Mat& mat_60, Mat& mat_10)
{
	Mat result_channel_mat = General_gistogram(mat);
	Mat result_channel_mat_90 = General_gistogram(mat_90);
	Mat result_channel_mat_60 = General_gistogram(mat_60);
	Mat result_channel_mat_10 = General_gistogram(mat_10);

	int height = result_channel_mat.rows;
	double width = 4 * result_channel_mat.cols + 15;

	Mat line(height, 5, result_channel_mat.type(), Scalar(128));

	Mat final_mat = Mat(height, width, result_channel_mat.type());
	Rect image_ROI = Rect(0, 0, result_channel_mat.cols, result_channel_mat.rows);
	Rect line_ROI = Rect(0, 0, line.cols, line.rows);

	image_ROI.x = 0;
	image_ROI.y = 0;
	result_channel_mat.copyTo(final_mat(image_ROI));

	image_ROI.x = result_channel_mat.cols;
	image_ROI.y = 0;
	line.copyTo(final_mat(line_ROI));

	image_ROI.x = result_channel_mat.cols + 5;
	image_ROI.y = 0;
	result_channel_mat_90.copyTo(final_mat(image_ROI));

	image_ROI.x = 2 * result_channel_mat.cols + 5;
	image_ROI.y = 0;
	line.copyTo(final_mat(line_ROI));

	image_ROI.x = 2 * result_channel_mat.cols + 10;
	image_ROI.y = 0;
	result_channel_mat_60.copyTo(final_mat(image_ROI));

	image_ROI.x = 3 * result_channel_mat.cols + 10;
	image_ROI.y = 0;
	line.copyTo(final_mat(line_ROI));

	image_ROI.x = 3 * result_channel_mat.cols + 15;
	image_ROI.y = 0;
	result_channel_mat_10.copyTo(final_mat(image_ROI));

	imwrite("Black-white_channel_differents.jpg", final_mat);

	imshow("Black-white channel differents (100%, 90%, 60%, 10% quality)", final_mat);
	waitKey(0);

	Mat result_general_mat = Channel_gistogram(mat);
	Mat result_general_mat_90 = Channel_gistogram(mat_90);
	Mat result_general_mat_60 = Channel_gistogram(mat_60);
	Mat result_general_mat_10 = Channel_gistogram(mat_10);
	//Channel_gistogram_mas(mat); // Не работает

	height = 2 * result_general_mat_90.rows;
	width = result_general_mat_90.cols;

	final_mat = Mat(height, width, result_general_mat.type());
	image_ROI = Rect(0, 0, result_general_mat.cols, result_general_mat.rows);

	image_ROI.x = 0;
	image_ROI.y = 0;
	result_general_mat.copyTo(final_mat(image_ROI));

	image_ROI.x = 0;
	image_ROI.y = result_general_mat.rows;
	result_general_mat_10.copyTo(final_mat(image_ROI));

	imwrite("RGB_channel_differents.jpg", final_mat);

	// Выводим гистограммы на экран
	imshow("RGB channels differents (100%, 10% quality)", final_mat);
	waitKey(0);
}
void Create_gistogram_BW(Mat& mat)
{
	double* channel = new double[256];

	for (int i = 0; i < 256; i++)
	{
		channel[i] = 0;
	}

	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			channel[mat.at<uint8_t>(i, j)]++;
		}
	}

	cout << "channel[i] = " << endl;
	for (int i = 0; i < 256; i++)
	{
		cout << channel[i] << endl;
	}

	// Ищем максимум и минимум
	double min = channel[0];
	double max = channel[0];

	for (int i = 0; i < 256; i++)
	{
		if (channel[i] > max)
		{
			max = channel[i];
		}
		if (channel[i] < min)
		{
			min = channel[i];
		}
	}

	// Нормируем значения
	if (max != min)
	{
		for (int i = 0; i < 256; i++)
		{
			channel[i] = round(((channel[i] - min) / (max - min)) * 255);
		}
	}

	// Создаём и заполняем гистограмму значениями
	int height = 256;
	int width = 256;

	Mat gistogram(height, width, CV_8UC1, Scalar(0));

	for (int i = 0; i < gistogram.cols; i++)
	{
		for (int j = 0; j < channel[i]; j++)
		{
			gistogram.at<uchar>(j, i) = 255;
		}
	}

	// Поворачиваем гистограмму
	Mat rotated_gistogram;

	Point2f pc(gistogram.cols / 2., gistogram.rows / 2.);
	Mat r = getRotationMatrix2D(pc, -180, 1.0);

	warpAffine(gistogram, rotated_gistogram, r, gistogram.size());

	// Выводим гистограмму на экран
	imshow("My_gistogram", rotated_gistogram);
	waitKey(0);
}

int main()
{
	Mat mat = imread("Cutie_cat.jpg");
	//Mat mat = imread("Sasha_photo_new.jpg");

	Mat bw_mat;
	cvtColor(mat, bw_mat, COLOR_BGR2GRAY);

	//imshow("Black_white_image", bw_mat);
	//waitKey(0);

	vector<int> compression_params;
	compression_params.push_back(IMWRITE_JPEG_QUALITY);
	compression_params.push_back(90);

	try
	{
		imwrite("Cutie_cat_90.jpg", mat, compression_params);

		compression_params.pop_back();
		compression_params.push_back(60);
		imwrite("Cutie_cat_60.jpg", mat, compression_params);

		compression_params.pop_back();
		compression_params.push_back(10);
		imwrite("Cutie_cat_10.jpg", mat, compression_params);
	}
	catch(const cv::Exception& ex)
	{
		cout << "Exception converting image to PNG format : " << endl;
	}

	Mat mat_90 = imread("Cutie_cat_90.jpg");
	Mat mat_60 = imread("Cutie_cat_60.jpg");
	Mat mat_10 = imread("Cutie_cat_10.jpg");

	imshow("Black_white_image", bw_mat);
	//waitKey(0);

	Create_gistogram_RGB(mat, mat_90, mat_60, mat_10);
	Create_gistogram_BW(bw_mat);
}
```
