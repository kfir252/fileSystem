
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
        if (other.data) {
            data = other.data;
            data->refCount++;
        } else {
            data = nullptr;
        }
        released = false;
    }

    RefCountedFile& operator=(const RefCountedFile& other) {
        if (this != &other) {
            release();
            if (other.data) {
                data = other.data;
                data->refCount++;
            } else {
                data = nullptr;
            }
            released = false;
        }
        return *this;
    }

    ~RefCountedFile() {
        //release() fatherLink also
        release();

    }

    void release() {
        if (!released && data) {
            if (--data->refCount == 0) {
                std::string filenameToDelete = data->filename;
                delete data;
                try {
                    std::filesystem::remove(filenameToDelete);
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Warning: Failed to delete file: " << e.what() << std::endl;
                }
            }
            released = true;
            data = nullptr;
        }
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
        if (src == dst) return;
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
        if (src != dst) {
            remove(src);//delete the old src
        }
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
        in.close();
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




///////////////////////////////////// HELP FUNCTIONS //////////////////
static bool startsWithVSlash(const std::string& input) {
    return input.size() >= 2 && input[0] == 'V' && input[1] == '/';
}
static std::string convertSlashesToUnderscores(const std::string& input) {
    std::string result = input;
    for (char& c : result) {
        if (c == '/') {
            c = '_';
        }
    }
    return result;
}
static std::string getFileNameFromPath(const std::string& path) {
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return path; // No '/' found, return the whole string
    }
    return path.substr(pos + 1);
}















class VirtualDirectory {
private:
    struct Node {
        std::string name;
        Node* parent;
        std::unordered_map<std::string, Node*> subdirs;
        std::unordered_map<std::string, RefCountedFile> files;

        Node(const std::string& name, Node* parent = nullptr)
            : name(name), parent(parent) {}

        ~Node() {
            for (auto& [_, subdir] : subdirs) {
                delete subdir; // recursively delete subdirectories
            }
            subdirs.clear(); // optional but clean
            files.clear();   // release all files
        }
    };

    Node* root;
    Node* current;

    void pwdNoEndl(Node* folder) const {
        std::vector<std::string> path;
        Node* temp = folder;
        while (temp) {
            path.push_back(temp->name);
            temp = temp->parent;
        }
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            std::cout << *it << "/";
        }
        std::cout << std::flush;
    }
    void deleteRecursive(Node* node) {
        if (!node) return;

        for (auto& [_, subdir] : node->subdirs) {
            deleteRecursive(subdir);
        }

        // No need to manually delete RefCountedFiles if they manage memory themselves
        delete node;
    }

