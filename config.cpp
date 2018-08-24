#include "config.h"

Config::Config(QObject *parent) : QObject(parent)
{

}

QString Config::input() const
{
    return m_input;
}

void Config::setInput(const QString &input)
{
    m_input = input;
}

QString Config::output() const
{
    return m_output;
}

void Config::setOutput(const QString &output)
{
    m_output = output;
}
