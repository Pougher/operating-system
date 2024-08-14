import copy
import glob

header_sequence = [ ord(i) for i in 'file']
num_files = 0
current_file = 0
current_byte = 0

def u32_to_bytes(u32):
    return [
        (u32 >> 24) & 0xff,
        (u32 >> 16) & 0xff,
        (u32 >> 8)  & 0xff,
        u32         & 0xff
    ][::-1]

# generates the root header for the entire filesystem
# header format is:
# Root header 'KERNFS' (6 Bytes)
# Version (2 bytes)
# Number of files (4 bytes)
def generate_root_header(numfiles):
    global current_byte
    header = []
    header += [ord(i) for i in 'KERNFS']
    header += [ 0x00, 0x00 ]
    header += u32_to_bytes(numfiles)
    current_byte += 12
    return header

# generates a file header from a given file input
# header format is:
# Header (4 Bytes)
# Length (4)
# Filename (32 Bytes)
# Next File Pointer (offset into root)
def generate_file_header(filename, data):
    global current_file, current_byte

    header = copy.deepcopy(header_sequence)
    header += u32_to_bytes(len(data))
    header += [ord(i) for i in filename]
    header += [0 for i in range(32 - len(filename))]

    current_file += 1

    if current_file == num_files:
        header += [0] * 4
    else:
        next = (current_byte + 44 + len(data))
        header += u32_to_bytes(next)
    current_byte += 44 + len(data)
    return header

# function to write a file's data into the output file
def add_file(path, open_file, name):
    file_bytes = bytearray([])
    with open(path, 'rb') as f:
        file_bytes = f.read()

    open_file.write(bytearray(generate_file_header(name, file_bytes)))
    open_file.write(bytearray(file_bytes))

    return file_bytes

# returns a filename from a file path
def get_filename(path):
    toks = path.split('/')
    filename = toks[-1]

    if len(filename) > 31:
        filename = filename[:30] + '~'
    return toks[-1]

if __name__ == '__main__':
    print("\n*** Building File System ***\n")
    files = glob.glob('filesystem/*.elf')

    try:
        files.remove('filesystem/filesystem')
    except ValueError:
        pass

    filenames = [ get_filename(path) for path in files ]
    num_files = len(files)

    with open('filesystem/filesystem', 'wb') as f:
        f.write(bytearray(generate_root_header(len(files))))
        for idx, i in enumerate(files):
            print("Adding file: " + i + '... ', end='')
            add_file(i, f, filenames[idx])
            print("Done")
    print("\nFilesystem Size: " + str(current_byte) + " bytes")
    print("\n*** Finished Building File System ***\n")

