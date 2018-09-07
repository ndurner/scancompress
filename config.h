#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

class Config : QObject
{
public:
    Config(QObject *parent = nullptr);

    QString input() const;
    void setInput(const QString &input);

    QString output() const;
    void setOutput(const QString &output);

    bool getIndexed() const;
    void setIndexed(bool value);

    uint8_t getColors() const;
    void setColors(int value);

    uint dpi() const;
    void setDpi(const uint &dpi);

private:
    QString m_input;
    QString m_output;
    bool indexed;
    int colors;
    uint _dpi;
};

#endif // CONFIG_H
