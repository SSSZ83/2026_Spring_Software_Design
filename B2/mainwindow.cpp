#include "mainwindow.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

// ==================== 构造 / 析构 ====================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QString::fromUtf8("模拟电信计费系统"));
    // [布局调整] 窗口大小: 宽度, 高度
    resize(1920, 1080);
    applyStyle();
    setupUI();
    m_statusLabel->setText(QString::fromUtf8("就绪 | 请导入源数据文件"));
}

MainWindow::~MainWindow() {}

// ==================== 样式 ====================
void MainWindow::applyStyle()
{
    setStyleSheet(
        "QMainWindow { background-color: #f5f6fa; }"
        "QMenuBar { font-size: 14px; padding: 2px; }"
        "QToolBar { spacing: 6px; padding: 4px; background: #fff; "
        "           border-bottom: 1px solid #ddd; }"
        "QToolButton { padding: 6px 14px; border-radius: 4px; font-size: 13px; }"
        "QToolButton:hover { background: #e8f0fe; }"
        "QGroupBox { font-weight: bold; font-size: 13px; border: 1px solid #ccc; "
        "            border-radius: 6px; margin-top: 10px; padding-top: 16px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }"
        "QPushButton { padding: 6px 18px; border-radius: 4px; "
        "              background: #3498db; color: white; font-weight: bold; }"
        "QPushButton:hover { background: #2980b9; }"
        "QLineEdit { padding: 6px; border: 1px solid #ccc; border-radius: 4px; font-size: 13px; }"
        "QTableWidget { gridline-color: #ddd; font-size: 12px; }"
        "QTableWidget::item { padding: 4px; }"
        "QHeaderView::section { background: #e8e8e8; font-weight: bold; padding: 4px; }"
    );
}

// ==================== UI 总装 ====================
void MainWindow::setupUI()
{
    setupMenuBar();
    setupToolBar();
    setupCentralWidget();
    setupStatusBar();
}

// ==================== 菜单栏 ====================
void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu(QString::fromUtf8("文件(&F)"));

    QAction *actImport = fileMenu->addAction(QString::fromUtf8("导入数据..."));
    connect(actImport, &QAction::triggered, this, &MainWindow::onImport);

    QAction *actExport = fileMenu->addAction(QString::fromUtf8("导出费用..."));
    connect(actExport, &QAction::triggered, this, &MainWindow::onExport);

    fileMenu->addSeparator();

    QAction *actExit = fileMenu->addAction(QString::fromUtf8("退出(&X)"));
    actExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(actExit, &QAction::triggered, this, &QMainWindow::close);

    QMenu *calcMenu = menuBar()->addMenu(QString::fromUtf8("计算(&C)"));
    QAction *actCalc = calcMenu->addAction(QString::fromUtf8("计算通话费用"));
    actCalc->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(actCalc, &QAction::triggered, this, &MainWindow::onCalculate);

    QMenu *helpMenu = menuBar()->addMenu(QString::fromUtf8("帮助(&H)"));
    QAction *actAbout = helpMenu->addAction(QString::fromUtf8("关于"));
    connect(actAbout, &QAction::triggered, this, &MainWindow::onAbout);
}

// ==================== 工具栏 ====================
void MainWindow::setupToolBar()
{
    QToolBar *toolbar = addToolBar(QString::fromUtf8("主工具栏"));
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(20, 20));

    m_actImport = toolbar->addAction(QString::fromUtf8("📥 导入数据"));
    connect(m_actImport, &QAction::triggered, this, &MainWindow::onImport);

    m_actCalculate = toolbar->addAction(QString::fromUtf8("💰 计算费用"));
    m_actCalculate->setEnabled(false);
    connect(m_actCalculate, &QAction::triggered, this, &MainWindow::onCalculate);

    m_actExport = toolbar->addAction(QString::fromUtf8("📤 导出结果"));
    m_actExport->setEnabled(false);
    connect(m_actExport, &QAction::triggered, this, &MainWindow::onExport);

    toolbar->addSeparator();

    QAction *actExit = toolbar->addAction(QString::fromUtf8("🚪 退出"));
    connect(actExit, &QAction::triggered, this, &QMainWindow::close);
}

