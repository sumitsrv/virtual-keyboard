#ifndef KEYS_HPP
#define KEYS_HPP

class Keys{
public:
    int count1, count2, count3;
    CvMemStorage* storage;

    void remove_loose_ends(IplImage*);
    void drawSquares( IplImage* img, CvSeq* squares, IplImage* original, int mode );
    CvSeq* findSquares4( IplImage* img, CvMemStorage* storage );
    double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 );
    int locate(IplImage*, IplImage*);
};

#endif

