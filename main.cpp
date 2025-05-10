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
    vd.chdir("tmp");
    vd.ls("V/tmp");

}

void testSystem2() {
    VirtualDirectory vd;
    vd.touch("furst.txt");
    vd.mkdir("tmp1");
    vd.mkdir("tmp2");
    vd.chdir("tmp1");

    vd.mkdir("lalala");
    vd.touch("V/tmp1/lalala/file.txt");

    vd.write("V/tmp1/lalala/file.txt", 0, 'W');
    vd.chdir("lalala");
    vd.write("file.txt", 1, 'X');

    vd.read("V/tmp1/lalala/file.txt", 0);
    vd.read("V/tmp1/lalala/file.txt", 1);

    vd.copy("file.txt", "file2.txt");
    vd.copy("file.txt", "V/tmp2/fafa.t");
    vd.lproot();

    vd.remove("V/tmp2/fafa.t");
    vd.remove("file.txt");
    vd.lproot();

    vd.cat("file2.txt");
    vd.cat("V/tmp1/lalala/file2.txt");
    vd.wc("V/tmp1/lalala/file2.txt");
}

void testSystem3() {
    VirtualDirectory vd;
    vd.mkdir("tmp1");
    vd.mkdir("V/tmp2");
    vd.mkdir("V/tmp2/dddddddddd");
    vd.chdir("tmp1");
    vd.pwd();
    vd.mkdir("lalala");
    vd.chdir("lalala");
    vd.pwd();
    vd.chdir("V/tmp2");
    vd.pwd();


    vd.lproot();
}


void testSystem4() {
    VirtualDirectory vd;
    vd.mkdir("tmp1");
    vd.mkdir("V/tmp2");
    vd.mkdir("V/tmp2/dddddddddd");
    vd.mkdir("V/tmp2/1");
    vd.mkdir("V/tmp2/1/okokok");
    vd.lproot();

    vd.chdir("V/tmp2");
    vd.pwd();
    vd.rmdir("1");


    vd.lproot();
}

void testSystem5() {
    VirtualDirectory vd;
    vd.mkdir("tmp1");
    vd.mkdir("V/tmp2");
    vd.mkdir("V/tmp2/dddddddddd");
    vd.mkdir("V/tmp3");
    vd.mkdir("V/okokok");
    vd.touch("V/test.txt");
    vd.touch("V/2.txt");
    vd.touch("4.txt");
    vd.touch("V/test4.txt");
    vd.ls("V/");
    vd.rmdir("V/tmp2");
    vd.ls("V/");
}
void testSystem6() {
    VirtualDirectory vd;
    vd.mkdir("tmp1");

    vd.touch("V/tmp1/test.txt");
    vd.write("V/tmp1/test.txt", 0, 'W');
    vd.cat("V/tmp1/test.txt");
    vd.cat("V/tmp1/test.txt");

    vd.lproot();

    vd.move("V/tmp1/test.txt", "V/tmp1/test.txt");
    vd.lproot();
    vd.cat("V/tmp1/test.txt");
}

void testSystem7() {
    VirtualDirectory vd;
    vd.mkdir("tmp1");

    vd.touch("V/tmp1/test.txt");
    vd.write("V/tmp1/test.txt", 0, 'W');
    vd.cat("V/tmp1/test.txt");
    vd.cat("V/tmp1/test.txt");

    vd.lproot();
    vd.ln("V/tmp1/test.txt", "V/test2.txt");

    vd.lproot();
    vd.cat("V/tmp1/test.txt");
}

void finalTest() {
    VirtualDirectory vd;
    vd.pwd();

    vd.touch("V/test.txt");
    vd.write("V/test.txt", 0, 'd');
    vd.write("V/test.txt", 0, 'd');
    vd.read("V/test.txt", 0);

    vd.write("V/test.txt", 1, 'e');
    vd.write("V/test.txt", 2, 'J');
    vd.write("V/test.txt", 3, 'o');
    vd.write("V/test.txt", 4, 'n');
    vd.write("V/test.txt", 5, 'g');

    // vd.cat("V/text.txt");
    vd.cat("V/test.txt");

    vd.mkdir("V/tmp");
    vd.chdir("V/tmp");
    vd.pwd();
    vd.ls("V/tmp");

    vd.copy("V/test.txt", "V/tmp/t.dat");
    vd.copy("FS.txt", "V/tmp/f.cc");
    vd.ln("V/tmp/f.cc", "V/tmp/FS.txt");
    vd.lproot();

    vd.rmdir("V/tmp");
    vd.lproot();
}
int main() {
    finalTest();
    return 0;

}


