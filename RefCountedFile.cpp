
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
    struct FileData {
        std::fstream stream;     // File stream
        int refCount;            // Reference counter
        std::string filename;    // File name

        // Constructor: open the file for read/write binary access
        FileData(const std::string& fname)
            : stream(fname, std::ios::in | std::ios::out | std::ios::binary), refCount(1), filename(fname) {
            if (!stream) {
                throw FileException("Failed to open file: " + fname);
            }
        }

        // Destructor: close the file stream
        ~FileData() {
            stream.close();
        }
    };
    FileData* data;  // Pointer to shared file data
    bool released = false;

    // Check if a stream position is valid (within file bounds)
    void checkBounds(std::streampos pos) const {

        std::ifstream file(data->filename, std::ios::binary);
        file.seekg(0, std::ios::end);
        if (pos < 0 || pos >= file.tellg()) {
            throw FileException("Index out of bounds.");
        }
    }

public:
    // Proxy class for reading/writing a single character in the file
    class CharProxy {
                RefCountedFile& file;
                std::streampos pos;

            public:
                CharProxy(RefCountedFile& f, std::streampos p) : file(f), pos(p) {}

                // Read a character from the file
                operator char() const {
                    std::ifstream in(file.data->filename, std::ios::binary);
                    if (!in) throw FileException("Cannot read from file.");
                    in.seekg(pos);
                    char c;
                    in.get(c);
                    return c;
                }

                // Write a character to the file
                CharProxy& operator=(char c) {
                    std::fstream out(file.data->filename, std::ios::in | std::ios::out | std::ios::binary);
                    if (!out) throw FileException("Cannot write to file.");
                    out.seekp(pos);
                    out.put(c);
                    out.flush();
                    return *this;
                }
            };

    // Non-const [] operator returns a proxy for reading/writing
    CharProxy operator[](std::streampos index) {
        return CharProxy(*this, index);
    }
    // Const [] operator for reading a character directly
    char operator[](std::streampos index) const {
        std::ifstream in(data->filename, std::ios::binary);
        if (!in) throw FileException("Cannot read from file.");
        in.seekg(index);
        char c;
        in.get(c);
        return c;
    }




    // Constructor: creates new FileData
    explicit RefCountedFile(const std::string& filename) {
        data = new FileData(filename);
    }
    // Copy constructor: increase reference count
    RefCountedFile(const RefCountedFile& other) {
        data = other.data;
        data->refCount++;
    }
    // Assignment operator: manage reference count correctly
    RefCountedFile& operator=(const RefCountedFile& other) {
        if (this != &other) {
            release();
            data = other.data;
            data->refCount++;
        }
        return *this;
    }
    // Destructor: release reference
    ~RefCountedFile() {
        release();
    }

    // Release function: decrease ref count and delete if zero
    void release() {
        if (!released && --data->refCount == 0) {
            delete data;
            try {
                std::filesystem::remove(data->filename);  // Delete the file from the filesystem
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
        copy(src, dst);
        remove(src);
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
        std::cout << data->filename << " (refs: " << data->refCount << ")\n";
        while (std::getline(in, line)) {
            std::cout << line << '\n';
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

    // Create a hard link to a file
    static void ln(const std::string& target, const std::string& linkName) {
        if (std::filesystem::exists(linkName)) {
            throw FileException("filename already exists cant create the link: " + linkName);
        }
        std::filesystem::create_hard_link(target, linkName);
    }

    // Getter for filename
    const std::string& getFilename() const {
        return data->filename;
    }

    const int getRefCount() const{
        return data->refCount;
    }
};













