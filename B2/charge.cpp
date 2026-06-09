#include "charge.h"
#include <iostream>
#include <iomanip>
#include <cmath>

charge::charge()
    : callist(), callType('0'), fee(0.0f) {}

charge::~charge() {}

void charge::SetBaseFromCallist(const callist& c) {
    // 使用 callist 的 public getter 复制基类数据到自身 (protected 成员)
    srcAreaCode = c.GetSrcAreaCode();
    srcPhone    = c.GetSrcPhone();
    dstAreaCode = c.GetDstAreaCode();
    dstPhone    = c.GetDstPhone();
    duration    = c.GetDuration();
}

void charge::Compute(const std::map<std::string, float>& rates) {
    int seconds = GetDuration();
    // 通话时长不满1分钟按1分钟计算
    int minutes = (seconds + 59) / 60;

    if (GetSrcAreaCode() == GetDstAreaCode()) {
        // ---- 本地通话 ----
        callType = '0';
        if (minutes <= LOCAL_BASE_MIN) {
            fee = LOCAL_BASE;
        } else {
            int extraBlocks = (minutes - LOCAL_BASE_MIN + LOCAL_STEP_MIN - 1)
                            / LOCAL_STEP_MIN;
            fee = LOCAL_BASE + extraBlocks * LOCAL_INCR;
        }
    } else {
        // ---- 长途通话 ----
        callType = '1';
        float rate = 0.0f;
        auto it = rates.find(GetDstAreaCode());
        if (it != rates.end()) {
            rate = it->second;
        }
        fee = rate * minutes;
    }
}

void charge::In(std::ifstream& in) {
    // 格式: 主叫电话号码 通话类型 话费金额
    std::string phone;
    char type;
    float f;
    in >> phone >> type >> f;

    // 注意: charge 继承自 callist，但 In 只读取费用相关信息
    // 主叫电话存储到基类（通过 SetId 方式不可用，这里直接存到 srcPhone）
    // 实际上 charge 有自己的 In/Out，与 callist 的 In 独立使用
    callType = type;
    fee = f;
}

void charge::Out(std::ofstream& out) {
    out << std::setfill(' ')
        << GetSrcPhone() << "  "
        << callType << "  "
        << std::fixed << std::setprecision(2) << fee;
}

void charge::print() const {
    // 先打印基类通话信息
    callist::print();
    // 再打印费用信息
    std::cout << "  "
              << (callType == '0' ? "本地" : "长途") << "  "
              << std::fixed << std::setprecision(2) << fee << " 元";
}

char  charge::GetCallType() const { return callType; }
float charge::GetFee()      const { return fee; }
