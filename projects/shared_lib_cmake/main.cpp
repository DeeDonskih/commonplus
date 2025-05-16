#include "MyLibrary/functions.hpp"
#include "MyLibrary/MyClass.hpp"
#include "MyLibrary/HeaderOnly.hpp"
#include <iostream>

int main() {
    MyLibrary::hello();
    std::cout << "add(3, 4) = " << MyLibrary::add(3, 4) << "\n";

    MyLibrary::MyClass obj(10);
    std::cout << "MyClass: " << obj.getValue() << "\n";
    obj.setValue(77);
    std::cout << "MyClass updated: " << obj.getValue() << "\n";

    MyLibrary::HeaderOnly<double> templ(3.14);
    std::cout << "HeaderOnly: " << templ.getValue() << "\n";

    return 0;
}
