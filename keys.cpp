#include "keys.hpp"
#include "colorEdge.hpp"
#include "edge.hpp"

double Keys::angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
void Keys::findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                        fabs(contourArea(approx)) > 1000 &&
                        isContourConvex(approx) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 )
                        squares.push_back(approx);
                }
            }
        }
    }
}


// the function draws all the squares in the image
void Keys::drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
    }

    imshow(wndname, image);
}


int Keys::locate(Mat img0, Mat original)
{
    int c;
    Mat img = Mat();
    // create memory storage that will contain all the dynamic data
    //           storage = new CvMemStorage();

    vector<vector<Point> > squares;
    // load i-th image
    //img0 = img1;
    //        if( img0 == NULL)
    //        {
    //            printf("Couldn't load\n");
    //        }
    img = img0.clone();

    // create window and a trackbar (slider) with parent "image" and set callback
    // (the slider regulates upper threshold, passed to Canny edge detector)
    cvNamedWindow( "Keys Detection", 1 );
    cvMoveWindow("Keys Detection", img.cols, 0);
    // find and draw the squares
    //        drawSquares( img, findSquares4( img, &storage ), original, CREATE_DATA_SET );

    findSquares(img, squares);
    drawSquares(img, squares);
    // wait for key.
    // Also the function cvWaitKey takes care of event processing
    c = cvWaitKey(0);
    // release both images
    //        cvReleaseImage( &img );
    // clear memory storage - reset free space position
    //        cvClearMemStorage( storage );

    cvDestroyWindow( "Keys Detection" );

    return 0;
}


void Keys::remove_loose_ends(Mat canny)
{
    Mat clone = canny.clone();

    uchar* data= (uchar*)canny.data;
    uchar* cldata= (uchar*)clone.data;

    int height = canny.rows;
    int width = canny.cols;
    int step= canny.step;

    int i, j, k, count = 0;

    for(i=0; i<height; i++)
    {
        for(j=0; j<width; j++)
        {
            count = 0;
            if(data[i*step + j] == BG)
            {
                for(k=-1; k<2; k++)
                {
                    if(cldata[(i+k)*step + (j+k)] == BG)
                    {
                        count++;
                    }
                    if(cldata[i*step +j+k] == BG)
                    {
                        count++;
                    }
                    if(cldata[(i+k)*step +j] == BG)
                    {
                        count++;
                    }
                }
                //printf("%d\t",count);
                if(count <4)
                    data[i*step + j] = FG;
            }

        }
    }
    //        cvReleaseImage(&clone);
}

