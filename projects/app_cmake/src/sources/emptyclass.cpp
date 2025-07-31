#include "emptyclass.hpp"
#include <iostream>

EmptyCppClass::EmptyCppClass() {
  std::cout <<__PRETTY_FUNCTION__<<std::endl;
  std::cout << "Empty class CTOR" <<std::endl;
}

EmptyCppClass::~EmptyCppClass() {
  std::cout <<__PRETTY_FUNCTION__<<std::endl;
  std::cout << "Empty class DTOR" <<std::endl;
}
void EmptyCppClass::method() {
  std::cout <<__PRETTY_FUNCTION__<<std::endl;
  std::cout << "Empty class method()" <<std::endl;
}
