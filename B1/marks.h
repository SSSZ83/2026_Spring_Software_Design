#ifndef MARKS_H
#define MARKS_H

#include "student.h"
#include <fstream>

class Marks : public Student {
public:
    static constexpr float USUAL_PCT   = 0.30f;
    static constexpr float MIDTERM_PCT = 0.30f;
    static constexpr float FINAL_PCT   = 0.40f;

private:
    float usualScore;
    float midtermScore;
    float finalScore;
    float zpScore;
    char  dj;

public:
    Marks();
    virtual ~Marks();

    void In(std::ifstream& in);
    void Out(std::ofstream& out);
    void print() const;
    void ComputeZP();
    void ComputeDJ();
    float GetUsualScore() const;
    float GetMidtermScore() const;
    float GetFinalScore() const;
    float GetZP() const;
    char  GetDJ() const;
};

#endif
