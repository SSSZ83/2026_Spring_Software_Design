#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <windows.h>
#include "marks.h"

using namespace std;

// ---- 全局状态 ----
vector<Marks> students;
bool dataLoaded = false;
bool calculated = false;

// ---- 等级描述 ----
const char* gradeDesc(char dj) {
    switch (dj) {
        case 'A': return "优";
        case 'B': return "良";
        case 'C': return "中";
        case 'D': return "及格";
        case 'E': return "不及格";
        default:  return "未知";
    }
}

// ---- 菜单 ----
void showMenu() {
    cout << "\n========================================\n";
    cout << "        学生成绩核算系统\n";
    cout << "========================================\n";
    if (dataLoaded) {
        cout << "  当前已读入: " << students.size() << " 名学生";
        if (calculated) cout << " [已计算]";
        cout << "\n";
    } else {
        cout << "  当前未读入数据\n";
    }
    cout << "----------------------------------------\n";
    cout << "  1. 从文件读入成绩 (note.dat)\n";
    cout << "  2. 计算总评成绩及等级\n";
    cout << "  3. 显示所有学生成绩\n";
    cout << "  4. 统计各等级人数及百分比\n";
    cout << "  5. 按等级查询学生\n";
    cout << "  6. 保存结果到文件 (out.dat)\n";
    cout << "  7. 退出\n";
    cout << "========================================\n";
    cout << "请选择操作 (1-7): ";
}

// ---- 1. 从文件读入成绩 ----
void loadFromFile() {
    cout << "\n请输入文件路径 (直接回车使用默认 note.dat): ";
    string path;
    getline(cin, path);
    if (path.empty()) {
        path = "note.dat";
    }

    ifstream inFile(path);
    if (!inFile.is_open()) {
        cout << "错误：无法打开文件 \"" << path << "\"。\n";
        cout << "请确认文件存在且可读。\n";
        return;
    }

    int count;
    inFile >> count;
    if (inFile.fail() || count <= 0) {
        cout << "错误：文件格式不正确，第一行应为学生人数。\n";
        inFile.close();
        return;
    }

    students.clear();
    students.reserve(count);

    for (int i = 0; i < count; ++i) {
        Marks m;
        m.In(inFile);
        if (inFile.fail()) {
            cout << "警告：第 " << (i + 2) << " 行数据格式错误，跳过后续。\n";
            break;
        }
        students.push_back(m);
    }
    inFile.close();

    dataLoaded = true;
    calculated = false;
    cout << "成功读入 " << students.size() << " 名学生成绩。\n";
}

// ---- 2. 计算总评成绩及等级 ----
void calculateAll() {
    if (!dataLoaded) {
        cout << "\n请先从文件读入成绩（菜单 1）。\n";
        return;
    }

    for (auto& s : students) {
        s.ComputeZP();
        s.ComputeDJ();
    }

    calculated = true;
    cout << "\n总评成绩及等级计算完成（" << students.size() << " 人）。\n";
    cout << "公式：总评 = 平时×30% + 期中×30% + 期末×40%\n";
}

// ---- 3. 显示所有学生成绩 ----
void displayAll() {
    if (!dataLoaded) {
        cout << "\n请先从文件读入成绩（菜单 1）。\n";
        return;
    }

    if (!calculated) {
        cout << "\n提示：总评尚未计算，将先自动计算。\n";
        calculateAll();
    }

    cout << "\n===================================================\n";
    cout << "               所有学生成绩一览\n";
    cout << "===================================================\n";
    cout << "  学号    平时成绩  期中成绩  期末成绩  总评成绩  等级\n";
    cout << "---------------------------------------------------\n";

    for (const auto& s : students) {
        s.print();
        cout << "\n";
    }

    // 计算并显示总平均
    float sum = 0;
    for (const auto& s : students) sum += s.GetZP();
    float avg = sum / students.size();

    cout << "---------------------------------------------------\n";
    cout << "  共 " << students.size() << " 人，班级总平均成绩: "
         << fixed << setprecision(2) << avg << "\n";
    cout << "===================================================\n";
}

