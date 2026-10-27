#include "calculator.h"

#include <QApplication>
#include <QFile>
#include <QStyleHints>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 界面配色固定为浅色，避免系统深色模式下控件配色与样式表冲突
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);

    QFile styleFile(QStringLiteral(":/style.qss"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));

    Calculator window;
    window.show();

    return app.exec();
}
