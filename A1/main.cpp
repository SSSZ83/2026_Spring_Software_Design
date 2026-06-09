#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <windows.h>

using namespace std;

// 学生结构体：存储姓名和分数
struct Student {
    string name;
    double score;
};

// 显示菜单
void showMenu() {
    cout << "\n====================================\n";
    cout << "      学生分数统计系统\n";
    cout << "====================================\n";
    cout << "  1. 输入学生信息\n";
    cout << "  2. 按分数降序排列并显示\n";
    cout << "  3. 显示原始数据\n";
    cout << "  4. 退出\n";
    cout << "====================================\n";
    cout << "请选择操作 (1-4): ";
}

// 添加学生信息
void addStudents(vector<Student>& students) {
    cout << "\n--- 输入学生信息 ---\n";
    cout << "（输入姓名为空时结束录入）\n\n";

    while (true) {
        Student stu;
        cout << "请输入第 " << (students.size() + 1) << " 位学生姓名: ";
        getline(cin, stu.name);

        // 姓名为空则结束录入
        if (stu.name.empty()) {
            break;
        }

        cout << "请输入 " << stu.name << " 的分数: ";
        while (!(cin >> stu.score)) {
            cout << "分数输入无效，请重新输入数字: ";
            cin.clear();
            cin.ignore(10000, '\n');
        }
        cin.ignore(10000, '\n'); // 清除输入缓冲区

        students.push_back(stu);
        cout << "  -> 已录入: " << stu.name << " (" << stu.score << " 分)\n\n";
    }

    cout << "\n共录入 " << students.size() << " 名学生信息。\n";
}

// 降序排序（分数从高到低，分数相同时按姓名升序）
void sortByScore(vector<Student>& students) {
    sort(students.begin(), students.end(),
        [](const Student& a, const Student& b) {
            if (a.score != b.score) {
                return a.score > b.score; // 分数降序
            }
            return a.name < b.name;       // 分数相同，姓名升序
        });
}

// 显示学生列表
void displayStudents(const vector<Student>& students, bool sorted) {
    cout << "\n--- ";
    if (sorted) {
        cout << "分数降序排列";
    } else {
        cout << "原始录入顺序";
    }
    cout << " ---\n\n";

    if (students.empty()) {
        cout << "  （暂无学生信息）\n";
        return;
    }

    // 格式化表格头
    cout << "  " << left << setw(6)  << "排名"
         << setw(16) << "姓名"
         << right << setw(8) << "分数" << "\n";
    cout << "  " << string(30, '-') << "\n";

    for (size_t i = 0; i < students.size(); ++i) {
        cout << "  " << left  << setw(6)  << (i + 1)
             << setw(16) << students[i].name
             << right << setw(8) << fixed << setprecision(1) << students[i].score << "\n";
    }
    cout << "\n  共 " << students.size() << " 人\n";
}

int main() {
    vector<Student> students;       // 原始数据
    vector<Student> sortedStudents; // 排序后数据
    bool isSorted = false;          // 排序数据是否最新
    int choice;

    // 设置控制台输出编码为 UTF-8，防止中文乱码
    SetConsoleOutputCP(CP_UTF8);

    cout << "欢迎使用学生分数统计系统！\n";

    while (true) {
        showMenu();

        if (!(cin >> choice)) {
            cout << "\n输入无效，请输入数字 1-4。\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1:
                addStudents(students);
                isSorted = false; // 数据更新，排序置为过期
                break;

            case 2:
                if (students.empty()) {
                    cout << "\n请先录入学生信息！\n";
                } else {
                    sortedStudents = students;
                    sortByScore(sortedStudents);
                    isSorted = true;
                    displayStudents(sortedStudents, true);
                }
                break;

            case 3:
                if (students.empty()) {
                    cout << "\n请先录入学生信息！\n";
                } else {
                    displayStudents(students, false);
                }
                break;

            case 4:
                cout << "\n感谢使用，再见！\n";
                return 0;

            default:
                cout << "\n无效选择，请输入 1-4 之间的数字。\n";
                break;
        }
    }

    return 0;
}