public:
    VirtualDirectory() {
        root = new Node("V", nullptr);
        current = root;
    }
    ~VirtualDirectory() {
        delete root;
    }

    void mkdir(const std::string& path) {
        std::string pathh = path;
        if (!pathh.empty() && pathh.back() == '/') {
            pathh.pop_back();
        }

        Node* where = getNodeFromPath(pathh);
        std::string dirname = getFileNameFromPath (pathh);
        if (where->subdirs.count(dirname)) {
            throw FileException("folder already exist");
        }
        else {
            where->subdirs.emplace(dirname, new Node(dirname, where));
        }

    }

    void chdir(const std::string& path) {
        std::string pathh = path;
        if (!pathh.empty() && pathh.back() == '/') {
            pathh.pop_back();
        }
        current = getNodeFromPathForDirSearch(pathh);
    }

    void rmdir(const std::string& path) {
        std::string pathh = path;
        if (!pathh.empty() && pathh.back() == '/') {
            pathh.pop_back();
        }

        Node* father = getNodeFromPath(pathh);
        std::string dirname = getFileNameFromPath(pathh);

        auto it = father->subdirs.find(dirname);
        if (it == father->subdirs.end()) {
            throw FileException("Directory not found: " + dirname);
        }

        delete it->second;  // free memory, recursively
        father->subdirs.erase(it);
    }

    void ls(const std::string& path) const {
        Node* folder = getNodeFromPathForDirSearch(path);
        pwdNoEndl(folder);
        std::cout << ":" << std::endl;

        for (const auto& [name, dir] : folder->subdirs) {
            std::cout << "  [D] " << name << '\n';
        }
        for (const auto& [name, file] : folder->files) {
            std::cout << "  [F] " << name << " (refs: " << file.getRefCount() << ")\n";
        }
    }

    void lproot() const {
        printRecursive(root, 0);
    }

    void printRecursive(Node* node, int depth) const {
        std::string indent(depth * 2, ' ');
        std::cout << indent << node->name << "/\n" << std::flush;

        for (const auto& [fname, file] : node->files) {
            std::cout << indent << "  " << fname << " (refs: " << file.getRefCount() << ")\n";
        }

        for (const auto& [_, subdir] : node->subdirs) {
            printRecursive(subdir, depth + 1);
        }
        std::cout << std::flush;
    }

    void pwd() const {
        std::vector<std::string> path;
        Node* temp = current;
        while (temp) {
            path.push_back(temp->name);
            temp = temp->parent;
        }
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            std::cout << *it << "/";
        }
        std::cout << std::endl;
    }




    ///////////////////////////////////////////////////////////////////////
    // Add clean file to system
    void touch(const std::string& FilePath) {
        std::string fileName = getFileNameFromPath(FilePath);

        Node* where = current;
        if (startsWithVSlash(FilePath))
            where = getNodeFromPath(FilePath);

        RefCountedFile::touch(fileName);
        where->files.emplace(fileName, RefCountedFile(fileName));
    }
    void write(const std::string& FilePath, const int pos, const char character) {
        auto it = getRefCountedFileFromPath(FilePath);
        it[pos] = character;
    }
    void read(const std::string& FilePath, const int pos) {
        auto it = getRefCountedFileFromPath(FilePath);
        std::cout << it[pos] << std::endl;
    }
    void copy(const std::string& FilePathSrc, const std::string& FilePathDst) {
        if (FilePathSrc == FilePathDst) {
            return;
        }

        std::string srcFileName = getFileNameFromPath(FilePathSrc);
        std::string dstFileName = getFileNameFromPath(FilePathDst);

        touch(FilePathDst);
        Node* where = current;

        if (startsWithVSlash(FilePathSrc))
            where = getNodeFromPath(FilePathSrc);

        auto it = where->files.find(srcFileName);
        if (it == where->files.end()) {
            touch(FilePathSrc);
        }

        RefCountedFile::copy(srcFileName, dstFileName);
    }
    void remove(const std::string& FilePath) {
        std::string FileName = getFileNameFromPath(FilePath);
        auto file = getRefCountedFileFromPath(FilePath);
        auto folder = getNodeFromPath(FilePath);
        folder->files.erase(FileName);
        file.release();
    }

    void move(const std::string& FilePathSrc, const std::string& FilePathDst) {
        if (FilePathSrc == FilePathDst) {
            return;
        }
        std::string srcFileName = getFileNameFromPath(FilePathSrc);
        std::string dstFileName = getFileNameFromPath(FilePathDst);

        if (srcFileName == dstFileName) {
            return;//TODO take the file and deep copy to somwere alse, then delete
        }

        RefCountedFile::move(srcFileName, dstFileName);
        touch(FilePathDst);

        auto folder = getNodeFromPath(FilePathSrc);
        folder->files.erase(srcFileName);

    }
    void cat(const std::string& FilePath) {
        auto it = getRefCountedFileFromPath(FilePath);
        it.cat();
    }
    void wc(const std::string& FilePath) {
        auto it = getRefCountedFileFromPath(FilePath);
        it.wc();
    }
    void ln(const std::string& FilePathSrc, const std::string& FilePathDst) {
        if (FilePathSrc == FilePathDst) {
            return;
        }

        std::string srcFileName = getFileNameFromPath(FilePathSrc);
        std::string dstFileName = getFileNameFromPath(FilePathDst);
        auto fileToHardCopy = getRefCountedFileFromPath(FilePathSrc);


        Node* where = current;
        if (startsWithVSlash(FilePathDst))
            where = getNodeFromPath(FilePathDst);

        if (where->files.find(dstFileName) != where->files.end()) {
            where->files.erase(dstFileName);  // Deletes the existing object
        }
        where->files.emplace(dstFileName, RefCountedFile(fileToHardCopy));  // Inserts the new one
    }


    //the most important thing here for working with full paths
    Node* getNodeFromPathForDirSearch(const std::string& path) const {
        if (path.empty()) return nullptr;


        // Remove the last component after the final '/'
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash == std::string::npos) {
            auto it = current->subdirs.find(path);
            if (it == current->subdirs.end()) {
                throw FileException("folder not found");
            }
            return it->second;
        }
        std::string directoryPath = path;

        Node* currentNode = root;

        std::stringstream ss(directoryPath);
        std::string segment;

        // Skip the root prefix like "V" if needed
        std::getline(ss, segment, '/');
        if (segment != "V") return nullptr;

        // Traverse the path
        while (std::getline(ss, segment, '/')) {
            if (segment.empty()) continue;

            auto it = currentNode->subdirs.find(segment);
            if (it == currentNode->subdirs.end()) {
                throw FileException("folder not found");
                return current; // Directory not found
            }

            currentNode = it->second;
        }
        return currentNode;
    }

    //the most important thing here for working with full paths
    Node* getNodeFromPath(const std::string& pathh) {
        if (pathh.empty()) return nullptr;

        std::string path = pathh;
        if (!pathh.empty() && pathh.back() == '/') {
            path.pop_back();
        }

        // Remove the last component after the final '/'
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash == std::string::npos) {
            return current; // invalid path
        }
        std::string directoryPath = path.substr(0, lastSlash); // remove the last segment

        Node* currentNode = root;

        std::stringstream ss(directoryPath);
        std::string segment;

        // Skip the root prefix like "V" if needed
        std::getline(ss, segment, '/');
        if (segment != "V") return nullptr;

        // Traverse the path
        while (std::getline(ss, segment, '/')) {
            if (segment.empty()) continue;

            auto it = currentNode->subdirs.find(segment);
            if (it == currentNode->subdirs.end()) {
                return nullptr; // Directory not found
            }

            currentNode = it->second;
        }
        return currentNode;
    }

    RefCountedFile& getRefCountedFileFromPath(const std::string& path) {
        std::string fileName = getFileNameFromPath(path);
        Node* where = current;

        if (startsWithVSlash(path))
            where = getNodeFromPath(path);

        auto it = where->files.find(fileName);
        if (it == where->files.end()) {
            throw FileException("File not found");
        }
        return it->second;
    }




    // Get file by name (optional)
    RefCountedFile& getFile(const std::string& name) {
        if (!current->files.count(name)) {
            throw FileException("File not found in current directory.");
        }
        return current->files[name];
    }
};



