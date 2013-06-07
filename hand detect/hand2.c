 #include "cv.h" 
 #include "cxcore.h" 
 #include "highgui.h" 
 #include "math.h" 
 #include <iostream> 
 #include <stdio.h> 
 #include <string.h> 
 #include <sstream> 
 
 using namespace std; 
 IplImage* GetThresholdedImage(IplImage* img) 
 {
	 IplImage* hsv_image = cvCreateImage(cvGetSize(img), 8, 3);
	 cvCvtColor(img, hsv_image, CV_BGR2HSV);         
	 IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);         
	 cvInRangeS(hsv_image, cvScalar(0, 30, 80), cvScalar(20, 150, 255), imgThreshed);         
	 cvReleaseImage(&hsv_image);    
	 return imgThreshed; 
} 

int main() 
{        
	CvCapture* capture = 0;        
	capture = cvCaptureFromCAM( 0 );        
	if(!capture)        
	{                
		printf("could not intialize capturing....\n");                
		return -1;        
	}        
	int width = cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH);        
	int height = cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT);        
	int fps = cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);        
	int count = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);        
	printf("width: %d, height: %d, frame rate: %d, count: %d\n", width, height, fps, count);        
	cvNamedWindow("video");        
	cvNamedWindow("thresh");           
	// This image holds the "scribble" data...    
	// the tracked positions of the ball    
	IplImage* imgScribble = NULL;        
	while(true)        
	{                
		IplImage* frame = 0;                
		frame = cvQueryFrame(capture);                
		if(!frame)                        
		break;                 
		// If this is the first frame, we need to initialize it        
		if(imgScribble == NULL)        
		{            
			imgScribble = cvCreateImage(cvGetSize(frame), 8, 3);        
		}                 
		IplImage* imgYellowThresh = GetThresholdedImage(frame);                  
		// Calculate the moments to estimate the position of the ball        
		CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));        
		cvMoments(imgYellowThresh, moments, 1);        
		// The actual moment values        
		double moment10 = cvGetSpatialMoment(moments, 1, 0);        
		double moment01 = cvGetSpatialMoment(moments, 0, 1);        
		double area = cvGetCentralMoment(moments, 0, 0);                
		// Holding the last and current ball positions        
		static int posX = 0;        
		static int posY = 0;        
		int lastX = posX;        
		int lastY = posY;        
		posX = moment10/area;        
		posY = moment01/area;                   
		// Print it out for debugging purposes                
		printf("position (%d,%d)\n", posX, posY);                
		// We want to draw a line only if its a valid position        
		if(lastX>0 && lastY>0 && posX>0 && posY>0)        
		{            
			// Draw a yellow line from the previous point to the current point                         
			//cvCircle( imgScribble, cvPoint(posX,posY), 3, CV_RGB(0,255,0), -1, 8, 0 );                         
			//cvCircle( imgScribble, cvPoint(lastX,lastY), 3, CV_RGB(255,0,0), 3, 8, 0 );           
			// cvLine(imgScribble, cvPoint(lastX, lastY), cvPoint(lastX, lastY), cvScalar(0,0,255),8);                        
			cvLine(imgScribble, cvPoint(lastX, lastY), cvPoint(lastX, lastY), cvScalar(0,0,255), 3 , 2 , 0);        
		}                
		cvErode(frame, frame, 0, 2);        
		// Add the scribbling image and the frame...        
		cvAdd(frame, imgScribble, frame);        
		cvShowImage("scribble", imgScribble);
		cvShowImage("thresh", frame);        
		cvShowImage("video", imgYellowThresh);                
		//displaying the images, we check if a key was pressed:        
		// Wait for a keypress        
		int c = cvWaitKey(10);        
		if(c!=-1)        
		{            
			// If pressed, break out of the loop            
			break;        
		}
		 
        // Release the thresholded image+moments... we need no memory leaks.. please        
        cvReleaseImage(&imgYellowThresh);        
        delete moments;        
        }                
        cvReleaseCapture(&capture);                
        //return 0; 
}