// ==================== 中央区域 ====================
void MainWindow::setupCentralWidget()
{
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(2);

    // ===== 左侧：主表格 =====
    m_table = new QTableWidget(0, 8);
    m_table->setHorizontalHeaderLabels({
        QString::fromUtf8("序号"),
        QString::fromUtf8("主叫区号"),
        QString::fromUtf8("主叫电话"),
        QString::fromUtf8("被叫区号"),
        QString::fromUtf8("被叫电话"),
        QString::fromUtf8("时长(秒)"),
        QString::fromUtf8("类型"),
        QString::fromUtf8("费用(元)")
    });
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    // [布局调整] 主表格各列宽度(px): 序号 主叫区号 主叫电话 被叫区号 被叫电话 时长 类型
    //                                      (第8列"费用"自动填满剩余空间)
    m_table->setColumnWidth(0, 130);
    m_table->setColumnWidth(1, 130);
    m_table->setColumnWidth(2, 130);
    m_table->setColumnWidth(3, 130);
    m_table->setColumnWidth(4, 130);
    m_table->setColumnWidth(5, 130);
    m_table->setColumnWidth(6, 130);

    splitter->addWidget(m_table);

    // ===== 右侧：查询 + 统计面板 =====
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    // [布局调整] 右侧面板内边距: 左 上 右 下
    rightLayout->setContentsMargins(8, 8, 8, 8);
    // [布局调整] 查询区与统计区之间的垂直间距
    rightLayout->setSpacing(10);

    // --- 查询区 ---
    QGroupBox *queryGroup = new QGroupBox(QString::fromUtf8("🔍 话费查询"));
    QVBoxLayout *queryLayout = new QVBoxLayout(queryGroup);

    QHBoxLayout *inputRow = new QHBoxLayout();
    QLabel *lbl = new QLabel(QString::fromUtf8("电话号码:"));
    m_queryEdit = new QLineEdit();
    m_queryEdit->setPlaceholderText(QString::fromUtf8("输入电话号码如 3491234"));
    m_queryBtn = new QPushButton(QString::fromUtf8("查询"));
    inputRow->addWidget(lbl);
    inputRow->addWidget(m_queryEdit, 1);
    inputRow->addWidget(m_queryBtn);
    queryLayout->addLayout(inputRow);

    m_queryResult = new QLabel();
    m_queryResult->setWordWrap(true);
    m_queryResult->setStyleSheet(
        "QLabel { background: #fff; border: 1px solid #ddd; border-radius: 4px; "
        "        padding: 10px; font-size: 13px; min-height: 50px; }");
    queryLayout->addWidget(m_queryResult);

    // 查询结果小表格
    m_queryTable = new QTableWidget(0, 5);
    m_queryTable->setHorizontalHeaderLabels({
        QString::fromUtf8("被叫区号"),
        QString::fromUtf8("被叫电话"),
        QString::fromUtf8("时长"),
        QString::fromUtf8("类型"),
        QString::fromUtf8("费用")
    });
    m_queryTable->verticalHeader()->setVisible(false);
    m_queryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // [布局调整] 查询小表格最大高度，超出则出现滚动条
    m_queryTable->setMaximumHeight(600);
    m_queryTable->horizontalHeader()->setStretchLastSection(true);
    // [布局调整] 查询小表格列宽(px): 被叫区号 被叫电话 时长 类型
    //                                (第5列"费用"自动填满剩余空间)
    m_queryTable->setColumnWidth(0, 100);
    m_queryTable->setColumnWidth(1, 100);
    m_queryTable->setColumnWidth(2, 100);
    m_queryTable->setColumnWidth(3, 100);
    queryLayout->addWidget(m_queryTable);

    rightLayout->addWidget(queryGroup);
    connect(m_queryBtn, &QPushButton::clicked, this, &MainWindow::onQuery);
    connect(m_queryEdit, &QLineEdit::returnPressed, this, &MainWindow::onQuery);

    // --- 统计区 ---
    QGroupBox *statsGroup = new QGroupBox(QString::fromUtf8("📊 通话统计"));
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);

    m_totalCallsLabel = new QLabel(QString::fromUtf8("总通话记录: -- 条"));
    m_totalCallsLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    statsLayout->addWidget(m_totalCallsLabel);

    statsLayout->addSpacing(6);

    // 本地通话分布条
    QLabel *localTitle = new QLabel(QString::fromUtf8("本地通话:"));
    statsLayout->addWidget(localTitle);
    m_barLocal = new QProgressBar();
    m_barLocal->setRange(0, 100);
    m_barLocal->setValue(0);
    m_barLocal->setTextVisible(true);
    m_barLocal->setFormat(QString());
    m_barLocal->setStyleSheet(
        "QProgressBar { border: 1px solid #ccc; border-radius: 4px; "
        "text-align: center; height: 18px; } "
        "QProgressBar::chunk { background: #5dade2; border-radius: 3px; }");
    statsLayout->addWidget(m_barLocal);
    m_localPctLabel = new QLabel();
    statsLayout->addWidget(m_localPctLabel);

    statsLayout->addSpacing(4);

    // 长途通话分布条
    QLabel *longTitle = new QLabel(QString::fromUtf8("长途通话:"));
    statsLayout->addWidget(longTitle);
    m_barLong = new QProgressBar();
    m_barLong->setRange(0, 100);
    m_barLong->setValue(0);
    m_barLong->setTextVisible(true);
    m_barLong->setFormat(QString());
    m_barLong->setStyleSheet(
        "QProgressBar { border: 1px solid #ccc; border-radius: 4px; "
        "text-align: center; height: 18px; } "
        "QProgressBar::chunk { background: #f0b27a; border-radius: 3px; }");
    statsLayout->addWidget(m_barLong);
    m_longPctLabel = new QLabel();
    statsLayout->addWidget(m_longPctLabel);

    statsLayout->addSpacing(6);
    m_totalFeeLabel = new QLabel(QString::fromUtf8("总通话费用: -- 元"));
    m_totalFeeLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #c0392b;");
    statsLayout->addWidget(m_totalFeeLabel);

    statsLayout->addStretch();
    rightLayout->addWidget(statsGroup);

    splitter->addWidget(rightPanel);
    // [布局调整] 左右分割比例: 左侧(主表格) : 右侧(查询面板)
    //                               当前 2:1, 调大右侧 → (0,3):(1,2) = 3:2
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);
}

