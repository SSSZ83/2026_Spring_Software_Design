#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QPushButton>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QColor>
#include <QFont>
#include <vector>
#include <string>
#include "marks.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onImport();
    void onCalculate();
    void onExport();
    void onFilterChanged(int id);
    void onAbout();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupCentralWidget();
    void setupStatusBar();
    void refreshTable(int gradeFilter = 0);
    void refreshStats();
    void applyStyle();

    // 数据层（复用现有 Marks 类）
    std::vector<Marks> m_students;
    bool m_calculated = false;

    // ---- 表格 ----
    QTableWidget *m_table = nullptr;

    // ---- 统计面板 ----
    QProgressBar *m_barA = nullptr;
    QProgressBar *m_barB = nullptr;
    QProgressBar *m_barC = nullptr;
    QProgressBar *m_barD = nullptr;
    QProgressBar *m_barE = nullptr;
    QLabel *m_labelA = nullptr;
    QLabel *m_labelB = nullptr;
    QLabel *m_labelC = nullptr;
    QLabel *m_labelD = nullptr;
    QLabel *m_labelE = nullptr;
    QLabel *m_totalLabel = nullptr;
    QLabel *m_avgLabel   = nullptr;

    // ---- 筛选 ----
    QButtonGroup *m_filterGroup = nullptr;

    // ---- 状态栏 ----
    QLabel *m_statusLabel = nullptr;

    // ---- 工具栏按钮 ----
    QAction *m_actImport    = nullptr;
    QAction *m_actCalculate = nullptr;
    QAction *m_actExport    = nullptr;
};

#endif // MAINWINDOW_H
