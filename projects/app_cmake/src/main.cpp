#include "app.h"
#include "lib.h"
#include "header_only_lib.h"
#include <iostream>

int main() {
    std::cout << "Main app running...\n";
    say_hello();
    use_library();
    return 0;
}