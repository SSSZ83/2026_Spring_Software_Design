#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <windows.h>

using namespace std;

// 文本编辑器状态
vector<string> lines;      // 按行存储的文本内容
string currentFile;        // 当前文件路径（空表示未关联文件）
bool modified = false;     // 内容是否已修改

// 显示菜单
void showMenu() {
    cout << "\n====================================\n";
    cout << "          文本编辑器\n";
    cout << "====================================\n";
    cout << "  当前文件: ";
    if (currentFile.empty()) {
        cout << "(未命名)";
    } else {
        cout << currentFile;
    }
    if (modified) cout << " [已修改]";
    cout << "\n";
    cout << "------------------------------------\n";
    cout << "  1. 新建文本\n";
    cout << "  2. 可视编辑模式\n";
    cout << "  3. 打开文件\n";
    cout << "  4. 保存文件\n";
    cout << "  5. 显示内容\n";
    cout << "  6. 退出\n";
    cout << "====================================\n";
    cout << "请选择操作 (1-6): ";
}

// 新建文本（清空当前内容）
void newText() {
    if (modified && !lines.empty()) {
        cout << "\n当前内容尚未保存，确定新建吗？(y/n): ";
        char confirm;
        cin >> confirm;
        cin.ignore(10000, '\n');
        if (confirm != 'y' && confirm != 'Y') {
            cout << "已取消。\n";
            return;
        }
    }
    lines.clear();
    currentFile.clear();
    modified = false;
    cout << "\n已创建空白文档。\n";
}

