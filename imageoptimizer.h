#ifndef IMAGEOPTIMIZER_H
#define IMAGEOPTIMIZER_H

#include <QImage>

class ImageOptimizer
{
public:
    static QImage reduceColors(QImage &src, int colors);
private:
    ImageOptimizer();

};

#endif // IMAGEOPTIMIZER_H
