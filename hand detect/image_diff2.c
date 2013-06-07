#include <cv.h>
#include <highgui.h>

#define tr 15

int main(int argc, char *argv[])
{
	CvCapture *capture;
    // initialize camera
    capture = cvCaptureFromCAM( 0 );
    assert( capture );
    IplImage* image;
IplImage* lastImage;
IplImage* diffImage;
IplImage* bitImage;

for(;;)
{
IplImage* frame = 0;
int c;

frame = cvQueryFrame( capture );
if( !frame )
break;
//If is the first frame
if(!image){
//Create image header same as frame but with 1 channel to gray
image=cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
bitImage=cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
}
//Convert frame to gray and store in image
cvCvtColor(frame, image,CV_BGR2GRAY);

//If is the first frame
if(!lastImage){

//If no lastImage clone actual image;
lastImage=cvCloneImage(image);
}
if(!diffImage){
//Create image header same as frame but with 1 channel to gray
diffImage=cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
}

cvShowImage("CamSub", frame );
//Differences with actual and last image
cvAbsDiff(image,lastImage,diffImage);
//threshold image
cvThreshold(diffImage,bitImage,tr,255,CV_THRESH_BINARY);
//Change datas;
lastImage=cvCloneImage(image);

cvShowImage("CamSub 1",bitImage);

c = cvWaitKey(10);
if( (char) c == 27 )
break;
}

return 0;
}
