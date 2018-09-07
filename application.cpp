#include "application.h"

#include <QCommandLineParser>
#include <QImageReader>
#include <QDebug>
#include <QtEndian>
#include <QBuffer>

#include "pdfwriter.h"
#include "imageoptimizer.h"
#include "tiffreader.h"

Application::Application(int &argc, char **argv) : QCoreApplication(argc, argv)
{
    QCommandLineParser pars;

    QCommandLineOption help = pars.addHelpOption();
    pars.addPositionalArgument("input", "input file");
    pars.addPositionalArgument("output", "output file");
    pars.addOption({"indexed", "generate indexed palette"});
    pars.addOption({"colors", "number of colors to reduce to", "colors", "255"});
    pars.addOption({"dpi", "DPI to reduce to", "dpi"});

    const bool fail = !pars.parse(arguments());
    const auto &&args = pars.positionalArguments();

    initFailed = fail || pars.isSet(help) || args.length() < 2;
    if(initFailed) {
        pars.showHelp();
        return;
    }

    cfg.setInput(args[0]);
    cfg.setOutput(args[1]);
    cfg.setIndexed(pars.isSet("indexed"));
    cfg.setColors(pars.value("colors").toInt());
    cfg.setDpi(pars.value("dpi").toUInt());
}

void Application::run()
{
    // check command line validity
    if (initFailed) {
        qApp->exit(1);
        return;
    }

    // load input
    QFile src(cfg.input());
    if (!src.open(QIODevice::ReadOnly)) {
        qCritical() << "Cannot open" << cfg.input() << ":" << src.errorString();
        qApp->exit(1);
        return;
    }

    auto mapping = src.map(0, src.size());
    Q_ASSERT(mapping);
    QByteArray buf(reinterpret_cast<char *>(mapping), int(src.size()));

    QBuffer buffer(&buf);
    QImageReader rd(&buffer);
    if (!rd.canRead()) {
        qCritical() << "Cannot read" << cfg.input() << ":" << rd.errorString();
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

    // get file DPI if TIFF
    auto sourceDPI = TIFFReader::dpi(buf);

    // process individual pages
    for (auto pgCntr = rd.imageCount(); pgCntr > 0; --pgCntr, rd.jumpToNextImage()) {
        QImage img = rd.read();

        if (img.isNull()) {
            qCritical() << "Loading input page" << rd.currentImageNumber() <<
                        "failed:" << rd.errorString();
            qApp->exit(1);
            return;
        }

        // rescale
        if (sourceDPI != 0 && cfg.dpi() != 0 && sourceDPI != cfg.dpi()) {
            qDebug() << "rescaling to" << cfg.dpi() << "DPI";
            img = img.scaledToHeight(int(uint(img.height()) / sourceDPI * cfg.dpi()),
                                     Qt::SmoothTransformation);
        }

        // convert image
        Image cvImg = ImageOptimizer::reduceColors(img, cfg.getColors(), cfg.getIndexed());

        qDebug() << Q_FUNC_INFO << "add page";
        pdf.addPage(cvImg);
    }

    // finish PDF
    qDebug() << Q_FUNC_INFO << "finish pdf";
    pdf.finish();

    qApp->quit();
}