// ==================== 状态栏 ====================
void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(QString::fromUtf8("就绪"));
    statusBar()->addWidget(m_statusLabel, 1);
}

// ==================== 槽：导入数据 ====================
void MainWindow::onImport()
{
    // 加载费率
    {
        std::ifstream in("fl.dat");
        if (!in.is_open()) {
            QMessageBox::warning(this,
                QString::fromUtf8("导入错误"),
                QString::fromUtf8("无法打开费率文件 fl.dat。"));
            return;
        }
        m_rates.clear();
        std::string area; float rate;
        while (in >> area >> rate) m_rates[area] = rate;
        in.close();
    }

    // 加载用户
    {
        std::ifstream in("yh.dat");
        if (!in.is_open()) {
            QMessageBox::warning(this,
                QString::fromUtf8("导入错误"),
                QString::fromUtf8("无法打开用户文件 yh.dat。"));
            return;
        }
        m_users.clear();
        while (!in.eof()) {
            user u; u.In(in);
            if (in.fail()) break;
            m_users.push_back(u);
        }
        in.close();
    }

    // 加载话单
    {
        std::ifstream in("hd.dat");
        if (!in.is_open()) {
            QMessageBox::warning(this,
                QString::fromUtf8("导入错误"),
                QString::fromUtf8("无法打开源数据文件 hd.dat。"));
            return;
        }
        m_calls.clear();
        while (!in.eof()) {
            callist c; c.In(in);
            if (in.fail()) break;
            m_calls.push_back(c);
        }
        in.close();
    }

    m_charges.clear();
    m_calculated = false;

    refreshTable();
    refreshStats();
    m_actCalculate->setEnabled(true);
    m_actExport->setEnabled(false);

    m_statusLabel->setText(QString::fromUtf8(
        "已加载 %1 条话单 | %2 条费率 | %3 位用户 | 费用未计算")
        .arg(m_calls.size()).arg(m_rates.size()).arg(m_users.size()));
}

// ==================== 槽：计算费用 ====================
void MainWindow::onCalculate()
{
    if (m_calls.empty()) {
        QMessageBox::information(this,
            QString::fromUtf8("提示"),
            QString::fromUtf8("请先导入数据。"));
        return;
    }

    if (m_rates.empty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("错误"),
            QString::fromUtf8("费率表为空，请检查 fl.dat。"));
        return;
    }

    m_charges.clear();
    for (const auto& c : m_calls) {
        charge chg;
        chg.SetBaseFromCallist(c);
        chg.Compute(m_rates);
        m_charges.push_back(chg);
    }

    m_calculated = true;
    refreshTable();
    refreshStats();
    m_actExport->setEnabled(true);

    // 自动保存 fy.dat
    std::ofstream out("fy.dat");
    if (out.is_open()) {
        out << m_charges.size() << "\n";
        for (auto& chg : m_charges) {
            chg.Out(out);
            out << "\n";
        }
        out.close();
    }

    m_statusLabel->setText(QString::fromUtf8(
        "费用已计算 ✓ | %1 条记录已保存到 fy.dat").arg(m_charges.size()));
}

