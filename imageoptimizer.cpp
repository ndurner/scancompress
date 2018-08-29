#include "imageoptimizer.h"
#include "libimagequant/libimagequant.h"

#include <QDebug>
#include <QPalette>

QImage ImageOptimizer::reduceColors(QImage &src, int colors)
{
    QImage rgba = src.convertToFormat(QImage::Format_RGBA8888);

    liq_attr *attrs = liq_attr_create();
    liq_image *img = liq_image_create_rgba(attrs, rgba.constBits(),
                                                   rgba.width(), rgba.height(), 0);
    liq_set_max_colors(attrs, colors);

    liq_result *res;
    if (liq_image_quantize(img, attrs, &res) != LIQ_OK) {
        qCritical() << "Quantization failed\n";
        return rgba;
    }

    // Use libimagequant to make new image pixels from the palette

    const size_t rawSize = static_cast<size_t>(rgba.width() * rgba.height());
    QImage ret(rgba.size(), QImage::Format_Indexed8);
    liq_set_dithering_level(res, 1.0);

    liq_write_remapped_image(res, img, ret.bits(), rawSize);
    const liq_palette *pal = liq_get_palette(res);

    ret.setColorCount(static_cast<int>(pal->count));
    for (unsigned int idx = 0; idx < pal->count; idx++) {
        const auto c = pal->entries[idx];
        ret.setColor(static_cast<int>(idx), qRgb(c.r, c.g, c.b));
    }

    liq_result_destroy(res);
    liq_image_destroy(img);
    liq_attr_destroy(attrs);

    return ret;
}

ImageOptimizer::ImageOptimizer()
{

}
