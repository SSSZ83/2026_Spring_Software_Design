#include <iostream>
#include <string>
#include <iomanip>
#include <windows.h>

using namespace std;

// 显示菜单
void showMenu() {
    cout << "\n====================================\n";
    cout << "        加密 / 解密系统\n";
    cout << "====================================\n";
    cout << "  1. 加密明文\n";
    cout << "  2. 解密密文\n";
    cout << "  3. 退出\n";
    cout << "====================================\n";
    cout << "  公式: Ci = mi + K  (加密)\n";
    cout << "        mi = Ci - K  (解密)\n";
    cout << "====================================\n";
    cout << "请选择操作 (1-3): ";
}

// 加密: Ci = mi + K
string encrypt(const string& plaintext, int key) {
    string ciphertext;
    ciphertext.reserve(plaintext.size());
    for (char c : plaintext) {
        ciphertext += (char)(c + key);
    }
    return ciphertext;
}

// 解密: mi = Ci - K
string decrypt(const string& ciphertext, int key) {
    string plaintext;
    plaintext.reserve(ciphertext.size());
    for (char c : ciphertext) {
        plaintext += (char)(c - key);
    }
    return plaintext;
}

// 以十六进制显示字符串（用于不可打印字符）
void showHex(const string& str, const string& label) {
    cout << label << " (十六进制): ";
    for (size_t i = 0; i < str.size(); ++i) {
        cout << hex << uppercase << setw(2) << setfill('0')
             << (int)(unsigned char)str[i] << " ";
    }
    cout << dec << "\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    cout << "欢迎使用加密/解密系统！\n";

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

        if (choice == 3) {
            cout << "\n感谢使用，再见！\n";
            return 0;
        }

        if (choice == 1 || choice == 2) {
            string input;
            int key;

            if (choice == 1) {
                cout << "\n--- 加密操作 ---\n";
                cout << "请输入明文 M: ";
            } else {
                cout << "\n--- 解密操作 ---\n";
                cout << "请输入密文 C: ";
            }
            getline(cin, input);

            if (input.empty()) {
                cout << "输入不能为空。\n";
                continue;
            }

            cout << "请输入密钥 K (整数): ";
            while (!(cin >> key)) {
                cout << "密钥无效，请输入整数: ";
                cin.clear();
                cin.ignore(10000, '\n');
            }
            cin.ignore(10000, '\n');

            // 显示输入信息
            cout << "\n--- 输入信息 ---\n";
            cout << "明文/密文: \"" << input << "\"  (长度 n = " << input.size() << ")\n";
            cout << "密钥 K   : " << key << "\n";

            if (choice == 1) {
                // 加密
                string ciphertext = encrypt(input, key);

                cout << "\n--- 加密计算过程 ---\n";
                cout << "公式: Ci = mi + K\n\n";
                for (size_t i = 0; i < input.size(); ++i) {
                    cout << "  C[" << i << "] = m[" << i << "] + K"
                         << " = '" << input[i] << "'(" << (int)(unsigned char)input[i] << ")"
                         << " + " << key
                         << " = " << (int)(unsigned char)input[i] + key
                         << " -> '" << ciphertext[i] << "'\n";
                }

                cout << "\n--- 加密结果 ---\n";
                cout << "密文 C: \"" << ciphertext << "\"\n";
                showHex(ciphertext, "密文  ");
            } else {
                // 解密
                string plaintext = decrypt(input, key);

                cout << "\n--- 解密计算过程 ---\n";
                cout << "公式: mi = Ci - K\n\n";
                for (size_t i = 0; i < input.size(); ++i) {
                    cout << "  m[" << i << "] = C[" << i << "] - K"
                         << " = '" << input[i] << "'(" << (int)(unsigned char)input[i] << ")"
                         << " - " << key
                         << " = " << (int)(unsigned char)input[i] - key
                         << " -> '" << plaintext[i] << "'\n";
                }

                cout << "\n--- 解密结果 ---\n";
                cout << "明文 M: \"" << plaintext << "\"\n";
                showHex(plaintext, "明文  ");
            }
        } else {
            cout << "\n无效选择，请输入 1-3 之间的数字。\n";
        }
    }

    return 0;
}
