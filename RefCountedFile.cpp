
// Includes for file and string operations, exceptions, I/O, filesystem, time, and smart pointers
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <ctime>
#include <memory>
#include <unordered_map>
#include <vector>




// Custom exception for file-related errors
class FileException : public std::runtime_error {
public:
    explicit FileException(const std::string& msg) : std::runtime_error(msg) {}
};






// Main class managing a file with reference counting
class RefCountedFile {
private:
    // Struct to hold file data and reference count
private:
    struct FileData {
        int refCount;
        std::string filename;

        FileData(const std::string& fname)
            : refCount(1), filename(fname) {
            std::fstream test(fname, std::ios::in | std::ios::out | std::ios::binary);
            if (!test) {
                throw FileException("Failed to open file: " + fname);
            }
        }
    };

    FileData* data;  // Pointer to shared file data
    bool released = false;

    void checkBounds(std::streampos pos) const {
        std::ifstream file(data->filename, std::ios::binary);
        file.seekg(0, std::ios::end);
        if (pos < 0 || pos >= file.tellg()) {
            throw FileException("Index out of bounds.");
        }
    }

public:
    class CharProxy {
        RefCountedFile& file;
        std::streampos pos;

    public:
        CharProxy(RefCountedFile& f, std::streampos p) : file(f), pos(p) {}

        operator char() const {
            std::ifstream in(file.data->filename, std::ios::binary);
            if (!in) throw FileException("Cannot read from file.");
            in.seekg(pos);
            char c;
            in.get(c);
            return c;
        }

        CharProxy& operator=(char c) {
            std::fstream out(file.data->filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!out) throw FileException("Cannot write to file.");
            out.seekp(pos);
            out.put(c);
            out.flush();
            return *this;
        }
    };

    CharProxy operator[](std::streampos index) {
        return CharProxy(*this, index);
    }

    char operator[](std::streampos index) const {
        std::ifstream in(data->filename, std::ios::binary);
        if (!in) throw FileException("Cannot read from file.");
        in.seekg(index);
        char c;
        in.get(c);
        return c;
    }

    RefCountedFile() {
        data = nullptr;
        released = false;
        // Default constructor logic, if needed
    }

    explicit RefCountedFile(const std::string& filename) {
        data = new FileData(filename);
    }

    RefCountedFile(const RefCountedFile& other) {
        data = other.data;
        data->refCount++;
    }

    RefCountedFile& operator=(const RefCountedFile& other) {
        if (this != &other) {
            release();
            data = other.data;
            data->refCount++;
        }
        return *this;
    }

    ~RefCountedFile() {
        //release() fatherLink also
        release();

    }

    void release() {
        if (!released && --data->refCount == 0) {
            std::string filenameToDelete = data->filename;
            delete data;
            try {
                std::filesystem::remove(filenameToDelete);
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Warning: Failed to delete file: " << e.what() << std::endl;
            }
        }
        released = true;
    }


    // Creates the file if it doesn't exist and updates its modification time
    static void touch(const std::string& filename) {
        std::ofstream f(filename, std::ios::app);
        if (!f) throw FileException("Cannot touch file: " + filename);
        f.close();

        // Update the last write time to now
        std::filesystem::last_write_time(filename, std::filesystem::file_time_type::clock::now());
    }

    // Copy contents from one file to another
    static void copy(const std::string& src, const std::string& dst) {
        touch(src);

        std::ifstream in(src, std::ios::binary);
        if (!in)
            throw FileException("Failed to open source file for reading.");

        std::ofstream out(dst, std::ios::binary | std::ios::trunc);
        if (!out)
            throw FileException("Failed to open destination file for writing.");

        out << in.rdbuf();  // Stream buffer copy
    }

    // Delete a file
    static void remove(const std::string& filename) {

        if (!std::filesystem::remove(filename))
            throw FileException("Failed to remove file: " + filename);
    }

    // Move file from src to dst (copy then remove)
    static void move(const std::string& src, const std::string& dst) {
        copy(src, dst);//chose to do a new copy here
        remove(src);//delete the old src
    }

