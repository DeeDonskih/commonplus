#include "MyLibrary/functions.hpp"
#include "internal.hpp"
#include <iostream>

namespace MyLibrary {

void hello() {
    Internal::log("Calling hello()");
    std::cout << "Hello from MyLibrary!" << std::endl;
}

int add(int a, int b) {
    Internal::log("Calling add()");
    return a + b;
}

}
