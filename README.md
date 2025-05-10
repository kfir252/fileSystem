
# RefCountedFile Virtual Filesystem Library

Manage a virtual filesystem in C++ with reference-counted files. Includes a console app for testing.
the files are auto deleted when they are deleted in the virtual system, and when exiting.

## Features

- **Library Style**: Drop-in `RefCountedFile` and `VirtualDirectory` classes.
- **Reference Counting**: Files delete themselves when no references remain.
- **Virtual Directories**: `mkdir`, `chdir`, `rmdir`, `ls`, `lproot`, `pwd`.
- **File Operations**:
  - `touch`
  - `copy`, `move`, `remove`
  - `cat`
  - `wc`
  - `operator[]` for random-access read/write
  - `ln` for virtual hard links
- **Console App**: Interactive shell supporting all commands.

## File Layout

```
.
├── RefCountedFile.cpp  # Library implementation
├── main.cpp            # Console app
├── README.md           # This file
```

## Requirements

- C++17 or newer
- Standard headers: `<fstream>`, `<iostream>`, `<string>`, `<unordered_map>`, `<filesystem>`

## Example Session

```
pwd
touch V/test.txt
write V/test.txt 0 d
read V/test.txt 0
write V/test.txt 1 e
write V/test.txt 2 J
write V/test.txt 3 o
write V/test.txt 4 n
write V/test.txt 5 g
cat V/text.txt
cat V/test.txt
mkdir V/tmp
chdir V/tmp/
pwd
ls V/tmp
copy V/test.txt V/tmp/t.dat
copy FS.cpp V/tmp/f.cc
ln V/tmp/f.cc V/tmp/FS.cpp
ls V/tmp
lproot
rmdir V/tmp
lproot
exit
```
