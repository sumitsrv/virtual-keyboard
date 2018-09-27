#ifndef WEBCAM_HPP
#define WEBCAM_HPP

class WebCam {
public:
    int capture(CvCapture *, char*);
    void mouseHandler(int event, int x, int y, int flags, void *param);
    int end = 0;
};
#endif
