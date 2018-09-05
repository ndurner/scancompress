#include "image.h"

Image::Image()
{

}

QByteArray Image::sampleData() const
{
    return _sampleData;
}

void Image::setSampleData(const QByteArray &sampleData)
{
    _sampleData = sampleData;
}

uint8_t Image::bps() const
{
    return _bps;
}

void Image::setBps(const uint8_t &bps)
{
    _bps = bps;
}

void Image::setBpsFromColorCount(const uint8_t colors)
{
    uint8_t bits;
    const auto maxColor = colors - 1;
    for (bits = 8; bits > 0 && ((maxColor & (1 << (bits - 1))) == 0); bits--);

    setBps(bits);
}

Image::Palette Image::palette() const
{
    return _palette;
}

void Image::setPalette(const Palette &palette)
{
    _palette = palette;
}

uint Image::width() const
{
    return _width;
}

uint Image::height() const
{
    return _height;
}

void Image::setWidthHeight(const uint width, const uint height)
{
    _width = width;
    _height = height;
}
