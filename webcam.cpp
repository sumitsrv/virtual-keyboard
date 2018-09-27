#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

class WebCam {
    int capture(CvCapture *capture, char* img_name)
    {

        IplImage  *frame = 0;
        int key = 0;

        /* create a window for the video */
        cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );

        IplImage* img = 0;
        if(!cvGrabFrame(capture))
        {
            // capture a frame
            printf("Could not grab a frame\n\7");
            exit(0);
        }
        while( key != 27)
        {
            /* get a frame */
            frame = cvQueryFrame( capture );

            /* always check */
            if( !frame ) break;

            /* display current frame */
            cvShowImage( "result", frame );
            //cvSetMouseCallback( "result", mouseHandler, frame );
            /* exit if user press 'q' */
            key = cvWaitKey(5);
            //printf("%d  %c\t",key, key);
        }
        img=cvRetrieveFrame(capture);
        if(!cvSaveImage(img_name,img))
        {
            printf("Could not save: %s\n",img_name);
        }

        cvDestroyWindow( "result" );
        cvReleaseCapture( &capture );
        cvReleaseImage(&frame);
        cvReleaseImage(&img);
        return 0;
    }

    void mouseHandler(int event, int x, int y, int flags, void *img)
    {
        switch(event) {
        /* left button down */
        case CV_EVENT_LBUTTONDOWN:
            //exit(0);
            break;

            /* right button down */
        case CV_EVENT_RBUTTONDOWN:
            fprintf(stdout, "Right button down (%d, %d).\n", x, y);
            break;

            /* mouse move */
        case CV_EVENT_MOUSEMOVE:
            
            break;
        }
    }
};
