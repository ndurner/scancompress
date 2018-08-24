#ifndef PDFWRITER_H
#define PDFWRITER_H

#include <QObject>
#include <QFile>
#include <QVector>

#include "file.h"

class PDFWriter : public QObject
{
    Q_OBJECT

public:
    explicit PDFWriter(QObject *parent, const QString fn);
    bool writeHeader();
    bool addPage(const QImage &img);
    bool finish();

signals:

public slots:

private:
    File f;
    quint64 curOfs;
    quint16 curObj;
    quint16 curImg;
    QStringList xrefs;
    QStringList pageIds;
};

#endif // PDFWRITER_H