// 读取安全整数
int readInt(const string& prompt) {
    while (true) {
        cout << prompt;
        int num;
        if (cin >> num) {
            cin.ignore(10000, '\n');
            return num;
        }
        cout << "输入无效，请输入数字。\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// 显示带行号的编辑区
void showEditorView() {
    cout << "\n";
    if (lines.empty()) {
        cout << "  （文档为空）\n";
        return;
    }
    for (size_t i = 0; i < lines.size(); ++i) {
        cout << setw(4) << right << (i + 1) << " | " << lines[i] << "\n";
    }
}

// 可视编辑模式
void editText() {
    cout << "\n================== 可视编辑模式 ==================\n";
    cout << "  e <行号>  编辑指定行     例: e 3\n";
    cout << "  i <行号>  在指定行前插入 例: i 2\n";
    cout << "  d <行号>  删除指定行     例: d 5\n";
    cout << "  a         末尾追加一行\n";
    cout << "  s         完成编辑，返回主菜单\n";
    cout << "  h         重新显示帮助\n";
    cout << "==================================================\n";

    while (true) {
        // 显示当前内容
        showEditorView();
        cout << "\n[" << lines.size() << " 行] 请输入命令: ";

        string cmd;
        getline(cin, cmd);

        // 去除前后空格
        while (!cmd.empty() && cmd.front() == ' ') cmd.erase(0, 1);
        while (!cmd.empty() && cmd.back() == ' ') cmd.pop_back();

        if (cmd.empty()) {
            continue;
        }

        char op = cmd[0];
        if (op >= 'A' && op <= 'Z') op = tolower(op); // 兼容大小写

        // 提取行号参数（从第 2 个字符开始）
        string param = (cmd.size() > 1) ? cmd.substr(1) : "";
        while (!param.empty() && param.front() == ' ') param.erase(0, 1);

        if (op == 's') {
            cout << "退出编辑模式。\n";
            return;
        }

        if (op == 'h') {
            cout << "\n  e <行号> 编辑 | i <行号> 插入 | d <行号> 删除 | a 追加 | s 完成\n";
            continue;
        }

        if (op == 'a') {
            cout << "请输入新行内容: ";
            string newLine;
            getline(cin, newLine);
            lines.push_back(newLine);
            modified = true;
            cout << "已在末尾追加第 " << lines.size() << " 行。\n";
            continue;
        }

        // e / i / d 需要行号
        if (param.empty()) {
            cout << "错误：'" << op << "' 命令需要行号参数，如: " << op << " 3\n";
            continue;
        }

        int lineNo = 0;
        try { lineNo = stoi(param); }
        catch (...) {
            cout << "错误：'" << param << "' 不是有效的行号。\n";
            continue;
        }

        if (op == 'e') {
            // 编辑指定行
            if (lineNo < 1 || lineNo > (int)lines.size()) {
                cout << "错误：行号 " << lineNo << " 超出范围 (1-" << lines.size() << ")。\n";
                continue;
            }
            cout << "第 " << lineNo << " 行原文: " << lines[lineNo - 1] << "\n";
            cout << "请输入新内容: ";
            string newContent;
            getline(cin, newContent);
            if (!newContent.empty() || true) { // 允许清空为空白行
                lines[lineNo - 1] = newContent;
                modified = true;
                cout << "第 " << lineNo << " 行已更新。\n";
            }
        }
        else if (op == 'i') {
            // 在指定行前插入
            if (lineNo < 1 || lineNo > (int)lines.size() + 1) {
                cout << "错误：行号 " << lineNo << " 超出范围 (1-" << (lines.size() + 1) << ")。\n";
                continue;
            }
            cout << "将在第 " << lineNo << " 行前插入，请输入内容: ";
            string newContent;
            getline(cin, newContent);
            lines.insert(lines.begin() + (lineNo - 1), newContent);
            modified = true;
            cout << "已在第 " << lineNo << " 行前插入。\n";
        }
        else if (op == 'd') {
            // 删除指定行
            if (lineNo < 1 || lineNo > (int)lines.size()) {
                cout << "错误：行号 " << lineNo << " 超出范围 (1-" << lines.size() << ")。\n";
                continue;
            }
            cout << "确认删除第 " << lineNo << " 行: " << lines[lineNo - 1] << " (y/n): ";
            char confirm;
            cin >> confirm;
            cin.ignore(10000, '\n');
            if (confirm == 'y' || confirm == 'Y') {
                lines.erase(lines.begin() + (lineNo - 1));
                modified = true;
                cout << "第 " << lineNo << " 行已删除。\n";
            } else {
                cout << "已取消。\n";
            }
        }
        else {
            cout << "未知命令 '" << op << "'，输入 h 查看帮助。\n";
        }
    }
}

// 打开文件
void openFile() {
    if (modified && !lines.empty()) {
        cout << "\n当前内容尚未保存，确定打开其他文件吗？(y/n): ";
        char confirm;
        cin >> confirm;
        cin.ignore(10000, '\n');
        if (confirm != 'y' && confirm != 'Y') {
            cout << "已取消。\n";
            return;
        }
    }

    cout << "\n请输入文件路径: ";
    string path;
    getline(cin, path);

    if (path.empty()) {
        cout << "路径不能为空。\n";
        return;
    }

    ifstream inFile(path);
    if (!inFile.is_open()) {
        cout << "无法打开文件: " << path << "\n";
        cout << "请检查文件是否存在且可读。\n";
        return;
    }

    lines.clear();
    string line;
    int count = 0;
    while (getline(inFile, line)) {
        lines.push_back(line);
        count++;
    }
    inFile.close();

    currentFile = path;
    modified = false;
    cout << "已读取 " << count << " 行。\n";
}

// 保存文件
void saveFile() {
    string savePath = currentFile;

    if (savePath.empty()) {
        cout << "\n请输入保存路径: ";
        getline(cin, savePath);
        if (savePath.empty()) {
            cout << "路径不能为空，保存取消。\n";
            return;
        }
    } else {
        cout << "\n当前文件: " << savePath << "\n";
        cout << "直接回车使用原路径保存，或输入新路径另存为: ";
        string newPath;
        getline(cin, newPath);
        if (!newPath.empty()) {
            savePath = newPath;
        }
    }

    ofstream outFile(savePath);
    if (!outFile.is_open()) {
        cout << "无法写入文件: " << savePath << "\n";
        cout << "请检查路径是否有效。\n";
        return;
    }

    for (size_t i = 0; i < lines.size(); ++i) {
        outFile << lines[i];
        if (i < lines.size() - 1) {
            outFile << "\n";
        }
    }
    outFile.close();

    currentFile = savePath;
    modified = false;
    cout << "已保存 " << lines.size() << " 行到: " << savePath << "\n";
}

// 显示当前内容（带行号）
void displayContent() {
    cout << "\n--- 当前内容 ---\n\n";

    if (lines.empty()) {
        cout << "  （文档为空）\n";
        return;
    }

    for (size_t i = 0; i < lines.size(); ++i) {
        cout << setw(4) << right << (i + 1) << " | " << lines[i] << "\n";
    }
    cout << "\n共 " << lines.size() << " 行。\n";
}

// 退出前检查
bool confirmExit() {
    if (modified && !lines.empty()) {
        cout << "\n当前内容有未保存的修改，确定退出吗？(y/n): ";
        char confirm;
        cin >> confirm;
        cin.ignore(10000, '\n');
        return (confirm == 'y' || confirm == 'Y');
    }
    return true;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    cout << "欢迎使用文本编辑器！\n";
    cout << "（支持可视编辑、打开/保存文本文件）\n";

    while (true) {
        showMenu();

        int choice;
        if (!(cin >> choice)) {
            cout << "\n输入无效，请输入数字 1-6。\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: newText();    break;
            case 2: editText();   break;
            case 3: openFile();   break;
            case 4: saveFile();   break;
            case 5: displayContent(); break;
            case 6:
                if (confirmExit()) {
                    cout << "\n感谢使用，再见！\n";
                    return 0;
                }
                break;
            default:
                cout << "\n无效选择，请输入 1-6 之间的数字。\n";
                break;
        }
    }

    return 0;
}
