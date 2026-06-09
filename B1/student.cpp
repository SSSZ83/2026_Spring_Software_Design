#include "student.h"

Student::Student() : id(0), name(""), major("") {}

Student::Student(long id, std::string name, std::string major)
    : id(id), name(name), major(major) {}

Student::~Student() {}

void Student::SetId(long id) {
    this->id = id;
}

long Student::GetId() const {
    return id;
}

void Student::SetName(std::string name) {
    this->name = name;
}

std::string Student::GetName() const {
    return name;
}

void Student::SetMajor(std::string major) {
    this->major = major;
}

std::string Student::GetMajor() const {
    return major;
}