// ==================== 槽：导出结果 ====================
void MainWindow::onExport()
{
    if (!m_calculated) {
        QMessageBox::information(this,
            QString::fromUtf8("提示"),
            QString::fromUtf8("请先计算费用。"));
        return;
    }

    QString path = QFileDialog::getSaveFileName(this,
        QString::fromUtf8("导出费用文件"),
        "fy.dat",
        QString::fromUtf8("文本文件 (*.dat *.txt)"));
    if (path.isEmpty()) return;

    std::string spath = path.toStdString();
    std::ofstream out(spath);
    if (!out.is_open()) {
        QMessageBox::warning(this,
            QString::fromUtf8("导出错误"),
            QString::fromUtf8("无法写入文件: %1").arg(path));
        return;
    }

    out << m_charges.size() << "\n";
    for (auto& chg : m_charges) {
        chg.Out(out);
        out << "\n";
    }
    out.close();

    m_statusLabel->setText(QString::fromUtf8("已导出到 %1").arg(path));
}

// ==================== 槽：查询 ====================
void MainWindow::onQuery()
{
    // 如果费用尚未计算，自动触发计算
    if (!m_calculated) {
        if (m_calls.empty()) {
            m_queryResult->setText(QString::fromUtf8("请先导入数据（点击 📥 导入数据）。"));
            return;
        }
        onCalculate();
    }

    QString phone = m_queryEdit->text().trimmed();
    if (phone.isEmpty()) {
        m_queryResult->setText(QString::fromUtf8("请输入电话号码。"));
        return;
    }

    std::string phoneStr = phone.toStdString();

    // 查找用户名
    std::string userName = "未知用户";
    for (const auto& u : m_users) {
        if (u.GetPhone() == phoneStr) {
            userName = u.GetName();
            break;
        }
    }

    // 判断是否有通话记录
    bool hasCalls = false;
    for (const auto& c : m_calls) {
        if (c.GetSrcPhone() == phoneStr) { hasCalls = true; break; }
    }
    if (!hasCalls) {
        m_queryResult->setText(QString::fromUtf8(
            "用户: %1\n电话号码: %2\n\n该号码没有主叫通话记录。")
            .arg(QString::fromStdString(userName)).arg(phone));
        m_queryTable->setRowCount(0);
        return;
    }

    // 统计该号码的话费
    float localFee = 0.0f, longFee = 0.0f;
    int localCnt = 0, longCnt = 0;

    if (m_calculated) {
        for (const auto& chg : m_charges) {
            if (chg.GetSrcPhone() == phoneStr) {
                if (chg.GetCallType() == '0') {
                    localFee += chg.GetFee();
                    localCnt++;
                } else {
                    longFee += chg.GetFee();
                    longCnt++;
                }
            }
        }
    }

    m_queryResult->setText(QString::fromUtf8(
        "用户名: %1    号码: %2\n"
        "本地话费: %3 元 (%4 通)  |  长途话费: %5 元 (%6 通)\n"
        "话费总计: %7 元")
        .arg(QString::fromStdString(userName))
        .arg(phone)
        .arg(localFee, 0, 'f', 2).arg(localCnt)
        .arg(longFee, 0, 'f', 2).arg(longCnt)
        .arg(localFee + longFee, 0, 'f', 2));

    // 填充查询表格
    m_queryTable->setRowCount(0);
    int row = 0;
    for (size_t i = 0; i < m_calls.size(); i++) {
        if (m_calls[i].GetSrcPhone() == phoneStr) {
            m_queryTable->insertRow(row);

            m_queryTable->setItem(row, 0,
                new QTableWidgetItem(QString::fromStdString(m_calls[i].GetDstAreaCode())));
            m_queryTable->setItem(row, 1,
                new QTableWidgetItem(QString::fromStdString(m_calls[i].GetDstPhone())));
            m_queryTable->setItem(row, 2,
                new QTableWidgetItem(QString::number(m_calls[i].GetDuration()) + "s"));

            if (m_calculated && i < m_charges.size()) {
                bool isLocal = (m_charges[i].GetCallType() == '0');
                QTableWidgetItem *typeItem = new QTableWidgetItem(
                    isLocal ? QString::fromUtf8("本地")
                            : QString::fromUtf8("长途"));
                m_queryTable->setItem(row, 3, typeItem);

                QTableWidgetItem *feeItem = new QTableWidgetItem(
                    QString::number(m_charges[i].GetFee(), 'f', 2));
                m_queryTable->setItem(row, 4, feeItem);
            } else {
                m_queryTable->setItem(row, 3,
                    new QTableWidgetItem("-"));
                m_queryTable->setItem(row, 4,
                    new QTableWidgetItem("-"));
            }
            row++;
        }
    }
}

