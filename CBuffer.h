#ifndef CIRCBUFFER_H_
#define CIRCBUFFER_H_

#include <memory>

template <class T>
class CircBuffer {
public:
    CircBuffer(int length, void *addr = nullptr) {
        write = 0;
        read = 0;
        size = length + 1;
        if (addr) {
            buf = (T *)addr;
        } else {
            buf = (T *)malloc(size * sizeof(T));
        }
        //if (buf == nullptr)
           // error("Can't allocate memory");
    };

    bool isFull() {
        return (((write + 1) % size) == read);
    };

    bool isEmpty() {
        return (read == write);
    };

    void queue(T k) {
        if (isFull()) {
//            read++;
//            read %= size;
            return;
        }
        buf[write++] = k;
        write %= size;
    }
    
    void flush() {
        read = 0;
        write = 0;
    }
    

    int available() {
        return (write >= read) ? write - read : size - read + write;
    };

    bool dequeue(T * c) {
        bool empty = isEmpty();
        if (!empty) {
            *c = buf[read++];
            read %= size;
        }
        return(!empty);
    };

private:
    volatile int write;
    volatile int read;
    int size;
    T * buf;
};

#endif
