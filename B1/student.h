#ifndef STUDENT_H
#define STUDENT_H

#include <string>

class Student {
private:
    long id;
    std::string name;
    std::string major;

public:
    Student();
    Student(long id, std::string name, std::string major);
    virtual ~Student();

    void SetId(long id);
    long GetId() const;

    void SetName(std::string name);
    std::string GetName() const;

    void SetMajor(std::string major);
    std::string GetMajor() const;
};

#endif
