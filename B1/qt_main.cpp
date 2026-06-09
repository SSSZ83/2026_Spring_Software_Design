#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Qt 5 中设置 UTF-8 文本编解码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // 应用全局字体（确保中文正常显示）
    QFont font("Microsoft YaHei", 10);
    app.setFont(font);

    MainWindow w;
    w.show();

    return app.exec();
}
