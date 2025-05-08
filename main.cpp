#include "RefCountedFile.cpp"  // Assuming your code is in this header or .cpp file
#include <iostream>
#include <fstream>

using namespace std;

void test1() {
    try {
        RefCountedFile::touch("here.txt");
        RefCountedFile f("here.txt");

    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << '\n';
    }
    //here the file should be deleted from the stuck (and be the distractor from the file system)
}

void test2() {
    try {
        RefCountedFile::touch("here.txt");
        RefCountedFile f1("here.txt");
        f1[0] = 'A';
        f1[1] = 'B';
        f1[2] = 'C';
        cout << f1[0] << f1[1] << f1[2] << '\n';
        RefCountedFile::copy("here.txt", "Lala.txt");
        RefCountedFile f2("Lala.txt");

    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << '\n';
    }
    //here the file should be deleted from the stuck (and be the distractor from the file system)
}


void test3() {
    try {
        RefCountedFile::touch("here.txt");
        RefCountedFile f1("here.txt");
        f1[0] = 'A';
        f1[1] = 'B';
        f1[2] = 'C';
        cout << f1[0] << f1[1] << f1[2] << '\n';
        RefCountedFile::copy("here.txt", "Lala.txt");
        RefCountedFile f2("Lala.txt");
        RefCountedFile::remove("Lala.txt");


    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << '\n';
    }
    //here the file should be deleted from the stuck (and be the distractor from the file system)
}

int main() {
    test1();
    test2();
    test3();

    return 0;
}


