#include "mainwindow.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>

// ==================== 构造 / 析构 ====================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    applyStyle();
    setWindowTitle(QString::fromUtf8("学生成绩核算系统 v1.0"));
    resize(1100, 700);
}

MainWindow::~MainWindow() {}

// ==================== 整体布局 ====================
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
    QMenuBar *mb = menuBar();

    // ---- 文件 ----
    QMenu *fileMenu = mb->addMenu(QString::fromUtf8("文件(&F)"));
    QAction *actImport = fileMenu->addAction(QString::fromUtf8("导入成绩..."));
    actImport->setShortcut(QKeySequence("Ctrl+O"));
    connect(actImport, &QAction::triggered, this, &MainWindow::onImport);

    QAction *actExport = fileMenu->addAction(QString::fromUtf8("导出结果..."));
    actExport->setShortcut(QKeySequence("Ctrl+S"));
    connect(actExport, &QAction::triggered, this, &MainWindow::onExport);

    fileMenu->addSeparator();
    QAction *actQuit = fileMenu->addAction(QString::fromUtf8("退出(&Q)"));
    actQuit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actQuit, &QAction::triggered, this, &QWidget::close);

    // ---- 计算 ----
    QMenu *calcMenu = mb->addMenu(QString::fromUtf8("计算(&C)"));
    QAction *actCalc = calcMenu->addAction(QString::fromUtf8("计算总评成绩"));
    actCalc->setShortcut(QKeySequence("F5"));
    connect(actCalc, &QAction::triggered, this, &MainWindow::onCalculate);

    // ---- 查询 ----
    QMenu *queryMenu = mb->addMenu(QString::fromUtf8("查询(&Q)"));
    QAction *actShowAll = queryMenu->addAction(QString::fromUtf8("显示全部"));
    actShowAll->setShortcut(QKeySequence("Ctrl+R"));
    connect(actShowAll, &QAction::triggered, this, [this]() {
        refreshTable(0);
        QAbstractButton *btn = m_filterGroup ? m_filterGroup->button(0) : nullptr;
        if (btn) btn->setChecked(true);
    });

    // ---- 帮助 ----
    QMenu *helpMenu = mb->addMenu(QString::fromUtf8("帮助(&H)"));
    QAction *actAbout = helpMenu->addAction(QString::fromUtf8("关于..."));
    connect(actAbout, &QAction::triggered, this, &MainWindow::onAbout);
}

// ==================== 工具栏 ====================
void MainWindow::setupToolBar()
{
    QToolBar *tb = addToolBar(QString::fromUtf8("主工具栏"));
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tb->setMovable(false);

    m_actImport = tb->addAction(QString::fromUtf8("📂 导入"));
    connect(m_actImport, &QAction::triggered, this, &MainWindow::onImport);

    m_actCalculate = tb->addAction(QString::fromUtf8("📊 计算总评"));
    m_actCalculate->setEnabled(false);
    connect(m_actCalculate, &QAction::triggered, this, &MainWindow::onCalculate);

    m_actExport = tb->addAction(QString::fromUtf8("💾 导出结果"));
    m_actExport->setEnabled(false);
    connect(m_actExport, &QAction::triggered, this, &MainWindow::onExport);

    tb->addSeparator();

    QAction *actQuit = tb->addAction(QString::fromUtf8("❌ 退出"));
    connect(actQuit, &QAction::triggered, this, &QWidget::close);
}

