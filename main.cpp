#include "application.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    Application a(argc, argv);

    {
        QObject o;
        o.connect(&o, &QObject::destroyed, &a, &Application::run, Qt::QueuedConnection);
    }

    return a.exec();
}
