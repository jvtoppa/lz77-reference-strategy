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
        if(bytes_stored < capacity)
        {
            bytes_stored++;
        }
        return;
    }


    
};


int main(void)
{

    return 0;
}

