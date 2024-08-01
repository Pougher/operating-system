#ifndef CORE_FILE_H
#define CORE_FILE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// the maximum number of file descriptors that can be open simultaneously
#define MAXIMUM_FILES 2048

// returned when an issue occurs when calling a file routine
#define FILE_INVALID -1

// returned when a file function succeeds
#define FILE_SUCCESS 0

typedef uint32_t FileDescriptor;

// global file descriptors that are used to write to or read from the terminal
extern FileDescriptor stdout;
extern FileDescriptor stdin;
extern FileDescriptor stderr;

typedef struct File {
    // the file pointer (advanced whenever a write is called)
    uint32_t file_pointer;

    // the write function controls what happens when bytes are written to an
    // open file
    void (*write_function)(struct File*, char*, uint32_t);

    // set to false if the file structure is invalid (no longer represents a
    // file), and is set to true if the file is valid (represents an open file)
    bool is_open;
} File;

// initializes the file module by setting up standard io
void file_init();

// creates a file and returns a new file descriptor. If there was an error in
// creating the file, -1 is returned. A success is indicated by a non negative
// return value
int32_t file_create(void (*)(File*, char*, uint32_t));

// declares a file as open and sets its structure parameters accordingly
void file_declare(File*, void (*)(File*, char*, uint32_t));

// writes a number of bytes to a file descriptor. If the file is invalid, then
// -1 is returned by the function, otherwise 0 is returned
int32_t file_write(FileDescriptor, char*, uint32_t);

// marks a file as closed, and can fail with FILE_INVALID
int32_t file_delete(FileDescriptor);

// kernel level standard input output write function, displays bytes as ASCII
// text directly to the screen
void stdout_write(File*, char*, uint32_t);

#endif