// ---- 4. 统计各等级人数及百分比 ----
void showStatistics() {
    if (!dataLoaded) {
        cout << "\n请先从文件读入成绩（菜单 1）。\n";
        return;
    }

    if (!calculated) {
        cout << "\n提示：总评尚未计算，将先自动计算。\n";
        calculateAll();
    }

    int countA = 0, countB = 0, countC = 0, countD = 0, countE = 0;
    for (const auto& s : students) {
        switch (s.GetDJ()) {
            case 'A': countA++; break;
            case 'B': countB++; break;
            case 'C': countC++; break;
            case 'D': countD++; break;
            case 'E': countE++; break;
        }
    }

    int total = students.size();
    float sum = 0;
    for (const auto& s : students) sum += s.GetZP();
    float avg = sum / total;

    cout << "\n========================================\n";
    cout << "          成绩统计报告\n";
    cout << "========================================\n";
    cout << "  总人数:       " << total << "\n";
    cout << "  总平均成绩:   " << fixed << setprecision(2) << avg << "\n";
    cout << "----------------------------------------\n";
    cout << "  等级    人数    百分比\n";
    cout << "----------------------------------------\n";
    cout << "  优  A:  " << setw(3) << countA
         << " 人  " << setw(5) << fixed << setprecision(1)
         << (100.0 * countA / total) << "%\n";
    cout << "  良  B:  " << setw(3) << countB
         << " 人  " << setw(5) << (100.0 * countB / total) << "%\n";
    cout << "  中  C:  " << setw(3) << countC
         << " 人  " << setw(5) << (100.0 * countC / total) << "%\n";
    cout << "  及格D:  " << setw(3) << countD
         << " 人  " << setw(5) << (100.0 * countD / total) << "%\n";
    cout << "  不及格E: " << setw(3) << countE
         << " 人  " << setw(5) << (100.0 * countE / total) << "%\n";
    cout << "========================================\n";
}

// ---- 5. 按等级查询学生 ----
void queryByGrade() {
    if (!dataLoaded) {
        cout << "\n请先从文件读入成绩（菜单 1）。\n";
        return;
    }

    if (!calculated) {
        cout << "\n提示：总评尚未计算，将先自动计算。\n";
        calculateAll();
    }

    cout << "\n请选择要查询的等级:\n";
    cout << "  A - 优      (90-100)\n";
    cout << "  B - 良      (80-89)\n";
    cout << "  C - 中      (70-79)\n";
    cout << "  D - 及格    (60-69)\n";
    cout << "  E - 不及格  (0-59)\n";
    cout << "请输入等级字母 (A-E): ";

    char choice;
    cin >> choice;
    cin.ignore(10000, '\n');

    if (choice >= 'a' && choice <= 'z') choice = choice - 'a' + 'A';

    if (choice < 'A' || choice > 'E') {
        cout << "无效等级，请输入 A-E。\n";
        return;
    }

    cout << "\n========================================\n";
    cout << "  等级 " << choice << " (" << gradeDesc(choice)
         << ") 的学生\n";
    cout << "========================================\n";
    cout << "  学号      总评成绩\n";
    cout << "----------------------------------------\n";

    int found = 0;
    for (const auto& s : students) {
        if (s.GetDJ() == choice) {
            cout << "  " << setw(6) << setfill('0') << s.GetId()
                 << setfill(' ')
                 << "    " << fixed << setprecision(2) << s.GetZP()
                 << "\n";
            found++;
        }
    }

    if (found == 0) {
        cout << "  （该等级无学生）\n";
    } else {
        cout << "----------------------------------------\n";
        cout << "  共 " << found << " 人。\n";
    }
    cout << "========================================\n";
}

// ---- 6. 保存结果到 out.dat ----
void saveToFile() {
    if (!dataLoaded) {
        cout << "\n请先从文件读入成绩（菜单 1）。\n";
        return;
    }

    if (!calculated) {
        cout << "\n提示：总评尚未计算，将先自动计算。\n";
        calculateAll();
    }

    cout << "\n请输入保存路径 (直接回车使用默认 out.dat): ";
    string path;
    getline(cin, path);
    if (path.empty()) {
        path = "out.dat";
    }

    ofstream outFile(path);
    if (!outFile.is_open()) {
        cout << "错误：无法写入文件 \"" << path << "\"。\n";
        return;
    }

    outFile << students.size() << "\n";
    for (auto& s : students) {
        s.Out(outFile);
        outFile << "\n";
    }
    outFile.close();

    cout << "已保存 " << students.size() << " 名学生成绩到: "
         << path << "\n";
    cout << "格式：学号  总评成绩  等级\n";
}

// ---- 主函数 ----
int main() {
    SetConsoleOutputCP(CP_UTF8);

    cout << "欢迎使用学生成绩核算系统！\n";
    cout << "（支持从文件读入、计算总评、统计等级分布、导出结果）\n";

    while (true) {
        showMenu();

        int choice;
        if (!(cin >> choice)) {
            cout << "\n输入无效，请输入数字 1-7。\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: loadFromFile();   break;
            case 2: calculateAll();   break;
            case 3: displayAll();     break;
            case 4: showStatistics(); break;
            case 5: queryByGrade();   break;
            case 6: saveToFile();     break;
            case 7:
                cout << "\n感谢使用，再见！\n";
                return 0;
            default:
                cout << "\n无效选择，请输入 1-7 之间的数字。\n";
                break;
        }
    }

    return 0;
}
