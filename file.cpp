#include "file.h"

File::File(const QString fn) : QFile(fn)
{

}

quint64 File::write(QByteArray &ba)
{
    auto ret = QFile::write(ba);
    if (ret == -1)
        throw new IoErr;

    return static_cast<quint64>(ret);
}

quint64 File::write(QString str)
{
    auto ret = QFile::write(str.toUtf8());
    if (ret == -1)
        throw new IoErr;

    return static_cast<quint64>(ret);
}

quint64 File::write(const char *str)
{
    auto ret = QFile::write(str);
    if (ret == -1)
        throw new IoErr;

    return static_cast<quint64>(ret);
}

quint64 File::write(const uchar *bytes, qint64 len)
{
    auto ret = QFile::write(reinterpret_cast<const char *>(bytes), len);
    if (ret == -1)
        throw new IoErr;

    return static_cast<quint64>(ret);
}

bool File::open(QIODevice::OpenMode flags)
{
    if (!QFile::open(flags))
        throw new IoErr;

    return true;
}

