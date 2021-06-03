#include "MainWindow.h"
#include "Dexter.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMetaType>

int main(int argc, char *argv[])
{
    // 初始化 ffmpeg
    avdevice_register_all();
    avformat_network_init();

    // 初始化 QT
    QApplication a(argc, argv);

    // 注册参数类型
    qRegisterMetaType<int64_t> ("int64_t");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "QTPlay_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