    // Display contents of file with reference count
    void cat() const {
        if (released) {
            throw FileException("File Variable is released.");
        }
        std::ifstream in(data->filename);
        if (!in) throw FileException("Failed to open file for reading");
        std::string line;

        // Also print refCount and filename
        // std::cout << data->filename << " (refs: " << data->refCount << ")\n";
        while (std::getline(in, line)) {
            std::cout << line << std::endl;
        }
    }

    // Word count: count lines, words, and characters in file
    void wc() const {
        std::ifstream in(data->filename);
        if (!in) throw FileException("Failed to open file for wc");

        int lines = 0, words = 0, chars = 0;
        std::string word;
        char ch;
        while (in.get(ch)) {
            chars++;
            if (ch == '\n') lines++;
            if (std::isspace(static_cast<unsigned char>(ch))) {
                if (!word.empty()) {
                    words++;
                    word.clear();
                }
            } else {
                word += ch;
            }
        }
        if (!word.empty()) words++;
        std::cout << lines << " " << words << " " << chars << '\n';
    }

    // Getter for filename
    const std::string& getFilename() const {
        return data->filename;
    }

    const int getRefCount() const{
        return data->refCount;
    }
};






















class VirtualDirectory {
private:
    struct Node {
        std::string name;
        Node* parent;
        std::unordered_map<std::string, std::unique_ptr<Node>> subdirs;
        std::unordered_map<std::string, RefCountedFile> files;

        Node(const std::string& name, Node* parent = nullptr)
            : name(name), parent(parent) {}
    };

    std::unique_ptr<Node> root;
    Node* current;

public:
    VirtualDirectory() {
        root = std::make_unique<Node>("V", nullptr);
        current = root.get();
    }

    void mkdir(const std::string& dirname) {
        if (current->subdirs.count(dirname)) {
            throw FileException("Directory already exists: " + dirname);
        }
        current->subdirs[dirname] = std::make_unique<Node>(dirname, current);
    }

    void chdir(const std::string& dirname) {
        if (dirname == "..") {
            if (current->parent) {
                current = current->parent;
            }
        } else if (current->subdirs.count(dirname)) {
            current = current->subdirs[dirname].get();
        } else {
            throw FileException("Directory not found: " + dirname);
        }
    }

    void rmdir(const std::string& dirname) {
        if (!current->subdirs.count(dirname)) {
            throw FileException("Directory not found: " + dirname);
        }
        current->subdirs.erase(dirname); // Automatically frees all nested data due to unique_ptr
    }

    void ls() const {
        std::cout << "Folder:" << std::flush;
        pwd();
        for (const auto& [name, dir] : current->subdirs) {
            std::cout << "  [D] " << name << '\n';
        }
        for (const auto& [name, file] : current->files) {
            std::cout << "  [F] " << name << " (refs: " << file.getRefCount() << ")\n";
        }
    }

    void proot() const {
        printRecursive(root.get(), 0);
    }

    void printRecursive(Node* node, int depth) const {
        std::string indent(depth * 2, ' ');
        std::cout << indent << node->name << "/\n";

        for (const auto& [fname, file] : node->files) {
            std::cout << indent << "  " << fname << " (refs: " << file.getRefCount() << ")\n";
        }

        for (const auto& [_, subdir] : node->subdirs) {
            printRecursive(subdir.get(), depth + 1);
        }
    }

    void pwd() const {
        std::vector<std::string> path;
        Node* temp = current;
        while (temp) {
            path.push_back(temp->name);
            temp = temp->parent;
        }
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            std::cout << "/" << *it;
        }
        std::cout << '\n';
    }

    // Add file to current directory
    void addFile(const std::string& name, const std::string& realFilePath) {
        if (current->files.count(name)) {
            throw FileException("File already exists in virtual directory.");
        }
        current->files[name] = RefCountedFile(realFilePath);
    }

    // Get file by name (optional)
    RefCountedFile& getFile(const std::string& name) {
        if (!current->files.count(name)) {
            throw FileException("File not found in current directory.");
        }
        return current->files[name];
    }
};