// ==================== 中央区域 ====================
void MainWindow::setupCentralWidget()
{
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // ===== 左侧: 表格 =====
    m_table = new QTableWidget(0, 7);
    m_table->setHorizontalHeaderLabels({
        QString::fromUtf8("学号"),
        QString::fromUtf8("平时成绩"),
        QString::fromUtf8("期中成绩"),
        QString::fromUtf8("期末成绩"),
        QString::fromUtf8("总评成绩"),
        QString::fromUtf8("等级"),
        QString::fromUtf8("等级描述")
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setMinimumWidth(550);
    splitter->addWidget(m_table);

    // ===== 右侧: 统计面板 =====
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(8, 8, 8, 8);
    rightLayout->setSpacing(10);

    // --- 成绩分布 ---
    QGroupBox *distGroup = new QGroupBox(QString::fromUtf8("成绩分布"));
    QVBoxLayout *distLayout = new QVBoxLayout(distGroup);
    distLayout->setSpacing(6);

    struct {
        QProgressBar** bar;
        QLabel**      label;
        QString       color;
        QString       name;
    } grades[] = {
        { &m_barA, &m_labelA, "#27ae60", QString::fromUtf8("优   A (90-100)") },
        { &m_barB, &m_labelB, "#2ecc71", QString::fromUtf8("良   B (80-89)")  },
        { &m_barC, &m_labelC, "#f39c12", QString::fromUtf8("中   C (70-79)")  },
        { &m_barD, &m_labelD, "#e67e22", QString::fromUtf8("及格 D (60-69)")  },
        { &m_barE, &m_labelE, "#e74c3c", QString::fromUtf8("不及格 E (<60)")   },
    };

    for (auto& g : grades) {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *nameLabel = new QLabel(g.name);
        nameLabel->setFixedWidth(130);
        row->addWidget(nameLabel);

        *g.bar = new QProgressBar();
        (*g.bar)->setRange(0, 100);
        (*g.bar)->setValue(0);
        (*g.bar)->setTextVisible(true);
        (*g.bar)->setFormat(QString());
        (*g.bar)->setStyleSheet(
            QString("QProgressBar { border: 1px solid #ccc; border-radius: 4px; "
                    "text-align: center; height: 18px; } "
                    "QProgressBar::chunk { background-color: %1; border-radius: 3px; }")
                .arg(g.color));
        row->addWidget(*g.bar);

        *g.label = new QLabel(QString::fromUtf8("0 人 (0.0%)"));
        (*g.label)->setFixedWidth(110);
        row->addWidget(*g.label);

        distLayout->addLayout(row);
    }
    rightLayout->addWidget(distGroup);

    // --- 总体统计 ---
    QGroupBox *summaryGroup = new QGroupBox(QString::fromUtf8("总体统计"));
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryGroup);
    m_totalLabel = new QLabel(QString::fromUtf8("总人数: 0"));
    m_avgLabel   = new QLabel(QString::fromUtf8("总平均分: --"));
    QFont sf = m_totalLabel->font();
    sf.setPointSize(12);
    sf.setBold(true);
    m_totalLabel->setFont(sf);
    m_avgLabel->setFont(sf);
    summaryLayout->addWidget(m_totalLabel);
    summaryLayout->addWidget(m_avgLabel);
    rightLayout->addWidget(summaryGroup);

    // --- 快速筛选 ---
    QGroupBox *filterGrp = new QGroupBox(QString::fromUtf8("快速筛选"));
    QVBoxLayout *filterLayout = new QVBoxLayout(filterGrp);

    m_filterGroup = new QButtonGroup(this);
    QStringList filterNames = {
        QString::fromUtf8("全部"),
        QString::fromUtf8("A - 优 (90-100)"),
        QString::fromUtf8("B - 良 (80-89)"),
        QString::fromUtf8("C - 中 (70-79)"),
        QString::fromUtf8("D - 及格 (60-69)"),
        QString::fromUtf8("E - 不及格 (<60)")
    };

    for (int i = 0; i < filterNames.size(); ++i) {
        QRadioButton *rb = new QRadioButton(filterNames[i]);
        if (i == 0) rb->setChecked(true);
        m_filterGroup->addButton(rb, i);
        filterLayout->addWidget(rb);
    }

    connect(m_filterGroup,
            QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &MainWindow::onFilterChanged);
    rightLayout->addWidget(filterGrp);

    rightLayout->addStretch();
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);
}

// ==================== 状态栏 ====================
void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(QString::fromUtf8("就绪 | 请导入成绩数据 (Ctrl+O)"));
    statusBar()->addWidget(m_statusLabel);
}

// ==================== 样式表 ====================
void MainWindow::applyStyle()
{
    setStyleSheet(QString::fromUtf8(
        "QMainWindow { background-color: #f5f6fa; }"
        "QTableWidget {"
        "  gridline-color: #dcdde1; font-size: 13px;"
        "}"
        "QTableWidget::item:selected {"
        "  background-color: #3498db; color: white;"
        "}"
        "QHeaderView::section {"
        "  background-color: #2c3e50; color: white;"
        "  padding: 6px; font-weight: bold; border: none;"
        "}"
        "QGroupBox {"
        "  font-weight: bold; font-size: 13px;"
        "  border: 1px solid #bdc3c7; border-radius: 6px;"
        "  margin-top: 12px; padding-top: 14px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin; left: 10px; padding: 0 5px;"
        "}"
        "QToolBar {"
        "  background-color: #ecf0f1; border-bottom: 1px solid #bdc3c7;"
        "  spacing: 6px; padding: 4px;"
        "}"
        "QToolBar QToolButton {"
        "  padding: 6px 12px; border: 1px solid #bdc3c7;"
        "  border-radius: 4px; background-color: white; font-size: 13px;"
        "}"
        "QToolBar QToolButton:hover {"
        "  background-color: #3498db; color: white; border-color: #2980b9;"
        "}"
        "QRadioButton { font-size: 13px; padding: 3px; }"
        "QStatusBar { background-color: #ecf0f1; }"
    ));
}

