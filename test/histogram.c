#include <cv.h>
#include <highgui.h>

int main( int argc, char** argv )
{
    IplImage* src;
    if( argc == 2 && (src=cvLoadImage(argv[1], 1))!= 0)
    {
        IplImage* h_plane = cvCreateImage( cvGetSize(src), 8, 1 );
        IplImage* s_plane = cvCreateImage( cvGetSize(src), 8, 1 );
        IplImage* v_plane = cvCreateImage( cvGetSize(src), 8, 1 );
        IplImage* planes = v_plane ;
        IplImage* hsv = cvCreateImage( cvGetSize(src), 8, 3 );
        int v_bins = 30;
        int hist_size = v_bins;
        
        /* intensity varies from 0 (black-gray-white) to
           255 (pure spectrum color) */
        float v_ranges[] = { 0, 255 };
        float* ranges = v_ranges;
        int scale = 10;
        IplImage* hist_img =
            cvCreateImage( cvSize(v_bins*scale,v_bins*scale), 8, 3 );
        CvHistogram* hist;
        float max_value = 0;
        int h, s, v;

        cvCvtColor( src, hsv, CV_BGR2HSV );
        cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
        hist = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, &ranges, 1 );
        cvCalcHist( &planes, hist, 0, 0 );
        cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
        cvZero( hist_img );

        for( v = 0; v < v_bins; v++ )
        {
            //for( s = 0; s < s_bins; s++ )
            //{
                float bin_val = cvQueryHistValue_1D( hist, v );
                int intensity = cvRound(bin_val*255/max_value);
                cvRectangle( hist_img, cvPoint( v*scale, v*scale ),
                             cvPoint( (v+1)*scale - 1, (v+1)*scale - 1),
                             CV_RGB(intensity,intensity,intensity),
                             CV_FILLED );
           // }
        }

        cvNamedWindow( "Source", 1 );
        cvShowImage( "Source", src );
        
        cvNamedWindow("Intensity", 1);
        cvShowImage("Intensity", v_plane);

        cvNamedWindow( "H-S Histogram", 1 );
        cvShowImage( "H-S Histogram", hist_img );

        cvWaitKey(0);
    }
}
