#ifndef FILE_H
#define FILE_H

#include <QFile>
#include <exception>

class File : public QFile
{
public:
    class IoErr : public std::exception{};

    File(const QString fn);
    quint64 write(QByteArray &ba);
    quint64 write(QString str);
    quint64 write(const char *str);
    quint64 write(const uchar *bytes, qint64 len);
    bool open(OpenMode flags);
};

#endif // FILE_H