// ==================== 槽: 导入 ====================
void MainWindow::onImport()
{
    QString path = QFileDialog::getOpenFileName(
        this,
        QString::fromUtf8("导入成绩文件"),
        ".",
        QString::fromUtf8("数据文件 (*.dat *.txt);;所有文件 (*)"));
    if (path.isEmpty()) return;

    std::ifstream inFile(path.toLocal8Bit().constData());
    if (!inFile.is_open()) {
        QMessageBox::warning(this,
            QString::fromUtf8("错误"),
            QString::fromUtf8("无法打开文件:\n") + path);
        return;
    }

    int count = 0;
    inFile >> count;
    if (inFile.fail() || count <= 0) {
        QMessageBox::warning(this,
            QString::fromUtf8("格式错误"),
            QString::fromUtf8("文件第一行应为学生人数。"));
        inFile.close();
        return;
    }

    m_students.clear();
    m_students.reserve(count);

    for (int i = 0; i < count; ++i) {
        Marks m;
        m.In(inFile);
        if (inFile.fail()) {
            QMessageBox::warning(this,
                QString::fromUtf8("格式错误"),
                QString::fromUtf8("第 %1 行数据格式不正确，已停止读入。").arg(i + 2));
            break;
        }
        m_students.push_back(m);
    }
    inFile.close();

    m_calculated = false;
    m_actCalculate->setEnabled(true);
    m_actExport->setEnabled(false);

    refreshTable(0);
    refreshStats();
    m_statusLabel->setText(
        QString::fromUtf8("已加载 %1 名学生 | 总评未计算").arg(m_students.size()));
}

// ==================== 槽: 计算 ====================
void MainWindow::onCalculate()
{
    if (m_students.empty()) {
        QMessageBox::information(this,
            QString::fromUtf8("提示"),
            QString::fromUtf8("请先导入成绩数据。"));
        return;
    }

    for (auto &s : m_students) {
        s.ComputeZP();
        s.ComputeDJ();
    }

    m_calculated = true;
    m_actExport->setEnabled(true);

    int currentFilter = m_filterGroup ? m_filterGroup->checkedId() : 0;
    refreshTable(currentFilter);
    refreshStats();
    m_statusLabel->setText(
        QString::fromUtf8("已加载 %1 名学生 | 总评已计算 ✓").arg(m_students.size()));
}

// ==================== 槽: 导出 ====================
void MainWindow::onExport()
{
    if (m_students.empty() || !m_calculated) {
        QMessageBox::information(this,
            QString::fromUtf8("提示"),
            QString::fromUtf8("请先导入数据并计算总评。"));
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this,
        QString::fromUtf8("导出结果文件"),
        "out.dat",
        QString::fromUtf8("数据文件 (*.dat *.txt);;所有文件 (*)"));
    if (path.isEmpty()) return;

    std::ofstream outFile(path.toLocal8Bit().constData());
    if (!outFile.is_open()) {
        QMessageBox::warning(this,
            QString::fromUtf8("错误"),
            QString::fromUtf8("无法写入文件:\n") + path);
        return;
    }

    outFile << m_students.size() << "\n";
    for (auto &s : m_students) {
        s.Out(outFile);
        outFile << "\n";
    }
    outFile.close();

    QMessageBox::information(this,
        QString::fromUtf8("导出成功"),
        QString::fromUtf8("已将 %1 名学生成绩保存到:\n%2")
            .arg(m_students.size()).arg(path));
}

// ==================== 槽: 筛选 ====================
void MainWindow::onFilterChanged(int id)
{
    refreshTable(id);
}

// ==================== 槽: 关于 ====================
void MainWindow::onAbout()
{
    QMessageBox::about(this,
        QString::fromUtf8("关于学生成绩核算系统"),
        QString::fromUtf8(
            "<h3>学生成绩核算系统 v1.0</h3>"
            "<p>基于 <b>Qt 5.9.6</b> 构建</p>"
            "<hr>"
            "<p><b>功能：</b></p>"
            "<ul>"
            "<li>从文件导入学生平时/期中/期末成绩</li>"
            "<li>按 <b>30% + 30% + 40%</b> 计算总评成绩</li>"
            "<li>自动划分 <b>A/B/C/D/E</b> 五个等级</li>"
            "<li>成绩分布统计与可视化</li>"
            "<li>导出结果到 out.dat</li>"
            "</ul>"
            "<p>B23020200 课程设计</p>"));
}

