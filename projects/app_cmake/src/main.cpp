#include "app.h"
#include "header_only_lib.h"
#include "lib.h"
#include "sources/emptyclass.hpp"
#include "sources/pureccode.h"

#include <iostream>

#ifdef MA_NO_OPENCV
#warning "OpenCV is not enabled"
#endif

int main()
{
  std::cout << "Main app running...\n";
  say_hello();
  use_library();
  {
    EmptyCppClass object;
    object.method();
  }
  PureCFunction(__PRETTY_FUNCTION__);
  return 0;
}