\#\# Работа 1. Исследование гамма-коррекции

Автор: Полевой Д.В.

Исполнитель: Новицкий Д.А.

Дата: 20.04.20

Ссылка на репозиторий с кодом и картинками:

\<!\-- url: <https://github.com/xex238/Image_processing/tree/master/lab_01-->\>



\#\#\# Задание

1\. Сгенерировать серое тестовое изображение \$I\_1\$ в виде прямоугольника размером 768х60 пикселя с плавным изменение пикселей от черного к белому, одна градация серого занимает 3 пикселя по горизонтали.

2\. Применить к изображению \$I\_1\$ гамма-коррекцию с коэффициентом из интервала 2.2-2.4 и получить изображение \$G\_1\$.

3\. Сгенерировать серое тестовое изображение \$I\_2\$ в виде прямоугольника размером 768х60 пикселя со ступенчатым изменением яркости от черного к белому (от уровня 5 с шагом 10), одна градация серого занимает 30 пикселя по горизонтали.

4\. Применить к изображению \$I\_2\$ гамма-коррекцию с коэффициентом из интервала 2.2-2.4 и получить изображение \$G\_2\$.

5\. Показать визуализацию результатов в виде одного изображения, об



\#\#\# Результаты

![image-20200422015900233](C:\Users\Дмитрий\AppData\Roaming\Typora\typora-user-images\image-20200422015900233.png)

Рис. 1. Результаты работы программы (сверху вниз \$I\_1\$, \$G\_1\$, \$I\_2\$, \$G\_2\$)

!\[В репозитории записан как\](lab01.png)



\#\#\# Текст программы

```c++
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ostream>

using namespace cv;
using namespace std;

Mat Gamma_correction(Mat& image, float correction)
{
	Mat output_image = image.clone();
	for (int i = 0; i < output_image.rows; i++)
	{
		for (int j = 0; j < output_image.cols / 3; j++)
		{
			Vec3b value = output_image.at<Vec3b>(i, j);
			float x = pow(((float)value[0] / 255), correction) * 255;
			int new_x = round(x);

			//cout << "value[0] = " << value[0] << endl;
			//cout << "new_x = " << new_x << endl << endl;

			output_image.at<Vec3b>(i, j) = Vec3b(new_x, new_x, new_x);
		}
	}
	return output_image;
}

Mat Part_1(const int& height, const int& width)
{
	Mat black_white_image(height, width, CV_8UC1, Scalar(100));

	cout << "rows = " << black_white_image.rows << endl;
	cout << "columns = " << black_white_image.cols << endl;

	for (int i = 0; i < black_white_image.rows; i++)
	{
		int value = -1;
		for (int j = 0; j < black_white_image.cols / 3; j++)
		{
			black_white_image.at<Vec3b>(i, j) = Vec3b(j, j, j);
		}
	}

	float coefficient = 2.3;
	Mat gamma_image = Gamma_correction(black_white_image, coefficient);

	Mat final_image = Mat(height * 2, width, black_white_image.type());
	Rect imageROI = Rect(0, 0, black_white_image.cols, black_white_image.rows);

	imageROI.x = 0;
	imageROI.y = 0;
	black_white_image.copyTo(final_image(imageROI));

	imageROI.x = 0;
	imageROI.y = black_white_image.rows;
	gamma_image.copyTo(final_image(imageROI));

	imshow("My final image", final_image);
	waitKey(0);

	return final_image;
}
Mat Part_2(const int& height, const int& width)
{
	Mat black_white_image(height, width, CV_8UC1, Scalar(100));

	cout << "rows = " << black_white_image.rows << endl;
	cout << "columns = " << black_white_image.cols << endl;

	for (int i = 0; i < black_white_image.rows; i++)
	{
		int value = -5;
		for (int j = 0; j < black_white_image.cols; j++)
		{
			if (j % 30 == 0)
			{
				value = value + 10;
			}
			black_white_image.at<uint8_t>(i, j) = value;
		}
	}

	float coefficient = 2.3;
	Mat gamma_image = Gamma_correction(black_white_image, coefficient);

	Mat final_image = Mat(height * 2, width, black_white_image.type());
	Rect imageROI = Rect(0, 0, black_white_image.cols, black_white_image.rows);

	imageROI.x = 0;
	imageROI.y = 0;
	black_white_image.copyTo(final_image(imageROI));

	imageROI.x = 0;
	imageROI.y = black_white_image.rows;
	gamma_image.copyTo(final_image(imageROI));

	namedWindow("My final image", WINDOW_AUTOSIZE);
	imshow("My final image", final_image);
	waitKey(0);

	return final_image;
}

int main()
{
	int height = 60;
	int width = 768;

	Mat mat_1 = Part_1(height, width);
	Mat mat_2 = Part_2(height, width);

	Mat final_image = Mat(height * 4, width, CV_8UC1);
	Rect imageROI = Rect(0, 0, width, height * 2);

	imageROI.x = 0;
	imageROI.y = 0;
	mat_1.copyTo(final_image(imageROI));

	imageROI.x = 0;
	imageROI.y = height * 2;
	mat_2.copyTo(final_image(imageROI));

	imshow("Final image", final_image);
	waitKey(0);
}
```


