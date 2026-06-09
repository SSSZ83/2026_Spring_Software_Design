#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <windows.h>
#include "callist.h"
#include "charge.h"
#include "user.h"

using namespace std;

// ---- 全局状态 ----
vector<callist> calls;        // 原始话单（用于话单查询）
vector<charge>  charges;      // 费用记录（用于话费查询）
map<string, float> rates;     // 区号 -> 费率
vector<user>    users;        // 用户列表
bool feeCalculated = false;   // 费用是否已计算

// ---- 天数格式化 ----
void formatDuration(int seconds, ostream& out) {
    int mins = seconds / 60;
    int secs = seconds % 60;
    if (mins > 0)
        out << mins << "分" << secs << "秒";
    else
        out << secs << "秒";
}

// ---- 加载费率文件 ----
bool loadRates() {
    ifstream in("fl.dat");
    if (!in.is_open()) return false;

    rates.clear();
    string area;
    float rate;
    while (in >> area >> rate) {
        rates[area] = rate;
    }
    in.close();
    return true;
}

// ---- 加载用户文件 ----
bool loadUsers() {
    ifstream in("yh.dat");
    if (!in.is_open()) return false;

    users.clear();
    while (!in.eof()) {
        user u;
        u.In(in);
        if (in.fail()) break;
        users.push_back(u);
    }
    in.close();
    return true;
}

// ---- 根据电话号码查找用户名 ----
string findUserName(const string& phone) {
    for (const auto& u : users) {
        if (u.GetPhone() == phone) return u.GetName();
    }
    return "未知用户";
}

// ---- 菜单 ----
void showMenu() {
    cout << "\n========================================\n";
    cout << "        模拟电信计费系统\n";
    cout << "========================================\n";
    if (feeCalculated)
        cout << "  状态: 费用已计算 (" << charges.size() << " 条记录)\n";
    else
        cout << "  状态: 费用尚未计算\n";
    cout << "----------------------------------------\n";
    cout << "  1. 计算通话费用\n";
    cout << "  2. 话费查询（按电话号码）\n";
    cout << "  3. 话单查询（按电话号码）\n";
    cout << "  4. 显示全部话单\n";
    cout << "  5. 退出\n";
    cout << "========================================\n";
    cout << "请选择操作 (1-5): ";
}

// ---- 1. 计算通话费用 ----
void calculateFees() {
    cout << "\n===== 计算通话费用 =====\n\n";

    // 加载费率
    cout << "[1/3] 加载费率文件 fl.dat ... ";
    if (!loadRates()) {
        cout << "失败！\n";
        return;
    }
    cout << "OK (" << rates.size() << " 条)\n";

    // 加载用户
    cout << "[2/3] 加载用户文件 yh.dat ... ";
    if (!loadUsers()) {
        cout << "失败！\n";
        return;
    }
    cout << "OK (" << users.size() << " 位)\n";

    // 加载话单
    cout << "[3/3] 读入源数据文件 hd.dat ... ";
    ifstream hdFile("hd.dat");
    if (!hdFile.is_open()) {
        cout << "失败！\n";
        return;
    }

    calls.clear();
    charges.clear();

    while (!hdFile.eof()) {
        callist c;
        c.In(hdFile);
        if (hdFile.fail()) break;
        calls.push_back(c);

        // 从话单构造费用对象
        charge chg;
        chg.SetBaseFromCallist(c);               // 复制基类数据
        chg.Compute(rates);                       // 计算费用
        charges.push_back(chg);
    }
    hdFile.close();
    cout << "OK (" << calls.size() << " 条)\n";

    // 写入费用文件 fy.dat
    ofstream fyFile("fy.dat");
    if (!fyFile.is_open()) {
        cout << "警告：无法创建费用文件 fy.dat。\n";
    } else {
        fyFile << charges.size() << "\n";
        for (auto& chg : charges) {
            chg.Out(fyFile);
            fyFile << "\n";
        }
        fyFile.close();
        cout << "\n结果已保存到 fy.dat。\n";
    }

    feeCalculated = true;

    // 显示费用明细
    cout << "\n---------- 费用明细 ----------\n";
    cout << "序号  主叫电话    被叫区号  被叫电话    时长      类型  费用\n";
    cout << "---------------------------------------------------------\n";
    for (size_t i = 0; i < charges.size(); i++) {
        cout << setw(3) << (i + 1) << ".";
        charges[i].print();
        cout << "\n";
    }
    cout << "---------------------------------------------------------\n";
    cout << "共 " << charges.size() << " 条记录。\n";
}

