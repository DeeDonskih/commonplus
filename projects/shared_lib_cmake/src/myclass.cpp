#include "MyLibrary/MyClass.hpp"

namespace MyLibrary {

MyClass::MyClass(int x) : value(x) {}

int MyClass::getValue() const {
    return value;
}

void MyClass::setValue(int val) {
    value = val;
}

}
