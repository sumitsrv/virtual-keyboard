#ifndef OCR_HPP
#define OCR_HPP

class OCR{
public:
    void findX(IplImage*, int*, int*);
    void findY(IplImage*, int*, int*);
    CvRect findBB(IplImage*);
    IplImage preprocessing(IplImage*, int, int);
    void ocr();
};

#endif

