#ifndef CALLIST_H
#define CALLIST_H

#include <string>
#include <fstream>

class callist {
protected:
    std::string srcAreaCode;   // 主叫区号
    std::string srcPhone;      // 主叫电话号码
    std::string dstAreaCode;   // 被叫区号
    std::string dstPhone;      // 被叫电话号码
    int         duration;      // 通话时长(秒)

public:
    callist();
    virtual ~callist();

    // 从源数据文件(hd.dat)读入一条通话记录
    void In(std::ifstream& in);

    // 屏幕显示一次通话数据
    void print() const;

    // ---- getters ----
    std::string GetSrcAreaCode() const;
    std::string GetSrcPhone()    const;
    std::string GetDstAreaCode() const;
    std::string GetDstPhone()    const;
    int         GetDuration()    const;
};

#endif
