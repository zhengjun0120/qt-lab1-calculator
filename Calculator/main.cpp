#include "calculator.h"

#include <QApplication>
#include <QStyleHints>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 界面配色固定为浅色，避免系统深色模式下控件配色与样式表冲突
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);

    Calculator window;
    window.show();

    return app.exec();
}
