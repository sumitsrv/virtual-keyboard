// Example : image segmentation
// usage: prog {<image_name> | <video_name>}

// Author : Toby Breckon, toby.breckon@cranfield.ac.uk

// Copyright (c) 2007 School of Engineering, Cranfield University
// License : LGPL - http://www.gnu.org/licenses/lgpl.html

// portions based on OpenCV library example pyramid_segmentation.c
// N.B. trackbars only works for video file or camera inouts

#include "cv.h"       // open cv general include file
#include "highgui.h"  // open cv GUI include file

#include <stdio.h>
#include <algorithm>
using namespace std;

/******************************************************************************/
// setup the camera index / file capture properly based on OS platform

// 0 in linux gives first camera for v4l
//-1 in windows gives first device or user dialog selection

#ifdef linux
	#define CAMERA_INDEX 0
	#define VIDEOCAPTURE cvCaptureFromFile
#else
	#define CAMERA_INDEX -1
    #define VIDEOCAPTURE cvCaptureFromAVI
#endif

/******************************************************************************/

int main( int argc, char** argv )
{

  IplImage* img = NULL;      // image object 
  CvCapture* capture = NULL; // capture object
	
  char* windowName = "Cranfield University: Segmentation Demonstrator"; // window name

  bool keepProcessing = true;	// loop control flag
  char key;						// user input	
  int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame		
	
  // if command line arguments are provided try to read image/video_name
  // otherwise default to capture from attached H/W camera 

    if( 
	  ( argc == 2 && (img = cvLoadImage( argv[1], 1)) != 0 ) ||
	  ( argc == 2 && (capture = VIDEOCAPTURE( argv[1] )) != 0 ) || 
	  ( argc != 2 && (capture = cvCreateCameraCapture( 0 )) != 0 )
	  )
    {
      // create window object (use flag=0 to allow resize, 1 to auto fix size)

      cvNamedWindow(windowName, 0);		
		
	  // setup and assign trackbars for segmentation parameters

	  int smooth = 3;	
	  cvCreateTrackbar("Smoothing Window", windowName, &smooth, 10, NULL);
      int threshold1 = 150;
	  cvCreateTrackbar("Threshold1", windowName, &threshold1, 255, NULL);
      int threshold2 = 30;
	  cvCreateTrackbar("Threshold2", windowName,  &threshold2, 255, NULL);
	  int level = 3;
	  cvCreateTrackbar("Pyramid Levels", windowName,  &level, 5, NULL);
		
	  // define any required variables / images for processing	
	  // (if using a capture object we need to get a frame first to get the size)
		
	  if (capture) {
	
		  // cvQueryFrame s just a combination of cvGrabFrame 
		  // and cvRetrieveFrame in one call.
		  
		  img = cvQueryFrame(capture);
		  if(!img){
			if (argc == 2){				  
				printf("End of video file reached\n");
			} else {
				printf("ERROR: cannot get next fram from camera\n");
			}
			exit(0);
		  }
		  
	  }
	  IplImage* imgCopy = cvCloneImage( img );
	  CvMemStorage *storage = cvCreateMemStorage ( 1000 );
	  CvSeq *comp;
	  
	  // start main loop	
		
	  while (keepProcessing) {
		
		  // if capture object in use (i.e. video/camera)
		  // get image from capture object
			
		  if (capture) {
	
			  // cvQueryFrame s just a combination of cvGrabFrame 
			  // and cvRetrieveFrame in one call.
			  
			  img = cvQueryFrame(capture);
			  if(!img){
				if (argc == 2){				  
					printf("End of video file reached\n");
				} else {
					printf("ERROR: cannot get next fram from camera\n");
				}
				exit(0);
			  }
			  
		  }	else {
			  
			  // if not a capture object set event delay to zero so it waits
			  // indefinitely (as single image file, no need to loop)
			  
			  EVENT_LOOP_DELAY = 0;
		  }			  
		
		  // ***
		  
		  // ensure prior smoothing  parameter is odd
		  
		  if(fmod(smooth, 2.0) == 0)
		  {
			  smooth++;
		  }
		  
		  // pre-smooth a copy of the image and segment it (output back into img)
		  
		  cvSmooth(img, imgCopy, CV_GAUSSIAN, max(1, smooth), max(1, smooth));
		  cvPyrSegmentation(imgCopy, img,
		  			storage, &comp, max(1, level), threshold1+1, threshold2+1);
		  
		  // ***
		  
		  // display image in window
	
		  cvShowImage( windowName, img );
		  
		  // start event processing loop (very important,in fact essential for GUI)
	      // 4 ms roughly equates to 100ms/25fps = 4ms per frame
		  
		  key = cvWaitKey(EVENT_LOOP_DELAY);

		  if (key == 'x'){
			
	   		// if user presses "x" then exit
			
	   			printf("Keyboard exit requested : exiting now - bye!\n");	
	   			keepProcessing = false;
		  } 
		  
		  
	  }
      
      // destroy window objects
      // (triggered by event loop *only* window is closed)

      cvDestroyAllWindows();

      // destroy image object (if it does not originate from a capture object)

      if (!capture){
		  cvReleaseImage( &img );
      }
	  cvReleaseImage( &imgCopy );
	  
      // all OK : main returns 0

      return 0;
    }

    // not OK : main returns -1

    return -1;
}
/******************************************************************************/
