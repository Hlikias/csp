#include "csproject.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "XmlManager/xmlmanager.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 分配控制台
    AllocConsole();
    // 获取控制台窗口句柄并隐藏
    HWND consoleWnd = GetConsoleWindow();
    ShowWindow(consoleWnd, SW_HIDE);

    // 可选：重定向输出流，使 std::cout 也能输出到控制台
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
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

    a.setWindowIcon(QIcon("pic/icon.ico"));  // 从资源文件加载
    CSproject w;

    w.show();
    return a.exec();
}


