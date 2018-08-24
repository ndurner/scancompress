#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>
#include "config.h"

class Application : public QCoreApplication
{
public:
    Application(int &argc, char **argv);

public slots:
    void run();

private:
    Config cfg;
    bool initFailed;
};

#endif // APPLICATION_H
