#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
# include "opencv2/features2d/features2d.hpp"
# include "opencv2/nonfree/features2d.hpp"

#include <iostream>
#define dilation_size 2
using namespace cv;
using namespace std;

RNG rng(12345);
int angler(int a,int b)
{
  if(a>b)
    return b;
  else 
    return a;
}
int main(int argc, char** argv)
{
/*const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";
Mat src = imread(filename, 0);
Mat srcclr = imread(filename, CV_LOAD_IMAGE_COLOR);*/
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
 inRange(srchsv,Scalar(0,60,20),Scalar(15,255,255),srcred1);
 inRange(srchsv,Scalar(165,60,20),Scalar(180,255,255),srcred2);
 bitwise_or(srcred1,srcred2,srcred);

 inRange(srchsv,Scalar(0,0,140),Scalar(180,80,255),srcwhite);

 
  //Canny(srcred, cannyred,50 , 200, 3);
  //Canny(srcwhite, cannywhite, 50, 200, 3);

Mat element = getStructuringElement( MORPH_CROSS,Size( 2*dilation_size + 1, 2*dilation_size+1 ), Point( dilation_size, dilation_size ) );
dilate( srcwhite, cannywhite1, element );
dilate( srcred, cannyred1, element );
//1imshow("cred",cannyred1);
//imshow("cwhite",cannywhite1);
bitwise_and(cannyred1,cannywhite1,cannyfin);
//imshow("srcwhite",srcwhite);
//imshow("srcred",srcred);
medianBlur ( cannyfin, cannyfin, 1 );
vector<Vec4i> lines;
HoughLinesP(cannyfin, lines, 1, CV_PI/180, 50, 50, 3);

vector<int> anglestor;
int angle=0;
int longest=0;
int anglelongest=0,op1=0,op2=0,answ1=0;

vector<vector<Point>> contours;
vector<Vec4i> hierarchy;
vector<Point> approxCurve;

int largest_area=0;
int largest_contour_index=0;
 
findContours( cannyfin, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); 

 for( int i = 0; i< contours.size(); i++ ) 
      {
       double a=contourArea( contours[i],false); 
       if(a>largest_area){
       largest_area=a;
       largest_contour_index=i;                
       }
    }

 Scalar color( 0,255,0);
 //drawContours( srcclr, contours,largest_contour_index, color, CV_FILLED, 8, hierarchy ); 
 approxPolyDP(contours[largest_contour_index], approxCurve, arcLength(contours[largest_contour_index],1)*0.05, true);
 int it=0;
 int maxx=0,maxy=0,minx=30000,miny=30000;
if(approxCurve.size()==4 && isContourConvex(approxCurve)==1)
{
Point2f outputQuad[4],inputQuad[4];
for (auto iter: approxCurve)
{
    circle(srcclr,iter,2,CV_RGB(0,255,0),-1,8,0);
    if(iter.x>maxx)
    maxx=iter.x;
   if(iter.y>maxy)
   maxy=iter.y;
   if(iter.x<minx)
   minx=iter.x;
    if(iter.y<miny)
    miny=iter.y;
    inputQuad[it]=iter;
    it++;
}
Mat pers;
Mat check=imread("square.jpg",CV_LOAD_IMAGE_GRAYSCALE);
/*outputQuad[3]=Point(minx,miny);
outputQuad[2]=Point(minx,maxy);
outputQuad[0]=Point(maxx,miny);
outputQuad[1]=Point(maxx,maxy);
Rect myROI(((float)minx*0.9+(float)maxx*0.1),((float)miny*0.9+(float)maxy*0.1),(maxx-minx)*0.8,(maxy-miny)*0.8);
*/

outputQuad[3]=Point(0,0);
outputQuad[2]=Point(0,check.cols);
outputQuad[0]=Point(check.rows,0);
outputQuad[1]=Point(check.cols,check.cols);
Rect myROI(0,0,check.rows,check.cols);

Mat lambda( 2, 4, CV_32FC1 );
lambda = Mat::zeros( srcclr.rows, srcclr.cols, srcclr.type() );
lambda = getPerspectiveTransform(inputQuad, outputQuad);
warpPerspective(srcclr,pers,lambda,srcclr.size() );

if(pers.rows>=check.rows && pers.cols>=check.cols)
{
Mat result;
Mat croppedImage = pers(myROI);
flip(croppedImage,croppedImage,1);
cvtColor(croppedImage, croppedImage, CV_BGR2GRAY);
result.create( 1,1, CV_32FC1 );
matchTemplate( croppedImage, check, result, CV_TM_CCORR);
//cout<<"result: "<<result<<endl;
imshow("cropped",croppedImage);
}
}
else()
{
  
}

 char a=waitKey(100);
 if (a == 27)
      break;
}
 return 0;
}
