#ifndef CONTRASTNEW_HPP
#define CONTRASTNEW_HPP

class ContrastNew {
public:
    void glg(IplImage *img);
    void mergeLevels(CvHistogram*, float *, int*, float[], float[]);
    void lookUpTable(float *, float [], float [], int*, float []);
};

#endif // CONTRASTNEW_HPP
