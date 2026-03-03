#include "csproject.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "xmlmanager.h"
#include <QDebug>
bool loadStyleSheet(QWidget *, const QString &);
void loadGlobalQss(const QString &);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CSPro_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    // 1. 获取xmlmanager实例（引用），初始化配置
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    if (!xml_mgr.init("config/app_config.xml")) { // 资源路径更推荐
        qCritical() << "配置初始化失败，程序退出";
        return -1;
    }
    CSproject w;

    w.show();
    return a.exec();
}