// ---- 2. 话费查询 ----
void queryFee() {
    if (!feeCalculated) {
        cout << "\n费用尚未计算，正在自动计算...\n";
        calculateFees();
    }

    cout << "\n请输入要查询的电话号码: ";
    string phone;
    cin >> phone;
    cin.ignore(10000, '\n');

    // 查找用户名
    string name = findUserName(phone);

    // 统计该号码的本地话费和长途话费
    float localFee = 0.0f, longFee = 0.0f;
    for (const auto& chg : charges) {
        if (chg.GetSrcPhone() == phone) {
            if (chg.GetCallType() == '0')
                localFee += chg.GetFee();
            else
                longFee += chg.GetFee();
        }
    }

    cout << "\n========================================\n";
    cout << "          话费查询结果\n";
    cout << "========================================\n";
    cout << "  用户名      电话号码     本地话费   长途话费   话费总计\n";
    cout << "--------------------------------------------------------\n";
    cout << "  " << setw(8) << setfill(' ') << left << name
         << "  " << setw(15) << phone
         << "  " << fixed << setprecision(2)
         << setw(8) << localFee
         << "  " << setw(8) << longFee
         << "  " << setw(8) << (localFee + longFee)
         << "\n";
    cout << "========================================\n";
}

// ---- 3. 话单查询 ----
void queryCalls() {
    if (calls.empty()) {
        cout << "\n话单数据未加载，正在自动计算费用...\n";
        calculateFees();
    }

    cout << "\n请输入要查询的电话号码: ";
    string phone;
    cin >> phone;
    cin.ignore(10000, '\n');

    string name = findUserName(phone);

    cout << "\n========================================\n";
    cout << "  用户名: " << name << "    电话号码: " << phone << "\n";
    cout << "========================================\n";
    cout << "  序号  主叫区号  主叫电话    被叫区号  被叫电话      通话时长\n";
    cout << "------------------------------------------------------------\n";

    int count = 0;
    for (size_t i = 0; i < calls.size(); i++) {
        if (calls[i].GetSrcPhone() == phone) {
            count++;
            cout << "  " << setw(3) << count << ".";
            calls[i].print();
            cout << "\n";
        }
    }

    if (count == 0) {
        cout << "  （该号码没有主叫通话记录）\n";
    } else {
        cout << "------------------------------------------------------------\n";
        cout << "  共 " << count << " 条通话记录。\n";
    }
    cout << "========================================\n";
}

// ---- 4. 显示全部话单 ----
void displayAll() {
    if (calls.empty()) {
        cout << "\n话单数据未加载，正在自动计算费用...\n";
        calculateFees();
    }

    cout << "\n========================================\n";
    cout << "           全部通话记录\n";
    cout << "========================================\n";
    cout << "  序号  主叫区号  主叫电话    被叫区号  被叫电话      通话时长\n";
    cout << "------------------------------------------------------------\n";

    for (size_t i = 0; i < calls.size(); i++) {
        cout << "  " << setw(3) << (i + 1) << ".";
        calls[i].print();
        cout << "\n";
    }

    cout << "------------------------------------------------------------\n";
    cout << "  共 " << calls.size() << " 条通话记录。\n";
    cout << "========================================\n";
}

// ---- 主函数 ----
int main() {
    SetConsoleOutputCP(CP_UTF8);

    cout << "欢迎使用模拟电信计费系统！\n";

    while (true) {
        showMenu();

        int choice;
        if (!(cin >> choice)) {
            cout << "\n输入无效，请输入数字 1-5。\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: calculateFees(); break;
            case 2: queryFee();      break;
            case 3: queryCalls();    break;
            case 4: displayAll();    break;
            case 5:
                cout << "\n感谢使用，再见！\n";
                return 0;
            default:
                cout << "\n无效选择，请输入 1-5 之间的数字。\n";
                break;
        }
    }

    return 0;
}
