#include "webcam.hpp"

int WebCam::capture(VideoCapture capture, char* img_name)
{

    Mat frame;
    int key = 0;

    /* create a window for the video */
    namedWindow( "result", CV_WINDOW_AUTOSIZE );

    Mat img;
    if(!capture.grab())
    {
        // capture a frame
        printf("Could not grab a frame\n\7");
        exit(0);
    }
    while( key != 27)
    {
        /* get a frame */
        capture.read(frame);

        /* always check */
        if( countNonZero(frame)  ) break;

        /* display current frame */
        imshow("result", frame );
        //cvSetMouseCallback( "result", mouseHandler, frame );
        /* exit if user press 'q' */
        key = cvWaitKey(5);
        //printf("%d  %c\t",key, key);
    }
    capture.retrieve(img, 0);
    //        if(!cvSaveImage(img_name,img))
    //        {
    //            printf("Could not save: %s\n",img_name);
    //        }

    cvDestroyWindow( "result" );
    //        cvReleaseCapture( &capture );
    //        cvReleaseImage(&frame);
    //        cvReleaseImage(&img);
    return 0;
}

void WebCam::mouseHandler(int event, int x, int y, int flags, void *img)
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

