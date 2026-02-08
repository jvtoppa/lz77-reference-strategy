#include <vector>
#include <string>
#include <iostream>
#include "circular_buffer.h"
#include <unordered_map>
#include "random_access_queue.h"
using namespace std;

struct token
{
    int offset;
    int len;
    char next_character;
};

class lz77
{
    private:
    CircularBuffer search_buffer;
    vector<char> lookahead_buffer;
    unordered_map<int32_t, vector<size_t>> hash;

};


int main(void)
{

        RandomAccessQueue q(5);
        
        q.enqueue('a');
        q.enqueue('b');
        q.enqueue('c');
        cout << "Available: " << q.available() << endl;  // 3
        
        q.consume(2);
        cout << "Available after consume 2: " << q.available() << endl;  // 1
        
        stringstream input("defghij");
        q.refill(input, 5);
        cout << "Available after refill to 5: " << q.available() << endl;  // 5
        
        // Should stop at capacity 5, not read 'i' and 'j'
        q.refill(input, 10);
        cout << "Available after refill to 10: " << q.available() << endl;  // Still 5
        
        return 0;
}

