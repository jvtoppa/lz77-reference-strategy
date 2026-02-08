#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H
#include <vector>
#include <iostream>
#include <stdexcept>

using namespace std;

class CircularBuffer
{
    private:
    
    vector<char> buffer;
    size_t head;
    size_t capacity;
    size_t bytes_stored;

    public:

    //Constructor
    
    CircularBuffer(size_t s) : buffer(s), head(0), capacity(s), bytes_stored(0)
    {
        if (s == 0)
        {
            throw invalid_argument("Buffer capacity must be > 0");
        }
    }
    
    //Operations

    void append(char c)
    {
        buffer[head] = c;
        head = (head + 1) % capacity;

        if(bytes_stored < capacity) bytes_stored++;
        
        return;
    }

    void clear()
    {
        head = 0;
        bytes_stored = 0;
        return;
    }

    //Getters
    
    size_t get_capacity() const
    {
        return capacity;
    }

    char get_newest(size_t offset) const
    {
        if(offset >= bytes_stored)
        {
            throw out_of_range("Offset too large.\n");
        }

        return buffer[(head - offset - 1 + capacity) % capacity];
    
    }

    char get_oldest(size_t index) const
    {
        if(index >= bytes_stored)
        {
            throw out_of_range("Index out of bounds.\n");
        }

        return buffer[(((head - bytes_stored + capacity) % capacity) + index) % capacity];
    }

    size_t size() const
    {
        return bytes_stored;
    }

    //Helper methods

    void print() const
    {
        cout << "Buffer [" << bytes_stored << "/" << capacity << "]: ";
        for (size_t i = 0; i < bytes_stored; i++) {
            cout << get_oldest(i) << ", ";
        }
        cout << endl;
    }

    bool is_full() const
    {
        return bytes_stored == capacity;
    }
    
    bool is_empty() const
    {
        return bytes_stored == 0;
    }
};


#endif