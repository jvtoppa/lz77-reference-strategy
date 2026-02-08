#include <vector>
#include <string>
#include <iostream>

using namespace std;
struct lz77
{
    int offset;
    int len;
    char next_character;
};

class CircularBuffer
{
    private:
    
    vector<char> buffer;
    size_t head;
    size_t capacity;
    size_t bytes_stored;

    public:
    CircularBuffer(size_t s) : buffer(s), head(0), capacity(s), bytes_stored(0) {}
    

    void append(char c)
    {
        buffer[head] = c;
        head = (head + 1) % capacity;

        if(bytes_stored < capacity) bytes_stored++;
        
        return;
    }

    char get_newest(size_t offset)
    {
        if(offset >= bytes_stored)
        {
            throw out_of_range("Offset too large.\n");
        }

        return buffer[(head - offset - 1 + capacity) % capacity];
    
    }

    char get_oldest(size_t index)
    {
        if(index >= bytes_stored)
        {
            throw out_of_range("Index out of bounds.\n");
        }

        return buffer[(((head - bytes_stored + capacity) % capacity) + index) % capacity];
    }

    size_t size()
    {
        return bytes_stored;
    }

    void clear()
    {
        head = 0;
        bytes_stored = 0;
        return;
    }
    
    void print()
    {
        cout << "Buffer [" << bytes_stored << "/" << capacity << "]: ";
        for (size_t i = 0; i < bytes_stored; i++) {
            cout << get_oldest(i) << ", ";
        }
        cout << endl;
    }
};


int main(void)
{

    return 0;
}

