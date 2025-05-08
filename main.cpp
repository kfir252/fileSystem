#include "RefCountedFile.cpp"  // Assuming your code is in this header or .cpp file
#include <iostream>
#include <fstream>

int main() {
    try {
        RefCountedFile::touch("here.txt");
        RefCountedFile f("here.txt");












        // VirtualFileSystem vfs;
        // vfs.mkdir("docs");
        // vfs.chdir("docs");
        //
        //
        // // std::string path = "V/docs/f1.txt";
        // std::string path = "f1.txt";
        //
        // RefCountedFile::touch(path);
        // RefCountedFile file(path);
        // vfs.current->files.try_emplace(path, file);
        // vfs.lproot();
        // file.release();






        // vfs.pwd();

        // vfs.mkdir("projects");
        // vfs.pwd();
        //
        // // vfs.createFile("example.txt");
        // vfs.getFileByName("example.txt")[0] = 'x';
        // vfs.getFileByName("example.txt").cat();

        // file.wc();
        // std::cout << file[6];
        // file[6] = 'W';
        // std::cout << file[6] << std::endl;
        // file.release();

        // file.cat();
        // std::cout << "Testing reference count...\n";
        // RefCountedFile another = file;
        // std::cout << "Ref count after copy: " << another.getRefCount() << "\n";
        //
        // std::cout << "Creating a hard link...\n";
        // RefCountedFile::ln("example.txt", "linked_example.txt");
        //
        // std::cout << "Listing contents:\n";
        // vfs.ls();  // Shows example.txt
        //
        // std::cout << "Full directory structure:\n";
        // vfs.chdir("..");
        // vfs.lproot();
    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << '\n';
    }
    return 0;
}
