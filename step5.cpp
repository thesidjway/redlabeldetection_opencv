#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <queue>
#include <iostream>

using namespace cv;
using namespace std;
#define THRESH_MATCH 0.4

vector<Point> largestcontour(Mat img)
{
	Mat canny_output;
	vector<vector<Point>> contours; 
	vector<Vec4i> hierarchy;
	Mat bwimg;
	threshold(img,bwimg, 200, 255, THRESH_BINARY);
	imshow("bwimg",bwimg);
	
	int largest_area=0,largest_contour_index=0;
	for( int i = 0; i< contours.size(); i++ )
	{
		double a=contourArea( contours[i],false);
		if(a>largest_area)
		{
			largest_area=a;
			largest_contour_index=i;
			Rect bounding_rect=boundingRect(contours[i]);
		}
	}
	Scalar color(0,255,0);
	Canny( bwimg, canny_output, 50,50, 3 );
	findContours( canny_output, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	return contours[largest_contour_index];
}
int main()
{
Mat srcclr;
VideoCapture v(0);
while(1)
{
	v >> srcclr;
	Mat dst,dst1,dst2,dst3, srchsv,srcwhite,srcred1,srcred2,srcred;
	cvtColor(srcclr, srchsv, CV_BGR2HSV);
	inRange(srchsv,Scalar(0,70,30),Scalar(15,255,255),srcred1);
	inRange(srchsv,Scalar(165,70,30),Scalar(180,255,255),srcred2);
	bitwise_or(srcred1,srcred2,srcred);
	cvtColor(srcclr, srchsv, CV_BGR2HSV);
	inRange(srcclr,Scalar(128,128,128),Scalar(255,255,255),srcwhite);
	cv::Mat mask;
	srcwhite.copyTo(mask);
	for (int i = 0; i < mask.cols; i++) 
	{
		if (mask.at<char>(0, i) == 0) 
		{
			floodFill(mask, Point(i, 0), 255, 0, 10, 10);
		}
		if (mask.at<char>(mask.rows-1, i) == 0) 
		{
			floodFill(mask, Point(i, mask.rows-1), 255, 0, 10, 10);
		}
	}
	for (int i = 0; i < mask.rows; i++) 
	{
		if (mask.at<char>(i, 0) == 0) 
		{
			floodFill(mask, Point(0, i), 255, 0, 10, 10);
		}
		if (mask.at<char>(i, mask.cols-1) == 0) 
		{
			floodFill(mask, Point(mask.cols-1, i), 255, 0, 10, 10);
		}
	}

	Mat newImage;
	srcwhite.copyTo(newImage);
	for (int row = 0; row < mask.rows; ++row) 
	{
		for (int col = 0; col < mask.cols; ++col) 
		{
			if (mask.at<char>(row, col) == 0) 
			{
				newImage.at<char>(row, col) = 255;
			}
		}
	}
	Mat ultiimage;
	bitwise_and(newImage,srcred,ultiimage);
	vector<vector<Point>> contours; 
	vector<Vec4i> hierarchy;

	findContours( ultiimage, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	int largest_area=0,largest_contour_index=0;
	for( int i = 0; i< contours.size(); i++ )
	{
		double a=contourArea( contours[i],false);
		if(a>largest_area)
		{
			largest_area=a;
			largest_contour_index=i;
			Rect bounding_rect=boundingRect(contours[i]);
		}
	}
	
	Scalar color( 255,255,255);
	Mat image1=imread("image1lo.png",CV_LOAD_IMAGE_GRAYSCALE);
	Mat image2=imread("image2lo.png",CV_LOAD_IMAGE_GRAYSCALE);
	Mat image3=imread("image3lo.png",CV_LOAD_IMAGE_GRAYSCALE);

	dst= Mat::zeros( srcclr.rows, srcclr.rows, CV_8UC3 );
	dst1=Mat::zeros( image1.rows, image1.rows, CV_8UC3 );
	dst2=Mat::zeros( image2.rows, image2.rows, CV_8UC3 );
	dst3=Mat::zeros( image3.rows, image3.rows, CV_8UC3 );


	vector<Point> im1con=largestcontour(image1);
	vector<Point> im2con=largestcontour(image2);
	vector<Point> im3con=largestcontour(image3);

	for (auto iter: im1con)
	{
    	circle(dst1,iter,2,CV_RGB(0,255,0),-1,8,0);
    }
    for (auto iter: im2con)
	{
    	circle(dst2,iter,2,CV_RGB(0,255,0),-1,8,0);
    }
    for (auto iter: im3con)
	{
    	circle(dst3,iter,2,CV_RGB(0,255,0),-1,8,0);
	}

	imshow("dst1",dst1);
	imshow("dst2",dst2);
	imshow("dst3",dst3);

	if(contours.empty()!=1)
	{
		drawContours( dst, contours,largest_contour_index, color, CV_FILLED, 8, hierarchy );
		double image1_sim=matchShapes(contours[largest_contour_index], im1con, CV_CONTOURS_MATCH_I1, 0);
		double image2_sim=matchShapes(contours[largest_contour_index], im2con, CV_CONTOURS_MATCH_I1, 0);
		double image3_sim=matchShapes(contours[largest_contour_index], im3con, CV_CONTOURS_MATCH_I1, 0);	
		cout<<"image1_sim: "<<image1_sim<<" image2_sim: "<<image2_sim<<" image3_sim: "<<image3_sim<<endl;


		if(image1_sim<THRESH_MATCH && image1_sim<image2_sim && image1_sim<image3_sim)
		{
			int baseline=0;
	  		Size textSize = getTextSize("Image 1", FONT_HERSHEY_SCRIPT_SIMPLEX,2, 3, &baseline);
	  		Point textOrg((srcclr.cols - textSize.width)/2,(srcclr.rows + textSize.height)/2);
	  		putText(dst, "Image 1", textOrg, FONT_HERSHEY_SCRIPT_SIMPLEX, 2,Scalar::all(255), 3, 8);
		}
		else if(image2_sim<THRESH_MATCH && image2_sim<image1_sim && image2_sim<image3_sim)
		{
			int baseline=0;
	  		Size textSize = getTextSize("Image 2", FONT_HERSHEY_SCRIPT_SIMPLEX,2, 3, &baseline);
	  		Point textOrg((srcclr.cols - textSize.width)/2,(srcclr.rows + textSize.height)/2);
	  		putText(dst, "Image 2", textOrg, FONT_HERSHEY_SCRIPT_SIMPLEX, 2,Scalar::all(255), 3, 8);
		}
		else if(image3_sim<THRESH_MATCH && image3_sim<image2_sim && image3_sim<image1_sim)
		{
			int baseline=0;
	  		Size textSize = getTextSize("Image 3", FONT_HERSHEY_SCRIPT_SIMPLEX,2, 3, &baseline);
	  		Point textOrg((srcclr.cols - textSize.width)/2,(srcclr.rows + textSize.height)/2);
	  		putText(dst, "Image 3", textOrg, FONT_HERSHEY_SCRIPT_SIMPLEX, 2,Scalar::all(255), 3, 8);
		}
	}
	
	imshow("damn", dst);
	char a=waitKey(50);
	if (a == 27)
	break;

}
}

