#include "pdfwriter.h"

#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QBitArray>
#include "zopfli/src/zopfli/zopfli.h"

PDFWriter::PDFWriter(QObject *parent, const QString fn) : QObject(parent), f(fn)
{
    curOfs = curImg = 0;

    /* Reserve some object IDs:
             1 /Catalog
             2 /Pages
    */
    curObj = 2;
}

bool PDFWriter::writeHeader()
{
    try {
        if (!f.open(QFile::WriteOnly))
            throw new File::IoErr();
        curOfs += f.write("%PDF-1.7\n");
    }
    catch (File::IoErr ex) {
        return false;
    }

    return true;
}

bool PDFWriter::addPage(const QImage &img)
{
    try {
        const auto objIdx = ++curObj;
        const auto imgIdx = ++curImg;

        // record XRef for later use
        xrefs.append(QString("%1 00000 n")
                        .arg(curOfs, 10, 10, QChar('0')));

        // compress image
        qDebug() << "compressing...";
        ZopfliOptions opts;
        unsigned char *flate = nullptr;
        size_t flateLen = 0;
        QByteArray imgData;

        if (img.format() == QImage::Format_Mono) {
            // invert pixel bits (grayscale => b/w) and write out
            const size_t lineLen = size_t(img.width() / 8.0 + 0.5);
            const int lineLenBits = int(lineLen * 8);
            const QBitArray mask(lineLenBits, true);

            for (int row = 0; row < img.height(); row++) {
                QBitArray sl = QBitArray::fromBits(
                            reinterpret_cast<const char *>(img.scanLine(row)), lineLenBits);
                sl ^= mask;
                imgData.append(QByteArray(sl.bits(), int(lineLen)));
            }
        }
        else
            imgData = QByteArray::fromRawData(
                        reinterpret_cast<const char *>(img.constBits()), img.sizeInBytes());

        ZopfliInitOptions(&opts);
        opts.numiterations = 15;
        ZopfliCompress(&opts, ZOPFLI_FORMAT_ZLIB, reinterpret_cast<const uchar *>(imgData.constData()),
                       static_cast<size_t>(imgData.length()), &flate, &flateLen);
        qDebug() << "compression done";

        // write image
        curOfs += f.write(QString("%1 0 obj\n"
                                  "<<\n"
                                  "/Type /XObject\n"
                                  "/Subtype /Image\n"
                                  "/Width %2\n"
                                  "/Height %3\n"
                                  "/ColorSpace ")
                                  .arg(objIdx)
                                  .arg(img.width())
                                  .arg(img.height())
                         );
        if (img.format() == QImage::Format_Indexed8) {
            curOfs += f.write(QString("[/Indexed /DeviceGray %1 <")
                              .arg(img.colorCount() - 1));
            // write indexed palette
            foreach (QRgb c, img.colorTable()) {
                curOfs += f.write(QString("%1 ")
                              .arg(qGray(c), 2, 16, QChar('0')));
            }
            curOfs += f.write(">]\n");
        }
        else {
            curOfs += f.write("/DeviceGray\n");
        }

        curOfs += f.write(QString("/BitsPerComponent %1\n"
                                  "/Filter /FlateDecode\n"
                                  "/Length %2\n"
                                  ">>\n"
                                  "stream\n")
                                  .arg(img.bitPlaneCount())
                                  .arg(flateLen)
                         );

       curOfs += f.write(flate, static_cast<qint64>(flateLen));
       free(flate);
       curOfs += f.write(QString("\nendstream\n"
                                 "endobj\n"));

        // write page appearance stream
        xrefs.append(QString("%1 00000 n").arg(curOfs, 10, 10, QChar('0')));
        const auto appIdx = ++curObj;
        const auto appear = QString(
                    "q\n"
                    "595.275591 0 0 841.889764 0 0 cm\n" /* TODO: actual dimensions */
                    "/I%2 Do\n"
                    "Q\n"
                    ).arg(imgIdx);

        curOfs += f.write(QString("%1 0 obj\n"
                                  "<< /Length %2\n"
                                  ">>\n"
                                  "stream\n"
                                  "%3"
                                  "endstream\n"
                                  "endobj\n")
                            .arg(appIdx)
                            .arg(appear.length())
                            .arg(appear));

        // write page resource
        const quint16 resObj = ++curObj;
        xrefs.append(QString("%1 00000 n").arg(curOfs, 10, 10, QChar('0')));
        curOfs += f.write(QString("%1 0 obj\n"
                                  "<< /XObject <<\n"
                                  "/I%2 %3 0 R\n"
                                  ">>\n"
                                  ">>\n"
                                  "endobj\n")
                            .arg(resObj)
                            .arg(imgIdx)
                            .arg(objIdx));

        // write page
        xrefs.append(QString("%1 00000 n").arg(curOfs, 10, 10, QChar('0')));
        const auto pageIdx = ++curObj;

        curOfs += f.write(QString("%1 0 obj\n"
                                  "<< /Type /Page\n"
                                  "/Parent 2 0 R\n"
                                  "/MediaBox [0 0 595 841]\n" /* TODO: actual dimensions */
                                  "/Contents %2 0 R\n"
                                  "/Resources %3 0 R\n"
                                  ">>\n"
                                  "endobj\n")
                            .arg(pageIdx)
                            .arg(appIdx)
                            .arg(resObj));

        pageIds.append(QString().setNum(pageIdx));

        return true;
    }
    catch(...) {
        qCritical() << "adding page failed";
    }

    return false;
}

bool PDFWriter::finish()
{
    // write /Pages object
    const auto pagesOfs = curOfs;

    curOfs += f.write(QString("2 0 obj\n"
                              "<< /Type /Pages\n"
                              "/Count %1\n"
                              "/Kids [%2 0 R]\n"
                              "/ProcSet [/PDF /Text /ImageB /ImageC]\n"
                              ">>\n"
                              "endobj\n")
                              .arg(pageIds.count())
                              .arg(pageIds.join(" 0 R ")));

    xrefs.prepend(QString("%1 00000 n").arg(pagesOfs, 10, 10, QChar('0')));

    // write /Catalog object
    const auto catalogOfs = curOfs;
    curOfs += f.write(QString("1 0 obj\n"
                              "<< /Type /Catalog\n"
                              "/Pages 2 0 R\n"
                              ">>\n"
                              "endobj\n"));

    xrefs.prepend(QString("%1 00000 n").arg(catalogOfs, 10, 10, QChar('0')));

    // add xrefs
    const auto refOfs = curOfs;
    curOfs += f.write(QString("xref\n"
                              "%1 %2\n"
                              "0000000000 65535 f \n%3 \n")
                              .arg(0)
                              .arg(xrefs.count() + 1)
                              .arg(xrefs.join(" \n")));
    // add trailer
    curOfs += f.write(QString(
                    "trailer\n"
                    "<< /Size %1\n"
                    "/Root 1 0 R\n"
                    ">>\n"
                    "startxref\n"
                    "%2\n"
                    "%%EOF").arg(xrefs.count() + 1)
                            .arg(refOfs));

    return true;
}
