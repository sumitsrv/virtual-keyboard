#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

//Global storage
//
//Float, 3-channel images
//
IplImage *IavgF,*IdiffF, *IprevF, *IhiF, *IlowF;
IplImage *Iscratch,*Iscratch2;
//Float, 1-channel images
//
IplImage *Igray1,*Igray2, *Igray3;
IplImage *Ilow1, *Ilow2, *Ilow3;
IplImage *Ihi1,
*Ihi2, *Ihi3;
// Byte, 1-channel image
//
IplImage *Imaskt;
//Counts number of images learned for averaging later.
//
float Icount;

void AllocateImages( IplImage* I ){
CvSize sz = cvGetSize( I );
IavgF= cvCreateImage( sz, IPL_DEPTH_32F,3);
IdiffF= cvCreateImage( sz, IPL_DEPTH_32F,3);
IprevF= cvCreateImage( sz, IPL_DEPTH_32F,3);
IhiF= cvCreateImage( sz, IPL_DEPTH_32F,3);
IlowF= cvCreateImage( sz, IPL_DEPTH_32F,3);
Ilow1= cvCreateImage( sz, IPL_DEPTH_32F,3);
Ilow2= cvCreateImage( sz, IPL_DEPTH_32F,3);
Ilow3= cvCreateImage( sz, IPL_DEPTH_32F,3);
Ihi1= cvCreateImage( sz, IPL_DEPTH_32F,3);
Ihi2= cvCreateImage( sz, IPL_DEPTH_32F,3);
Ihi3= cvCreateImage( sz, IPL_DEPTH_32F,3);
cvZero(IavgF );
cvZero(IdiffF );
cvZero(IprevF );
cvZero(IhiF );
cvZero(IlowF );
Icount = 0.00001; //Protect against divide
Iscratch = cvCreateImage(sz, IPL_DEPTH_32F, 3);
Iscratch2 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
Igray1 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
Igray2 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
Igray3 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
Imaskt = cvCreateImage(sz, IPL_DEPTH_32F, 3);
cvZero( Iscratch );
cvZero( Iscratch2 );
}

void accumulateBackground( IplImage *I )
{
	static int first = 1;
	// nb. Not thread safe
	cvCvtScale( I, Iscratch, 1, 0 );
	// convert to float
	if( !first )
	{
		cvAcc( Iscratch, IavgF );
		cvAbsDiff( Iscratch, IprevF, Iscratch2 );
		cvAcc( Iscratch2, IdiffF );
		Icount += 1.0;
	}
	first = 0;
	cvCopy( Iscratch, IprevF );
}

void createModelsfromStats() 
{
	cvConvertScale( IavgF, IavgF,( double)(1.0/Icount) );
	cvConvertScale( IdiffF, IdiffF,(double)(1.0/Icount) );
	//Make sure diff is always something
	//
	cvAddS( IdiffF, cvScalar( 1.0, 1.0, 1.0), IdiffF );
	setHighThreshold( 7.0 );
	setLowThreshold( 6.0 );
}

void setHighThreshold( float scale )
{
	cvConvertScale( IdiffF, Iscratch, scale );
	cvAdd( Iscratch, IavgF, IhiF );
	cvSplit( IhiF, Ihi1, Ihi2, Ihi3, 0 );
}

void setLowThreshold( float scale )
{
	cvConvertScale( IdiffF, Iscratch, scale );
	cvSub( IavgF, Iscratch, IlowF );
	cvSplit( IlowF, Ilow1, Ilow2, Ilow3, 0 );
}

void backgroundDiff(IplImage *I, IplImage *Imask) 
{
	cvCvtScale(I,Iscratch,1,0); // To float;
	cvSplit( Iscratch, Igray1,Igray2,Igray3, 0 );
	//Channel 1
	//
	cvInRange(Igray1,Ilow1,Ihi1,Imask);
	//Channel 2
	//
	cvInRange(Igray2,Ilow2,Ihi2,Imaskt);
	cvOr(Imask,Imaskt,Imask);
	//Channel 3
	//
	cvInRange(Igray3,Ilow3,Ihi3,Imaskt);
	cvOr(Imask,Imaskt,Imask)
	//Finally, invert the results
	//
	cvSubRS( Imask, 255, Imask);
}

void DeallocateImages()
{
	cvReleaseImage( &IavgF);
	cvReleaseImage( &IdiffF );
	cvReleaseImage( &IprevF );
	cvReleaseImage( &IhiF );
	cvReleaseImage( &IlowF );
	cvReleaseImage( &Ilow1 );
	cvReleaseImage( &Ilow2 );
	cvReleaseImage( &Ilow3 );
	cvReleaseImage( &Ihi1 );
	cvReleaseImage( &Ihi2 );
	cvReleaseImage( &Ihi3 );
	cvReleaseImage( &Iscratch );
	cvReleaseImage( &Iscratch2 );
	cvReleaseImage( &Igray1 );
	cvReleaseImage( &Igray2 );
	cvReleaseImage( &Igray3 );
	cvReleaseImage( &Imaskt);
}




