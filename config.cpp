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

bool Config::getIndexed() const
{
    return indexed;
}

void Config::setIndexed(bool value)
{
    indexed = value;
}

uint8_t Config::getColors() const
{
    return colors;
}

void Config::setColors(int value)
{
    colors = value;
}

uint Config::dpi() const
{
    return _dpi;
}

void Config::setDpi(const uint &dpi)
{
    _dpi = dpi;
}
