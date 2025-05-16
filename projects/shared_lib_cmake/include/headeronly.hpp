#pragma once

namespace MyLibrary {

template<typename T>
class HeaderOnly {
public:
    HeaderOnly(T val) : value(val) {}
    T getValue() const { return value; }
    void setValue(T val) { value = val; }
private:
    T value;
};

}
