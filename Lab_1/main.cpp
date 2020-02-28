#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp> // Отвечает за базовый набор функций предоставляемый библиотекой OpenCV
#include <opencv2/highgui/highgui.hpp> // Содержит функции для взаимодействия с графическим движком операционной системы:
// считывание и запись графических файлов, а также взаимодействие с графическим оконным интерфейсом.

using namespace cv;
using namespace std;

void Channel_function(Mat& image)
{
	vector<Mat> spl;
	split(image, spl);

	cout << spl[0].size() << endl;
	cout << spl[1].size() << endl;
	cout << spl[2].size() << endl;

	Mat g, image_r, image_g, image_b;
	g = Mat::zeros(Size(image.cols, image.rows), CV_8UC1);

	vector<Mat> channel_r;
	channel_r.push_back(g);
	channel_r.push_back(g);
	channel_r.push_back(spl[2]);

	vector<Mat> channel_g;
	channel_g.push_back(g);
	channel_g.push_back(spl[1]);
	channel_g.push_back(g);

	vector<Mat> channel_b;
	channel_b.push_back(spl[0]);
	channel_b.push_back(g);
	channel_b.push_back(g);

	merge(channel_r, image_r);
	merge(channel_g, image_g);
	merge(channel_b, image_b);

	//imshow("spl1", spl[0]);
	//waitKey(0);
	//imshow("spl2", spl[1]);
	//waitKey(0);
	//imshow("spl3", spl[2]);
	//waitKey(0);

	// Вывод нескольких изображений на экран
	int width = 3 * image.cols;
	int height = 2 * image.rows;

	Mat ready_mat = Mat(height, width, image.type());
	Rect ImageROI = Rect(0, 0, image.cols, image.rows);

	ImageROI.x = 0;
	ImageROI.y = 0;
	image.copyTo(ready_mat(ImageROI));

	ImageROI.x = image.cols;
	ImageROI.y = 0;
	image.copyTo(ready_mat(ImageROI));

	ImageROI.x = 2 * image.cols;
	ImageROI.y = 0;
	image.copyTo(ready_mat(ImageROI));

	ImageROI.x = 0;
	ImageROI.y = image.rows;
	image_r.copyTo(ready_mat(ImageROI));

	ImageROI.x = image.cols;
	ImageROI.y = image.rows;
	image_g.copyTo(ready_mat(ImageROI));

	ImageROI.x = 2 * image.cols;
	ImageROI.y = image.rows;
	image_b.copyTo(ready_mat(ImageROI));

	imshow("My_image", ready_mat);
	waitKey(0);
}

int main()
{
	Mat image;
	//image = imread("star_sky_plain.jpg", IMREAD_COLOR);
	image = imread("Cutie_cat.jpg", IMREAD_COLOR);
	//image = imread("C://My_programs//8_semester//image_processing//Lab_1//Task_1//Task_1//Task_1//Cutie_cat.jpg", IMREAD_COLOR); // Ðàáîòàåò
	//image = imread("C:/My_programs/8_semester/image_processing/Lab_1/Task_1/Task_1/Task_1/Cutie_cat.jpg", IMREAD_COLOR); // Ðàáîòàåò
	//image = imread("cross_0256x0256.png", IMREAD_COLOR);
	//image = imread("night-sky-stars-starry-sky-1190358.jpg", IMREAD_COLOR);
	//image = imread("1339.jpg", IMREAD_COLOR);

	if (image.empty())
	{
		cout << "Cannot load image....!" << endl;
		return -1;
	}

	cout << image.size() << endl;
	//cout << image.channels() << endl;
	//cout << image.cols << endl;
	//cout << image.rows << endl;
	//cout << image.data[0] << endl;
	//cout << image.at<0>(Point(0, 0));

	Channel_function(image);
}
