#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QColor>
#include <QFont>
#include <QTextBrowser>
#include <vector>
#include <map>
#include <string>
#include "callist.h"
#include "charge.h"
#include "user.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onImport();
    void onCalculate();
    void onExport();
    void onQuery();
    void onAbout();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupCentralWidget();
    void setupStatusBar();
    void refreshTable();
    void refreshStats();
    void applyStyle();

    // ---- 数据层 ----
    std::vector<callist>   m_calls;
    std::vector<charge>    m_charges;
    std::map<std::string, float> m_rates;
    std::vector<user>      m_users;
    bool m_calculated = false;

    // ---- 表格 ----
    QTableWidget *m_table = nullptr;

    // ---- 查询面板 ----
    QLineEdit    *m_queryEdit  = nullptr;
    QPushButton  *m_queryBtn   = nullptr;
    QLabel       *m_queryResult = nullptr;
    QTableWidget *m_queryTable  = nullptr;

    // ---- 统计面板 ----
    QLabel *m_totalCallsLabel  = nullptr;
    QLabel *m_localCountLabel  = nullptr;
    QLabel *m_longCountLabel   = nullptr;
    QLabel *m_totalFeeLabel    = nullptr;
    QProgressBar *m_barLocal   = nullptr;
    QProgressBar *m_barLong    = nullptr;
    QLabel *m_localPctLabel    = nullptr;
    QLabel *m_longPctLabel     = nullptr;

    // ---- 状态栏 ----
    QLabel *m_statusLabel = nullptr;

    // ---- 工具栏 ----
    QAction *m_actImport    = nullptr;
    QAction *m_actCalculate = nullptr;
    QAction *m_actExport    = nullptr;
};

#endif // MAINWINDOW_H
