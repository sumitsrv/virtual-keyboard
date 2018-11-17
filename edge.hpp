#ifndef EDGE_HPP
#define EDGE_HPP

class Edge {
public:
  void drawEdge(IplImage *img);
  IplImage *getAndMarkLines(IplImage *srcimg, IplImage *dstimg, int linecolor);
};

#endif
