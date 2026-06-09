#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <iomanip>
#include <windows.h>

using namespace std;

// 字符池：大小写字母 + 数字
const string CHAR_POOL = "abcdefghijklmnopqrstuvwxyz"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "0123456789";

// 生成长度、内容随机的字符串
string generateRandomString(int& outLength) {
    // 随机数引擎
    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count()
                       ^ random_device{}());

    // 随机长度：5 ~ 20
    uniform_int_distribution<int> lenDist(5, 20);
    outLength = lenDist(rng);

    // 随机字符
    uniform_int_distribution<int> charDist(0, (int)CHAR_POOL.size() - 1);

    string result;
    result.reserve(outLength);
    for (int i = 0; i < outLength; ++i) {
        result += CHAR_POOL[charDist(rng)];
    }
    return result;
}

// 比较输入与目标，返回正确率
double compareStrings(const string& target, const string& input) {
    size_t len = target.size();
    if (len == 0) return 0.0;

    int correct = 0;
    for (size_t i = 0; i < len; ++i) {
        if (i < input.size() && input[i] == target[i]) {
            correct++;
        }
    }
    return (double)correct / len * 100.0;
}

// 显示对比结果
void showComparison(const string& target, const string& input) {
    size_t len = target.size();

    cout << "\n目标字符串: " << target << "  (长度: " << len << ")\n";
    cout << "你的输入:   " << input << "\n";

    // 错误位置标注
    cout << "           ";
    for (size_t i = 0; i < len; ++i) {
        if (i < input.size()) {
            if (input[i] == target[i]) {
                cout << " ";
            } else {
                cout << "^";
            }
        } else {
            cout << "?";
        }
    }

    // 输入过长时提示
    if (input.size() > len) {
        cout << "  (输入超出 " << (input.size() - len) << " 个字符)";
    }

    cout << "\n";

    // 统计错误字符
    int errors = 0;
    for (size_t i = 0; i < len; ++i) {
        if (i < input.size() && input[i] != target[i]) {
            if (errors == 0) {
                cout << "错误详情:\n";
            }
            cout << "  位置 " << (i + 1) << ": 应为 '" << target[i]
                 << "', 输入 '" << input[i] << "'\n";
            errors++;
        }
    }
    size_t missing = (len > input.size()) ? (len - input.size()) : 0;
    if (missing > 0) {
        if (errors == 0) cout << "错误详情:\n";
        cout << "  输入不足 " << missing << " 个字符\n";
    }
}

// 显示菜单
void showMenu() {
    cout << "\n====================================\n";
    cout << "        打字练习系统\n";
    cout << "====================================\n";
    cout << "  1. 开始新练习\n";
    cout << "  2. 显示上一次成绩\n";
    cout << "  3. 退出\n";
    cout << "====================================\n";
    cout << "请选择操作 (1-3): ";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    string lastTarget;      // 上一次的目标字符串
    double lastAccuracy = -1; // 上一次的正确率 (-1 表示无记录)
    int lastLength = 0;

    cout << "欢迎使用打字练习系统！\n";

    while (true) {
        showMenu();

        int choice;
        if (!(cin >> choice)) {
            cout << "\n输入无效，请输入数字 1-3。\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: {
                // 随机生成目标字符串
                int length;
                string target = generateRandomString(length);
                lastTarget = target;
                lastLength = length;

                cout << "\n--- 请准确输入以下字符串 ---\n\n";
                cout << "  >> " << target << " <<\n\n";
                cout << "请输入: ";

                string input;
                getline(cin, input);

                // 计算正确率
                lastAccuracy = compareStrings(target, input);

                // 显示对比结果
                showComparison(target, input);

                // 显示正确率
                cout << "\n★ 正确率: " << fixed << setprecision(1)
                     << lastAccuracy << "%\n";

                if (lastAccuracy == 100.0) {
                    cout << "  完美！太厉害了！\n";
                } else if (lastAccuracy >= 80.0) {
                    cout << "  不错，继续加油！\n";
                } else if (lastAccuracy >= 60.0) {
                    cout << "  还需多多练习。\n";
                } else {
                    cout << "  要更加仔细哦。\n";
                }
                break;
            }

            case 2:
                if (lastAccuracy < 0) {
                    cout << "\n暂无练习记录，请先开始练习。\n";
                } else {
                    cout << "\n--- 上一次练习成绩 ---\n";
                    cout << "  目标字符串: " << lastTarget
                         << " (长度: " << lastLength << ")\n";
                    cout << "  正确率: " << fixed << setprecision(1)
                         << lastAccuracy << "%\n";
                }
                break;

            case 3:
                cout << "\n感谢练习，再见！\n";
                return 0;

            default:
                cout << "\n无效选择，请输入 1-3 之间的数字。\n";
                break;
        }
    }

    return 0;
}
