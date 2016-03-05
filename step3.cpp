#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"
#include <fstream>

#include <iostream>
#define dilation_size 2
#define dilation_size_w 1
#define NORM_THRES 0.32
using namespace cv;
using namespace std;

RNG rng(12345);

double getSimilarity( const Mat A, const Mat B ) {
if ( A.rows > 0 && A.rows == B.rows && A.cols > 0 && A.cols == B.cols ) {
    Mat C,D;

    /*threshold(A, C, 128, 255,THRESH_BINARY);
    threshold(B, D, 128, 255, THRESH_BINARY);
        imshow("C",C);
        imshow("D",D);*/
    double errorL2 = norm(A,B, CV_L2 );
    double similarity = errorL2 / (double)( A.rows * A.cols );
    return similarity;
}
}


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
 inRange(srchsv,Scalar(0,40,30),Scalar(15,255,255),srcred1);
 inRange(srchsv,Scalar(165,40,30),Scalar(180,255,255),srcred2);
 bitwise_or(srcred1,srcred2,srcred);

 inRange(srchsv,Scalar(0,0,150),Scalar(180,60,255),srcwhite);

 
//Canny(srcred, cannyred,50 , 200, 3);
//Canny(srcwhite, cannywhite, 50, 200, 3);

Mat element = getStructuringElement( MORPH_CROSS,Size( 2*dilation_size + 1, 2*dilation_size+1 ), Point( dilation_size, dilation_size ) );
Mat element1 = getStructuringElement( MORPH_CROSS,Size( 2*dilation_size_w + 1, 2*dilation_size_w+1 ), Point( dilation_size_w, dilation_size_w ) );
dilate( srcwhite, cannywhite1, element1 );
dilate( srcred, cannyred1, element );
imshow("cred",cannyred1);
imshow("cwhite",cannywhite1);
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
Mat pers,check1,check2,check3;
Mat checker1,checker2,checker3,checkest1,checkest2,checkest3;
Mat check=imread("image1.png",CV_LOAD_IMAGE_GRAYSCALE);
Mat checker=imread("image2.png",CV_LOAD_IMAGE_GRAYSCALE);
Mat checkest=imread("image3.png",CV_LOAD_IMAGE_GRAYSCALE);

outputQuad[3]=Point(0,0);
outputQuad[2]=Point(0,check.cols);
outputQuad[0]=Point(check.rows,0);
outputQuad[1]=Point(check.cols,check.cols);
Rect myROI(0,0,check.rows,check.cols);

Mat lambda( 2, 4, CV_32FC1 );
lambda = Mat::zeros( srcclr.rows, srcclr.cols, srcclr.type() );
lambda = getPerspectiveTransform(inputQuad, outputQuad);
warpPerspective(srcclr,pers,lambda,srcclr.size() );

transpose(check,check1);
flip(check1,check1,1);
transpose(check1,check2);
flip(check2,check2,1);
transpose(check2,check3);
flip(check3,check3,1);

transpose(checker,checker1);
flip(checker1,checker1,1);
transpose(checker1,checker2);
flip(checker2,checker2,1);
transpose(checker2,checker3);
flip(checker3,checker3,1);

transpose(checkest,checkest1);
flip(checkest1,checkest1,1);
transpose(checkest1,checkest2);
flip(checkest2,checkest2,1);
transpose(checkest2,checkest3);
flip(checkest3,checkest3,1);

if(pers.rows>=check.rows && pers.cols>=check.cols)
{
Mat result;
Mat croppedImage = pers(myROI);
flip(croppedImage,croppedImage,1);
cvtColor(croppedImage, croppedImage, CV_BGR2GRAY);

double corr=getSimilarity(croppedImage,check);
double corr1=getSimilarity(croppedImage,check1);
double corr2=getSimilarity(croppedImage,check2);
double corr3=getSimilarity(croppedImage,check3);

cout<<"Corr: "<<corr<<" "<<corr1<<" "<<corr2<<" "<<corr3<<endl;
double correr=getSimilarity(croppedImage,checker);
double correr1=getSimilarity(croppedImage,checker1);
double correr2=getSimilarity(croppedImage,checker2);
double correr3=getSimilarity(croppedImage,checker3);

cout<<"Correr: "<<correr<<" "<<correr1<<" "<<correr2<<" "<<correr3<<endl;
double correst=getSimilarity(croppedImage,checkest);
double correst1=getSimilarity(croppedImage,checkest1);
double correst2=getSimilarity(croppedImage,checkest2);
double correst3=getSimilarity(croppedImage,checkest3);

cout<<"Correst: "<<correst<<" "<<correst1<<" "<<correst2<<" "<<correst3<<endl;
if(croppedImage.rows>0 && croppedImage.cols>0 && corr<=NORM_THRES||corr1<=NORM_THRES||corr2<=NORM_THRES||corr3<=NORM_THRES)
{
  int baseline=0;
  Size textSize = getTextSize("Image 1", FONT_HERSHEY_SCRIPT_SIMPLEX,
                            2, 3, &baseline);
  Point textOrg((srcclr.cols - textSize.width)/2,(srcclr.rows + textSize.height)/2);
  putText(srcclr, "Image 1", textOrg, FONT_HERSHEY_SCRIPT_SIMPLEX, 2,
      
        Scalar::all(255), 3, 8);
}
else if(croppedImage.rows>0 && croppedImage.cols>0 && correr<=NORM_THRES||correr1<=NORM_THRES||correr2<=NORM_THRES||correr3<=NORM_THRES)
{
  int baseline=0;
  Size textSize = getTextSize("Image 2", FONT_HERSHEY_SCRIPT_SIMPLEX,
                            2, 3, &baseline);
  Point textOrg((srcclr.cols - textSize.width)/2,(srcclr.rows + textSize.height)/2);
  putText(srcclr, "Image 2", textOrg, FONT_HERSHEY_SCRIPT_SIMPLEX, 2,
      
        Scalar::all(255), 3, 8);
}
else if(croppedImage.rows>0 && croppedImage.cols>0 && correst<=NORM_THRES||correst1<=NORM_THRES||correst2<=NORM_THRES||correst3<=NORM_THRES)
{
  int baseline=0;
  Size textSize = getTextSize("Image 3", FONT_HERSHEY_SCRIPT_SIMPLEX,
                            2, 3, &baseline);
  Point textOrg((srcclr.cols - textSize.width)/2,(srcclr.rows + textSize.height)/2);
  putText(srcclr, "Image 3", textOrg, FONT_HERSHEY_SCRIPT_SIMPLEX, 2,
      
        Scalar::all(255), 3, 8);
}
//cout<<"result: "<<result<<endl;
imshow("cropped",croppedImage);
}
else
{


}
}
imshow("orig",srcclr);
 char a=waitKey(100);
 if (a == 27)
      break;
}
 return 0;
}
