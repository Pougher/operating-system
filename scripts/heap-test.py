import random

class HeapHeader:
    def __init__(self, next, prev, free, len):
        self.next = next
        self.prev = prev
        self.free = free
        self.length = len

class Heap:
    def __init__(self, size):
        self.size = size
        self.max_cap = 2048
        self.heap = [0] * self.size

        self.heap[0] = HeapHeader(0, 0, True, len(self.heap) - 1)

    def split_heap(self, index, size):
        remainder = self.heap[index].length - size - 1

        self.heap[index].length = size
        self.heap[index].next = index + 1 + size
        self.heap[index].free = False

        self.heap[index + 1 + size] = HeapHeader(0, index, True, remainder)

    def malloc(self, size):
        for i in range(len(self.heap)):
            if isinstance(self.heap[i], HeapHeader):
                header = self.heap[i]
                if header.length >= size and header.free:
                    if (header.length - size) >= 16:
                        self.split_heap(i, size)
                        return i + 1
                    else:
                        self.heap[i].free = False
                        return i + 1
        return 0

    def free(self, ptr):
        header = self.heap[ptr - 1]
        next_index = header.next

        print(header.next)

        header.free = True

        if header.next != 0:
            if self.heap[header.next].free:
                next_header = self.heap[header.next]
                self.heap[next_header.next].prev = ptr - 1

                self.heap[ptr - 1].next = next_header.next
                self.heap[ptr - 1].length = next_header.length + 1 + \
                    self.heap[ptr - 1].length
                self.heap[next_index] = 0
            print("Defragmented heap: " + str(self.heap[ptr - 1].length))


heap = Heap(2048)

alloc = []

for i in range(20):
    x = heap.malloc(random.randint(16, 96))
    if x == 0:
        print("OUT OF MEMORY")
    else:
        alloc.append(x)


print(heap.heap)

for i in alloc[::-1]:
    heap.free(i)

print(heap.heap)
