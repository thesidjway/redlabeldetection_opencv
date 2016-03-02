#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#define dilation_size 1
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
/*const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";
Mat src = imread(filename, 0);
Mat srcclr = imread(filename, CV_LOAD_IMAGE_COLOR);*/

struct myclass {
  bool operator() (int i,int j) { return (i<j);}
} myobject;

Mat srcclr;
VideoCapture v(0);

 /*if(src.empty())
 {
     cout << "can not open " << filename << endl;
     return -1;
 }*/
while(1)
{
 v >> srcclr;
 Mat dst, cdst,srchsv,srcred,srcred1,srcred2,srcwhite,cannyred,cannywhite,cannyfin,cannywhite1,cannyred1;
 cvtColor(srcclr, srchsv, CV_BGR2HSV);
 inRange(srchsv,Scalar(0,90,75),Scalar(15,255,255),srcred1);
 inRange(srchsv,Scalar(165,90,75),Scalar(180,255,255),srcred2);
 bitwise_or(srcred1,srcred2,srcred);

 inRange(srcclr,Scalar(128,128,128),Scalar(255,255,255),srcwhite);

  Canny(srcred, cannyred,50 , 200, 3);
  Canny(srcwhite, cannywhite, 50, 200, 3);

  Mat element = getStructuringElement( MORPH_CROSS,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
  /// Apply the dilation operation
 //dilate( cannywhite, cannywhite1, element );
  dilate( cannyred, cannyfin, element );
  //bitwise_and(cannyred1,cannywhite1,cannyfin);
 imshow("srcwhite",srcwhite);
 imshow("srcred",srcred);
medianBlur ( cannyfin, cannyfin, 1 );
 vector<Vec4i> lines;
  HoughLinesP(cannyfin, lines, 1, CV_PI/180, 50, 50, 3);
 vector<Point> pointstor;
 int angle;
 for( size_t i = 0; i < lines.size(); i++ )
 {
    Vec4i l = lines[i];
    line( srcclr, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0), 3, CV_AA);
    pointstor.push_back( Point(l[0], l[1]));
    pointstor.push_back( Point(l[2], l[3]));
 }
 if(pointstor.size()>0) {
 Rect daa=boundingRect(pointstor);
 imshow("srcbad",srcclr(daa));
}
imshow("srcclr",srcclr);
//imshow("srccanny",cannyfin);

// imshow("redwhite",cannyfin);

 char a=waitKey(33);
 if (a == 27)
      break;
}
 return 0;
}
