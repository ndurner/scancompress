#include "application.h"

#include <QCommandLineParser>
#include <QImageReader>
#include <QDebug>

#include "pdfwriter.h"

Application::Application(int &argc, char **argv) : QCoreApplication(argc, argv)
{
    QCommandLineParser pars;

    QCommandLineOption help = pars.addHelpOption();
    pars.addPositionalArgument("input", "input file");
    pars.addPositionalArgument("output", "output file");

    const bool fail = !pars.parse(arguments());
    const auto &&args = pars.positionalArguments();

    initFailed = fail || pars.isSet(help) || args.length() < 2;
    if(initFailed) {
        pars.showHelp();
        return;
    }

    cfg.setInput(args[0]);
    cfg.setOutput(args[1]);
}

void Application::run()
{
    // check command line validity
    if (initFailed) {
        qApp->exit(1);
        return;
    }

    // load input
    QImageReader rd(cfg.input());
    if (!rd.canRead()) {
        qCritical() << "Cannot open" << cfg.input() << ":" << rd.errorString();
        qApp->exit(1);
        return;
    }

    // setup PDF
    qDebug() << Q_FUNC_INFO << "setup PDF";
    PDFWriter pdf(this, cfg.output());
    if (!pdf.writeHeader()) {
        qApp->exit(1);
        return;
    }

    // process individual pages
    for (auto pgCntr = rd.imageCount(); pgCntr > 0; --pgCntr, rd.jumpToNextImage()) {
        QImage img = rd.read();

        if (img.isNull()) {
            qCritical() << "Loading input page" << rd.currentImageNumber() <<
                        "failed:" << rd.errorString();
            qApp->exit(1);
            return;
        }

        qDebug() << Q_FUNC_INFO << "add page";
        pdf.addPage(img);

    }

    // finish PDF
    qDebug() << Q_FUNC_INFO << "finish pdf";
    pdf.finish();

    qApp->quit();
}
