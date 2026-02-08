#ifndef RAMDOM_ACCESS_QUEUE_H
#define RAMDOM_ACCESS_QUEUE_H

#include <vector>
#include <stdexcept>
#include <sstream>
using namespace std;

class RandomAccessQueue
{
    private:
    vector<char> buffer;
    size_t current_pos;
    size_t capacity;

    public:
    
    //Constructor
    RandomAccessQueue(size_t s) : buffer(), capacity(s), current_pos(0)
    {
        if (s == 0)
        {
            throw invalid_argument("Buffer capacity must be > 0.");
        }
    }

    //Operations
    char peek(size_t offset = 0) const
    {
        if (current_pos + offset >= buffer.size())
        {
            throw out_of_range("Not enough bytes in buffer.");
        }
        return buffer[current_pos + offset];
    }

    char operator[](size_t offset) const
    {
    
        return peek(offset);
    }

    void enqueue(char c)
    {
        if (available() >= capacity)  // Use available() instead!
        {
            throw out_of_range("Queue at capacity.");
        }
        buffer.push_back(c);
    }

    void consume(size_t n)
    {
        if(current_pos + n > buffer.size())
        {
            throw out_of_range("Out of Bounds.");
        }

        current_pos += n;
        if (current_pos > buffer.size() / 2)
        {
            buffer.erase(buffer.begin(), buffer.begin() + current_pos);
            current_pos = 0;
        }
    }

    void refill(istream& input, size_t s)
    {
        while (available() < s && available() < capacity)
        {
            if (input.eof())
            {
                return;
            }
            char c = input.get();
            buffer.push_back(c);
        }
    }

    void clear()
    {
        buffer.clear();
        current_pos = 0;
        return;
    }

    //Helper Funcs


    size_t available() const
    {
        if (current_pos >= buffer.size()) return 0;
        return buffer.size() - current_pos;
    }

    bool empty() const
    {
        return available() == 0;
    }

    size_t get_capacity() const
    {
        return capacity;
    }

};



#endif