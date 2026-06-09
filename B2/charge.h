#ifndef CHARGE_H
#define CHARGE_H

#include "callist.h"
#include <map>
#include <fstream>

class charge : public callist {
public:
    static constexpr float LOCAL_BASE     = 0.50f;   // 本地3分钟以内
    static constexpr float LOCAL_INCR     = 0.20f;   // 每3分钟递增
    static constexpr int   LOCAL_BASE_MIN = 3;       // 基础分钟数
    static constexpr int   LOCAL_STEP_MIN = 3;       // 递增步长(分钟)

private:
    char  callType;   // 通话类型 '0'=本地 '1'=长途
    float fee;         // 通话费用

public:
    charge();
    virtual ~charge();

    // 从话单对象复制基类数据（用于从 callist 构建 charge）
    void SetBaseFromCallist(const callist& c);

    // 计算通话类型和费用(需要费率表)
    void Compute(const std::map<std::string, float>& rates);

    // 从费用文件(fy.dat)读入
    void In(std::ifstream& in);

    // 向费用文件(fy.dat)输出
    void Out(std::ofstream& out);

    // 屏幕显示费用
    void print() const;

    // ---- getters ----
    char  GetCallType() const;
    float GetFee()      const;
};

#endif
