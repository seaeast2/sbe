#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "SBE - Toy Compiler Backend" << std::endl;
    
    if (argc > 1) {
        std::cout << "Processing input file: " << argv[1] << std::endl;
    } else {
        std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    return 0;
}
