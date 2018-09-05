#ifndef IMAGE_H
#define IMAGE_H

#include <QByteArray>
#include <QRgb>
#include <QVector>

class Image
{
public:
    typedef QVector<QRgb> Palette;

    Image();
    QByteArray sampleData() const;
    void setSampleData(const QByteArray &sampleData);
    uint8_t bps() const;
    void setBps(const uint8_t &bps);
    void setBpsFromColorCount(const uint8_t colors);
    Palette palette() const;
    void setPalette(const Palette &palette);
    uint width() const;
    uint height() const;
    void setWidthHeight(const uint width, const uint height);

protected:
    QByteArray _sampleData;
    uint8_t _bps;
    QVector<QRgb> _palette;
    uint _width, _height;
};

#endif // IMAGE_H
