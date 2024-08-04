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

// default stdin buffer size
#define STDIN_BUFFER_SIZE 4096

typedef uint32_t FileDescriptor;

// global file descriptors that are used to write to or read from the terminal
extern FileDescriptor stdout;
extern FileDescriptor stdin;
extern FileDescriptor stderr;

typedef struct File {
    // the file pointer (advanced whenever a write is called)
    uint32_t file_pointer;

    // variable to hold the current state of the file object
    void *state;

    // the init function is run once upon construction of the file object
    void (*init_function)(struct File*);

    // the write function controls what happens when bytes are written to an
    // open file
    void (*write_function)(struct File*, char*, uint32_t);
    
    // the read function is called when a file read is performed, and controls
    // what happens when bytes are read from an open file
    void (*read_function)(struct File*, void*, uint32_t);

    // set to false if the file structure is invalid (no longer represents a
    // file), and is set to true if the file is valid (represents an open file)
    bool is_open;
} File;

// typedefs to clean up code a bit, used as types for function pointers
typedef void (*FileWriteFunc)(struct File*, char*, uint32_t);
typedef void (*FileInitFunc)(struct File*);
typedef void (*FileReadFunc)(struct File*, void*, uint32_t);

// initializes the file module by setting up standard io
void file_init();

// creates a file and returns a new file descriptor. If there was an error in
// creating the file, -1 is returned. A success is indicated by a non negative
// return value
int32_t file_create(FileInitFunc, FileWriteFunc, FileReadFunc);

// declares a file as open and sets its structure parameters accordingly
void file_declare(File*, FileInitFunc, FileWriteFunc, FileReadFunc);

// writes a number of bytes to a file descriptor. If the file is invalid, then
// -1 is returned by the function, otherwise 0 is returned
int32_t file_write(FileDescriptor, char*, uint32_t);

// reads a number of bytes from a file descriptor into an object. The streamed
// file data is cut off when the end of the file is reached. If the file is
// invalid then -1 is returned, otherwise 0 is returned.
int32_t file_read(FileDescriptor, void*, uint32_t);

// marks a file as closed, and can fail with FILE_INVALID
int32_t file_delete(FileDescriptor);

// writes a character to the specified file descriptor. -1 is returned if the
// file descriptor is invalid, otherwise 0 is returned
int32_t file_write_char(FileDescriptor, char);

// kernel-level default I/O file functions

// initializes the stdin buffer to be 4096 bytes large
void stdin_init(File*);

// kernel level standard output write function, displays bytes as ASCII
// text directly to the screen
void stdout_write(File*, char*, uint32_t);

// kernel level standard input write function, displays bytes on screen as ASCII
// but also writes the characters into a 4k buffer
void stdin_write(File*, char*, uint32_t);

// kernel level standard input write function, reads a number of bytes stored in
// stdin to a buffer
void stdin_read(File*, void*, uint32_t);

#endif
