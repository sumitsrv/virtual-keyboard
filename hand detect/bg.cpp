#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include <ctype.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
IplImage *frame, *in, *out, *greyImage; 
CvCapture *input; 
bool first=true; 
input = cvCaptureFromCAM(0); 
CvSize imgSize; 
int iMaxx, iMinx, iMaxy, iMiny, i, key; 
CvGaussBGStatModelParams* params = new CvGaussBGStatModelParams; 
CvBGStatModel* bg_model; 
CvMemStorage *storage = cvCreateMemStorage(0); 
CvSeq *contour = 0; 
IplImage *contourImg; 

if (input==NULL) 
        { 
                frame=0; 
        } else 
        { 
                frame = cvQueryFrame(input); 
                imgSize.width = frame->width; 
                imgSize.height = frame->height; 
                cvNamedWindow("Video", CV_WINDOW_AUTOSIZE); 
                cvNamedWindow("Result", CV_WINDOW_AUTOSIZE); 
                in = cvCreateImage( cvSize(frame->width/2,frame->height/2), IPL_DEPTH_8U,1 ); 
                out = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U,1 ); 
                greyImage=cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U,1); 
bg_model = cvCreateGaussianBGModel( frame ); 

} 
        while (key!=27) 
        { 
                ////BACKGROUND EXTRACTION///////// 
                        

                        cvUpdateBGStatModel( frame, bg_model ); 
                        greyImage=cvCloneImage(bg_model->foreground); 
                        cvDilate(greyImage, greyImage, NULL, 4); 
                        cvErode(greyImage, greyImage, NULL, 2); 
                        
                        cvPyrDown (greyImage, in, CV_GAUSSIAN_5x5); 

                        cvSmooth(in, in, CV_BLUR); 
                        cvPyrUp (in, out, CV_GAUSSIAN_5x5); 
                        cvShowImage("Result", out); 
                
                cvShowImage("Video", frame); 
                frame = cvQueryFrame(input); 
                key = cvWaitKey(30); 
                
        } 
return 0;
}
