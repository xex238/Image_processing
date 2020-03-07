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

int main()
{
	int height = 60;
	int width = 768;

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
	return 0;
}