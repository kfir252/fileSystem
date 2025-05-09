#include "RefCountedFile.cpp"  // Assuming your code is in this header or .cpp file
#include <iostream>
#include <fstream>


using namespace std;

void testCopy() {
    RefCountedFile::touch("here.txt");
    RefCountedFile f1("here.txt");
    f1[0] = 'A';
    f1[1] = 'B';
    f1[2] = 'C';
    RefCountedFile::copy("here.txt", "Lala.txt");
    RefCountedFile f2("Lala.txt");
    f1.cat();
    f2.cat();

}


void testRemove() {
    try {
        RefCountedFile::touch("here.txt");
        RefCountedFile f1("here.txt");
        f1[0] = 'A';f1[1] = 'B';f1[2] = 'C';
        RefCountedFile::copy("here.txt", "Lala.txt");
        RefCountedFile f2("Lala.txt");
        RefCountedFile::remove("Lala.txt");
        f1.cat();
        f2.cat();
    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() <<  endl;
    }
    //here the file should be deleted from the stuck (and be the distractor from the file system)
}

void testMove() {
    try {
        RefCountedFile::touch("here.txt");
        RefCountedFile f1("here.txt");
        f1[0] = 'A';f1[1] = 'B';f1[2] = 'C';
        RefCountedFile::move("here.txt", "Lala.txt");
        RefCountedFile f2("Lala.txt");
        f2.cat();
        f1.cat();
    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() <<  endl;
    }
    //here the file should be deleted from the stuck (and be the distractor from the file system)
}

void testWC() {
    try {
        RefCountedFile::touch("here.txt");
        RefCountedFile f1("here.txt");
        f1[0] = 'A';f1[1] = 'B';f1[2] = 'C';
        f1.cat();f1.wc();
        f1[1] = ' ';;
        f1.cat();f1.wc();

    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << endl;
    }
    //here the file should be deleted from the stuck (and be the distractor from the file system)
}

void testLink() {
    RefCountedFile::touch("here.txt");
    RefCountedFile f1("here.txt");

    f1[0] = 'A';f1[1] = 'B';f1[2] = 'C';
    RefCountedFile f2 = f1;// this is how to link (create a new fake file that point to the same file)

    cout << "refCount: " << f1.getRefCount();
    f1.cat();

    cout << "refCount: " << f2.getRefCount();
    f2.cat();
    //here the file should be deleted from the stuck (and be the distractor from the file system)
}


void testSystem() {
    VirtualDirectory vd;

    //start pwd
    vd.pwd();

    //add file
    RefCountedFile::touch("test.txt");
    RefCountedFile f1("test.txt");

    //read write
    f1[0] = 'D';
    cout << f1[0] << endl;

    //cat check
    f1[1] = 'e';f1[2] = 'j';f1[3] = 'o';f1[4] = 'n';f1[5] = 'g';
    f1.cat();

    //should error "no such file name"
    // RefCountedFile f2("text.txt");
    // f2.cat();

    //chdir mkdir check
    vd.mkdir("tmp");
    vd.ls();

}

void testSystem2() {
    VirtualDirectory vd;
    vd.touch("file.txt");
    vd.ls();
    cout << "ok";

}


int main() {
    // testCopy();
    // testRemove();
    // testMove();
    // testWC();
    // testLink();
    // testSystem();
    testSystem2();
    return 0;
}


