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

private:
    QString m_input;
    QString m_output;
};

#endif // CONFIG_H
