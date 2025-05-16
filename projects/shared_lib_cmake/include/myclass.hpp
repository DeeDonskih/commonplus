#pragma once

namespace MyLibrary {

class MyClass {
public:
    MyClass(int x);
    int getValue() const;
    void setValue(int val);
private:
    int value;
};

}
