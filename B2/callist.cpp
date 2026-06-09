#include "callist.h"
#include <iostream>
#include <iomanip>

callist::callist()
    : srcAreaCode(""), srcPhone(""),
      dstAreaCode(""), dstPhone(""),
      duration(0) {}

callist::~callist() {}

void callist::In(std::ifstream& in) {
    in >> srcAreaCode >> srcPhone
       >> dstAreaCode >> dstPhone
       >> duration;
}

void callist::print() const {
    using std::cout;
    using std::setw;
    using std::setfill;

    cout << "  " << setfill(' ')
         << setw(4)  << srcAreaCode << "  "
         << setw(8)  << srcPhone   << "  "
         << setw(4)  << dstAreaCode << "  "
         << setw(10) << dstPhone   << "  "
         << setw(4)  << duration << "s";
}

std::string callist::GetSrcAreaCode() const { return srcAreaCode; }
std::string callist::GetSrcPhone()    const { return srcPhone; }
std::string callist::GetDstAreaCode() const { return dstAreaCode; }
std::string callist::GetDstPhone()    const { return dstPhone; }
int         callist::GetDuration()    const { return duration; }
