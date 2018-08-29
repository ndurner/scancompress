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

    int getColors() const;
    void setColors(int value);

private:
    QString m_input;
    QString m_output;
    bool indexed;
    int colors;
};

#endif // CONFIG_H
