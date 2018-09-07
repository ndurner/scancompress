#include "imageoptimizer.h"
#include "libimagequant/libimagequant.h"
#include "image.h"

#include <QDebug>
#include <QPalette>
#include <QBitArray>

Image ImageOptimizer::reduceColors(QImage &src, uint8_t colors, bool indexed)
{
    QByteArray quantRet;
    int quantScanlinePad = 0;
    Image out;

    if (indexed) {
        QImage rgba = src.convertToFormat(QImage::Format_RGBA8888);

        liq_attr *attrs = liq_attr_create();
        liq_image *img = liq_image_create_rgba(attrs, rgba.constBits(),
                                                       rgba.width(), rgba.height(), 0);
        liq_set_max_colors(attrs, colors);
        liq_set_speed(attrs, 1);

        qDebug() << "Quantizing to" << colors << "colors";

        liq_result *res;
        if (liq_image_quantize(img, attrs, &res) != LIQ_OK) {
            qCritical() << "Quantization failed\n";
            return Image(); // FIXME
        }

        liq_set_dithering_level(res, 1.0);

        quantRet.resize(rgba.width() * rgba.height());
        if (liq_write_remapped_image(res, img, quantRet.data(), size_t(quantRet.capacity())) != LIQ_OK) {
            qCritical() << "remapping image failed";
            return Image(); // FIXME
        }

        const liq_palette *pal = liq_get_palette(res);

        Image::Palette imgPal;
        for (unsigned int idx = 0; idx < pal->count; idx++) {
            const auto c = pal->entries[idx];
            imgPal.append(qRgb(c.r, c.g, c.b));
        }
        out.setPalette(imgPal);
        colors = uint8_t(imgPal.length());

        liq_result_destroy(res);
        liq_image_destroy(img);
        liq_attr_destroy(attrs);
    }
    else {
        if (src.format() != QImage::Format_Grayscale8) {
            QImage tmp = src.convertToFormat(QImage::Format_Grayscale8);
            quantRet.setRawData(reinterpret_cast<char *>(tmp.bits()), uint(tmp.sizeInBytes()));
            quantRet.detach();
            quantScanlinePad = tmp.bytesPerLine() - tmp.width();
        }
        else {
            quantRet.setRawData(reinterpret_cast<char *>(src.bits()), uint(src.sizeInBytes()));
            quantScanlinePad = src.bytesPerLine() - src.width();
        }
    }

    out.setBpsFromColorCount(colors);
    out.setWidthHeight(uint(src.width()), uint(src.height()));

    int padBits = 8 - ((src.width() * out.bps()) % 8);
    if (padBits == 8)
        padBits = 0;

    QByteArray bytes(static_cast<int>(((src.width() * out.bps() + padBits) * src.height()) / 8.0 + 0.5), Qt::Uninitialized);
    uchar *outPos = reinterpret_cast<uchar *>(bytes.data());
    uint8_t bitsUsed = 0;
    int byteInScanline = 0;

    qDebug() << "converting to" << out.bps() << "bits";

    for(QByteArray::iterator sample = quantRet.begin(); sample < quantRet.cend(); sample++) {
        uchar outByte;

        if (indexed)
            outByte = uchar(*sample);
        else
            outByte = static_cast<uchar>((uchar(*sample) / 255.0) * ((1 << out.bps()) - 1));

        outByte = uchar(outByte << (8 - out.bps()));

        *outPos = *outPos | (outByte >> bitsUsed);
        bitsUsed += out.bps();

        byteInScanline++;
        if (byteInScanline == src.width()) {
            byteInScanline = 0;
            bitsUsed = 0;
            outPos++;
            if (quantScanlinePad)
                sample += quantScanlinePad;
        }
        else {
            if (bitsUsed >= 8) {
                bitsUsed -= 8;
                outPos++;
                *outPos = uchar(outByte << (out.bps() - bitsUsed));
            }
        }
    }

    out.setSampleData(bytes);

    return out;
}

ImageOptimizer::ImageOptimizer()
{

}
