#ifndef CONTRAST_HPP
#define CONTRAST_HPP

class Contrast {
public:
    void glg(IplImage* img);
    void mergeLevels(CvHistogram*, float *, int*, float[], float[]);
    void lookUpTable(float *, float [], float [], int*, float []);

};
#endif
