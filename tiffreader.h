#ifndef TIFFREADER_H
#define TIFFREADER_H

#include <QIODevice>

class TIFFReader
{
public:
    static uint dpi(QByteArray &inp);

private:
    TIFFReader();
};

#endif // TIFFREADER_H