// ==================== 槽：关于 ====================
void MainWindow::onAbout()
{
    QMessageBox::about(this,
        QString::fromUtf8("关于"),
        QString::fromUtf8(
            "模拟电信计费系统 v1.0\n\n"
            "功能:\n"
            "• 导入通话记录并自动计算费用\n"
            "• 本地通话: 3分钟以内0.5元，每3分钟递增0.2元\n"
            "• 长途通话: 费率×通话时长\n"
            "• 按电话号码查询话费和话单\n"
            "• 导出费用到 fy.dat\n\n"
            "Qt 5.9.6 + MSVC"));
}

// ==================== 刷新主表格 ====================
void MainWindow::refreshTable()
{
    m_table->setRowCount(0);
    int total = static_cast<int>(m_calls.size());

    for (int i = 0; i < total; i++) {
        m_table->insertRow(i);
        const callist& c = m_calls[i];

        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));

        auto makeItem = [](const QString& s) {
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setTextAlignment(Qt::AlignCenter);
            return item;
        };

        m_table->setItem(i, 1, makeItem(QString::fromStdString(c.GetSrcAreaCode())));
        m_table->setItem(i, 2, makeItem(QString::fromStdString(c.GetSrcPhone())));
        m_table->setItem(i, 3, makeItem(QString::fromStdString(c.GetDstAreaCode())));
        m_table->setItem(i, 4, makeItem(QString::fromStdString(c.GetDstPhone())));
        m_table->setItem(i, 5, makeItem(QString::number(c.GetDuration())));

        if (m_calculated && i < static_cast<int>(m_charges.size())) {
            const charge& chg = m_charges[i];
            bool isLocal = (chg.GetCallType() == '0');
            m_table->setItem(i, 6, makeItem(isLocal
                ? QString::fromUtf8("本地")
                : QString::fromUtf8("长途")));
            m_table->setItem(i, 7, makeItem(
                QString::number(chg.GetFee(), 'f', 2)));

            // 行颜色
            QColor bg = isLocal ? QColor("#d6eaf8") : QColor("#fdebd0");
            for (int col = 0; col < 8; col++) {
                if (m_table->item(i, col))
                    m_table->item(i, col)->setBackground(bg);
            }
        } else {
            m_table->setItem(i, 6, makeItem("-"));
            m_table->setItem(i, 7, makeItem("-"));
        }
    }
}

// ==================== 刷新统计面板 ====================
void MainWindow::refreshStats()
{
    int total = static_cast<int>(m_calls.size());
    m_totalCallsLabel->setText(
        QString::fromUtf8("总通话记录: %1 条").arg(total));

    if (!m_calculated || m_charges.empty()) {
        m_barLocal->setValue(0);
        m_barLong->setValue(0);
        m_localPctLabel->setText(QString::fromUtf8("-- 通 (--%)"));
        m_longPctLabel->setText(QString::fromUtf8("-- 通 (--%)"));
        m_totalFeeLabel->setText(QString::fromUtf8("总通话费用: 尚未计算"));
        return;
    }

    int localCnt = 0, longCnt = 0;
    float totalFee = 0.0f;

    for (const auto& chg : m_charges) {
        totalFee += chg.GetFee();
        if (chg.GetCallType() == '0') localCnt++;
        else longCnt++;
    }

    // 进度条：最大值填满
    int maxCnt = std::max(localCnt, longCnt);
    int barLocalVal = maxCnt > 0 ? (localCnt * 100 / maxCnt) : 0;
    int barLongVal  = maxCnt > 0 ? (longCnt  * 100 / maxCnt) : 0;

    m_barLocal->setValue(barLocalVal);
    m_barLong->setValue(barLongVal);

    double localPct = total > 0 ? (100.0 * localCnt / total) : 0;
    double longPct  = total > 0 ? (100.0 * longCnt  / total) : 0;

    m_localPctLabel->setText(
        QString("%1 通 (%2%)").arg(localCnt).arg(localPct, 0, 'f', 1));
    m_longPctLabel->setText(
        QString("%1 通 (%2%)").arg(longCnt).arg(longPct, 0, 'f', 1));
    m_totalFeeLabel->setText(
        QString::fromUtf8("总通话费用: %1 元").arg(totalFee, 0, 'f', 2));
}
