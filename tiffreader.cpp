#include "tiffreader.h"

#include <QtEndian>
#include <QDebug>

uint TIFFReader::dpi(QByteArray &inp)
{
    bool bigEndian;
    uint16_t ofs;
    uint32_t unit = 2; // inch

    // read header
    auto idx = inp.begin();

    bigEndian = (memcmp(idx, "MM", 2) == 0);
    if (memcmp(idx + 2, "\x00\x2A", 2) != 0)
        return 0;
    ofs =*(reinterpret_cast<uint16_t *>(idx + 6));
    if (bigEndian)
        ofs = qFromBigEndian(ofs);

    // move to IFD
    if (inp.begin() + ofs + sizeof(uint16_t) > inp.end())
        return 0;

    idx = inp.begin() + ofs;

    // process IFD
    while (true) {
        uint16_t entries;

        // get number of entries
        entries = *reinterpret_cast<uint16_t *>(idx);
        if (bigEndian)
            entries = qFromBigEndian(entries);
        idx += 2;

        // process IFD entries
        for (uint32_t entry = 0; entry < entries; entry++) {
            const QByteArray::iterator ifd = idx;

            if (ifd + 12 > inp.end())
                return 0;

            // read IFD entry metadata
            uint16_t tag = *(reinterpret_cast<uint16_t *>(ifd));
            if (bigEndian)
                tag = qFromBigEndian(tag);

            uint16_t type = *(reinterpret_cast<uint16_t *>(ifd + 2));
            if (bigEndian)
                type = qFromBigEndian(type);

            // read the IFDs we're interested in
            if (tag == 296) { // Resolution Unit
                Q_ASSERT(type == 3); // SHORT
                unit = *(reinterpret_cast<uint32_t *>(ifd + 8));
                if (bigEndian)
                    unit = qFromBigEndian(unit);
            } else if (tag == 282 || tag == 283) { // Resolution Unit
                uint32_t valOfs, num, denom;

                valOfs = *(reinterpret_cast<uint32_t *>(ifd + 8));
                if (bigEndian)
                    valOfs = qFromBigEndian(valOfs);

                num = *reinterpret_cast<uint32_t *>(inp.begin() + valOfs);
                if (bigEndian)
                    num = qFromBigEndian(num);

                denom = *reinterpret_cast<uint32_t *>(inp.begin() + valOfs + sizeof(uint32_t));
                if (bigEndian)
                    denom = qFromBigEndian(denom);

                auto dpi = num / denom;
                if (unit == 3)
                    // dots/cm
                    dpi *= 2.54;
                else if (unit != 2)
                    // 0 = "no absolute measurement", 2 = inches
                    return 0;

                return dpi;
            }

            idx += 12; // 12 = IFD size
        }

        // get next IFD position
        uint32_t nextIFD = *reinterpret_cast<uint32_t *>(idx);
        if (bigEndian)
            nextIFD = qFromBigEndian(nextIFD);

        if (nextIFD == 0)
            return 0; // resolution not found

        idx = inp.begin() + nextIFD;
    }
}

TIFFReader::TIFFReader()
{

}
