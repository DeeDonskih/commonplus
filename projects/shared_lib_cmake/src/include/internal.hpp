#pragma once

namespace MyLibrary::Internal {
    inline void log(const char* msg) {
        // Простая отладочная печать
        std::cout << "[Internal] " << msg << std::endl;
    }
}
