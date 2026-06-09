#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <bitset>
#include <windows.h>

using namespace std;

// 将 unsigned long long 转为二进制字符串
string toBinary(unsigned long long n) {
    if (n == 0) return "0";

    string result;
    while (n > 0) {
        result = (char)('0' + (n & 1)) + result;
        n >>= 1;
    }
    return result;
}

// 显示菜单
void showMenu() {
    cout << "\n====================================\n";
    cout << "          进制转换器\n";
    cout << "====================================\n";
    cout << "  请选择输入数据的进制:\n";
    cout << "    1. 二进制  (BIN)\n";
    cout << "    2. 八进制  (OCT)\n";
    cout << "    3. 十进制  (DEC)\n";
    cout << "    4. 十六进制 (HEX)\n";
    cout << "    5. 退出\n";
    cout << "====================================\n";
    cout << "请选择操作 (1-5): ";
}

// 验证输入字符串是否合法
bool isValidInput(const string& str, int base) {
    if (str.empty()) return false;

    for (char c : str) {
        bool valid = false;
        switch (base) {
            case 2:
                valid = (c == '0' || c == '1');
                break;
            case 8:
                valid = (c >= '0' && c <= '7');
                break;
            case 10:
                valid = (c >= '0' && c <= '9');
                break;
            case 16:
                valid = (c >= '0' && c <= '9')
                     || (c >= 'A' && c <= 'F')
                     || (c >= 'a' && c <= 'f');
                break;
        }
        if (!valid) {
            cout << "错误：字符 '" << c << "' 在"
                 << base << " 进制中不合法。\n";
            return false;
        }
    }
    return true;
}

// 进制名称
const char* baseName(int base) {
    switch (base) {
        case 2:  return "二进制";
        case 8:  return "八进制";
        case 10: return "十进制";
        case 16: return "十六进制";
        default: return "未知";
    }
}

// 显示转换结果
void showConversion(unsigned long long value, const string& input, int srcBase) {
    cout << "\n========================================\n";
    cout << "               转换结果\n";
    cout << "========================================\n";
    cout << "  输入: " << input << " (" << baseName(srcBase) << ")\n";
    cout << "----------------------------------------\n";
    cout << "  二进制   (BIN): " << toBinary(value) << "\n";
    cout << "  八进制   (OCT): " << oct << value << "\n";
    cout << "  十进制   (DEC): " << dec << value << "\n";
    cout << "  十六进制 (HEX): " << uppercase << hex << value << "\n";
    cout << "========================================\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    cout << "欢迎使用进制转换器！\n";
    cout << "（支持二进制、八进制、十进制、十六进制互转）\n";

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

        if (choice == 5) {
            cout << "\n感谢使用，再见！\n";
            return 0;
        }

        if (choice < 1 || choice > 4) {
            cout << "\n无效选择，请输入 1-5 之间的数字。\n";
            continue;
        }

        // 确定进制
        int base;
        switch (choice) {
            case 1: base = 2;  break;
            case 2: base = 8;  break;
            case 3: base = 10; break;
            case 4: base = 16; break;
            default: continue;
        }

        cout << "\n请输入 " << baseName(base) << " 数: ";
        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "输入不能为空。\n";
            continue;
        }

        // 验证输入
        if (!isValidInput(input, base)) {
            continue;
        }

        // 转换
        try {
            unsigned long long value = stoull(input, nullptr, base);
            showConversion(value, input, base);
        } catch (const out_of_range&) {
            cout << "错误：输入数值过大，超出转换范围。\n";
        } catch (const invalid_argument&) {
            cout << "错误：输入格式不合法。\n";
        }
    }

    return 0;
}