// ==================== 表格刷新 ====================
void MainWindow::refreshTable(int gradeFilter)
{
    // gradeFilter: 0=全部, 1=A, 2=B, 3=C, 4=D, 5=E
    char filterChar = 0;
    if (gradeFilter > 0 && m_calculated) {
        filterChar = 'A' + char(gradeFilter - 1);
    }

    // 收集符合条件的索引
    std::vector<size_t> indices;
    for (size_t i = 0; i < m_students.size(); ++i) {
        if (filterChar == 0 || m_students[i].GetDJ() == filterChar) {
            indices.push_back(i);
        }
    }

    m_table->setRowCount(static_cast<int>(indices.size()));

    auto setCell = [&](int row, int col, const QString &text, const QColor &bg = QColor()) {
        QTableWidgetItem *item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        if (bg.isValid()) item->setBackground(bg);
        m_table->setItem(row, col, item);
    };

    // 等级 → 颜色
    auto gradeColor = [](char dj) -> QColor {
        switch (dj) {
            case 'A': return QColor("#d5f5e3");  // 浅绿
            case 'B': return QColor("#d6eaf8");  // 浅蓝
            case 'C': return QColor("#fef9e7");  // 浅黄
            case 'D': return QColor("#fae5d3");  // 浅橙
            case 'E': return QColor("#fadbd8");  // 浅红
            default:  return QColor();
        }
    };

    auto gradeText = [](char dj) -> QString {
        switch (dj) {
            case 'A': return QString::fromUtf8("优");
            case 'B': return QString::fromUtf8("良");
            case 'C': return QString::fromUtf8("中");
            case 'D': return QString::fromUtf8("及格");
            case 'E': return QString::fromUtf8("不及格");
            default:  return "--";
        }
    };

    for (int row = 0; row < static_cast<int>(indices.size()); ++row) {
        const Marks &s = m_students[indices[row]];
        QColor bg = m_calculated ? gradeColor(s.GetDJ()) : QColor();

        setCell(row, 0, QString("%1").arg(s.GetId(), 6, 10, QChar('0')), bg);
        setCell(row, 1, QString::number(s.GetUsualScore(),   'f', 1), bg);
        setCell(row, 2, QString::number(s.GetMidtermScore(), 'f', 1), bg);
        setCell(row, 3, QString::number(s.GetFinalScore(),   'f', 1), bg);

        if (m_calculated) {
            setCell(row, 4, QString::number(s.GetZP(), 'f', 2), bg);
            setCell(row, 5, QString(QChar(s.GetDJ())), bg);
            setCell(row, 6, gradeText(s.GetDJ()), bg);
        } else {
            setCell(row, 4, "--", bg);
            setCell(row, 5, "--", bg);
            setCell(row, 6, "--", bg);
        }
    }
}

// ==================== 统计刷新 ====================
void MainWindow::refreshStats()
{
    if (!m_calculated || m_students.empty()) {
        m_barA->setValue(0); m_barB->setValue(0);
        m_barC->setValue(0); m_barD->setValue(0);
        m_barE->setValue(0);
        m_labelA->setText(QString::fromUtf8("0 人 (0.0%)"));
        m_labelB->setText(QString::fromUtf8("0 人 (0.0%)"));
        m_labelC->setText(QString::fromUtf8("0 人 (0.0%)"));
        m_labelD->setText(QString::fromUtf8("0 人 (0.0%)"));
        m_labelE->setText(QString::fromUtf8("0 人 (0.0%)"));
        m_totalLabel->setText(QString::fromUtf8("总人数: %1").arg(m_students.size()));
        m_avgLabel->setText(QString::fromUtf8("总平均分: -- (未计算)"));
        return;
    }

    int cnt[5] = {0, 0, 0, 0, 0};
    double sum = 0;

    for (const auto &s : m_students) {
        sum += s.GetZP();
        switch (s.GetDJ()) {
            case 'A': cnt[0]++; break;
            case 'B': cnt[1]++; break;
            case 'C': cnt[2]++; break;
            case 'D': cnt[3]++; break;
            case 'E': cnt[4]++; break;
        }
    }

    int total = static_cast<int>(m_students.size());
    double avg = sum / total;

    // 找到最大人数，最大值填满进度条，其余按比例填充
    int maxCnt = 0;
    for (int i = 0; i < 5; i++) {
        if (cnt[i] > maxCnt) maxCnt = cnt[i];
    }

    auto updateBar = [&](QProgressBar *bar, QLabel *label, int count) {
        int barVal = maxCnt > 0 ? (count * 100 / maxCnt) : 0;
        bar->setValue(barVal);
        double realPct = total > 0 ? (100.0 * count / total) : 0;
        label->setText(QString("%1 人 (%2%)")
            .arg(count)
            .arg(realPct, 0, 'f', 1));
    };

    updateBar(m_barA, m_labelA, cnt[0]);
    updateBar(m_barB, m_labelB, cnt[1]);
    updateBar(m_barC, m_labelC, cnt[2]);
    updateBar(m_barD, m_labelD, cnt[3]);
    updateBar(m_barE, m_labelE, cnt[4]);

    m_totalLabel->setText(QString::fromUtf8("总人数: %1").arg(total));
    m_avgLabel->setText(QString::fromUtf8("总平均分: %1").arg(avg, 0, 'f', 2));
}
