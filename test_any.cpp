#include "any.hpp"
#include <iostream>

int main() {
    any a = 1;
    std::cout << a.type().name() << "\n";

    a = 1.2;
    std::cout << a.type().name() << "\n";

    return 0;
}