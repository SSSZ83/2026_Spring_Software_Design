#include "marks.h"
#include <iostream>
#include <iomanip>

Marks::Marks()
    : Student(), usualScore(0), midtermScore(0),
      finalScore(0), zpScore(0), dj('E') {}

Marks::~Marks() {}

void Marks::In(std::ifstream& in) {
    long id;
    float usual, midterm, final;
    in >> id >> usual >> midterm >> final;
    SetId(id);
    // name 和 major 默认空，可在后续扩展中设置
    usualScore   = usual;
    midtermScore = midterm;
    finalScore   = final;
}

void Marks::Out(std::ofstream& out) {
    out << std::setw(6) << std::setfill('0') << GetId() << "  "
        << std::fixed << std::setprecision(2) << zpScore << "  "
        << dj;
}

void Marks::print() const {
    using std::cout;
    using std::setw;
    using std::setfill;
    using std::fixed;
    using std::setprecision;

    cout << "  " << setw(6) << setfill('0') << GetId() << setfill(' ')
         << "  " << setw(8) << fixed << setprecision(1) << usualScore
         << "  " << setw(8) << midtermScore
         << "  " << setw(8) << finalScore
         << "  " << setw(8) << zpScore
         << "    " << dj;
}

void Marks::ComputeZP() {
    zpScore = usualScore * USUAL_PCT
            + midtermScore * MIDTERM_PCT
            + finalScore * FINAL_PCT;
}

void Marks::ComputeDJ() {
    if (zpScore >= 90.0f)
        dj = 'A';
    else if (zpScore >= 80.0f)
        dj = 'B';
    else if (zpScore >= 70.0f)
        dj = 'C';
    else if (zpScore >= 60.0f)
        dj = 'D';
    else
        dj = 'E';
}

float Marks::GetZP() const {
    return zpScore;
}

float Marks::GetUsualScore() const   { return usualScore; }
float Marks::GetMidtermScore() const { return midtermScore; }
float Marks::GetFinalScore() const   { return finalScore; }

char Marks::GetDJ() const {
    return dj;
}
