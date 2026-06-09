# ============================================
# B1 学生成绩核算系统 - Qt GUI 项目文件
# Qt 5.9.6 + MSVC 2017/2019+
# ============================================

QT       += core gui widgets
TARGET    = grade_system_qt
TEMPLATE  = app

# 编译选项：UTF-8 源文件 + C++17 + 异常处理
QMAKE_CXXFLAGS += /utf-8 /EHsc /std:c++17

# 使用 release 库（Anaconda Qt 仅提供 release 版本）
# 保留 /Zi 以生成调试符号供 VS Code 使用
CONFIG += release
QMAKE_CXXFLAGS += /Zi
QMAKE_LFLAGS   += /DEBUG

# 源文件
SOURCES += \
    qt_main.cpp \
    mainwindow.cpp \
    student.cpp \
    marks.cpp

HEADERS += \
    mainwindow.h \
    student.h \
    marks.h

# 目标输出目录
DESTDIR = .
