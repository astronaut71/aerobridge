#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <map>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "hough.h"

//extern FILE *stdin;
//extern FILE *stdout;
//extern FILE *stderr;

std::string img_path;
int threshold = 0;

const char* CW_IMG_ORIGINAL 	= "Result";
const char* CW_IMG_EDGE		=    "Canny Edge Detection";
const char* CW_ACCUMULATOR  	= "Accumulator";

void doTransform(std::string, int threshold);




int main(int argc, char** argv) {

	int c;


    cv::namedWindow(CW_IMG_ORIGINAL, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(CW_IMG_EDGE, 	 cv::WINDOW_AUTOSIZE);
    cv::namedWindow(CW_ACCUMULATOR,	 cv::WINDOW_AUTOSIZE);

    cvMoveWindow(CW_IMG_ORIGINAL, 10, 10);
    cvMoveWindow(CW_IMG_EDGE, 680, 10);
    cvMoveWindow(CW_ACCUMULATOR, 1350, 10);

    doTransform(img_path, threshold);

	return 0;
}



void doTransform(std::string file_path, int threshold)
{
	cv::Mat img_edge;
	cv::Mat img_blur;

	cv::Mat gray;
	cv::Mat dil;
	cv::Mat erd;

	cv::Mat img_ori = cv::imread("window_rect1.jpg");
	//cv::blur( img_ori, img_blur, cv::Size(5,5) );
	cv::GaussianBlur(img_ori, img_blur, cv::Size(9, 9), 2, 2);
	//cv::Canny(img_blur, img_edge, 1200, 50, 5,true);
	cv::Canny(img_blur, img_edge, 1100, 20, 5,true);





	int w = img_edge.cols;
	int h = img_edge.rows;

	//Transform
	keymolen::Hough hough;
	hough.Transform(img_edge.data, w, h);


      // if(threshold == 0)
		//threshold = w>h?w/4:h/4;
		threshold = 150;

	while(1)
	{
		cv::Mat img_res = img_ori.clone();

		//Search the accumulator
		std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > lines = hough.GetLines(threshold);

		//Draw the results
		std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > >::iterator it;
		for(it=lines.begin();it!=lines.end();it++)
		{
			cv::line(img_res, cv::Point(it->first.first, it->first.second), cv::Point(it->second.first, it->second.second), cv::Scalar( 0, 0, 255), 2, 8);
		}

		//Visualize all
		int aw, ah, maxa;
		aw = ah = maxa = 0;
		const unsigned int* accu = hough.GetAccu(&aw, &ah);

		for(int p=0;p<(ah*aw);p++)
		{
			if((int)accu[p] > maxa)
				maxa = accu[p];
		}
		double contrast = 1.0;
		double coef = 255.0 / (double)maxa * contrast;

		cv::Mat img_accu(ah, aw, CV_8UC3);
		for(int p=0;p<(ah*aw);p++)
		{
			unsigned char c = (double)accu[p] * coef < 255.0 ? (double)accu[p] * coef : 255.0;
			img_accu.data[(p*3)+0] = 255;
			img_accu.data[(p*3)+1] = 255-c;
			img_accu.data[(p*3)+2] = 255-c;
		}


		cv::imshow(CW_IMG_ORIGINAL, img_res);
		cv::imshow(CW_IMG_EDGE, img_edge);
		cv::imshow(CW_ACCUMULATOR, img_accu);



		char c = cv::waitKey(0);
		if(c == '+')
			threshold += 5;
		if(c == '-')
			threshold -= 5;
		if(c == 27)
			break;
	}
}
