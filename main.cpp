#include "RefCountedFile.cpp"  // Assuming your code is in this header or .cpp file
#include <iostream>
#include <fstream>

int main() {
    try {
        RefCountedFile::touch("here.txt");
        RefCountedFile f("here.txt");

    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << '\n';
    }
    return 0;
}
