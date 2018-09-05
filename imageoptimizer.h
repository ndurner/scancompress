#ifndef IMAGEOPTIMIZER_H
#define IMAGEOPTIMIZER_H

#include "image.h"
#include <QImage>

class ImageOptimizer
{
public:
    static Image reduceColors(QImage &src, uint8_t colors, bool indexed);
private:
    ImageOptimizer();
};

#endif // IMAGEOPTIMIZER_H
