#ifndef USER_H
#define USER_H

#include <string>
#include <fstream>

class user {
private:
    std::string name;         // 用户姓名
    std::string phoneNumber;  // 电话号码

public:
    user();
    user(std::string phone, std::string name);
    ~user();

    // 从用户文件(yh.dat)读入
    void In(std::ifstream& in);

    // 屏幕显示用户名和电话号码
    void print() const;

    // ---- setters / getters ----
    void SetPhone(std::string phone);
    void SetName(std::string name);

    std::string GetPhone() const;
    std::string GetName()  const;
};

#endif
