////////////////////////////////////////////////////////////////////////////////
// count.cpp
//   An example tool computing the size of LZ77 parsing of a given file.
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Juha Karkkainen, Dominik Kempa and Simon J. Puglisi
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////


//Data structures:

//1. Count how many references each character has (reference_count array)
//2. Insert pointer to positions in a priority queue
//3. Create bitvector that represents where a position is chosen


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <cstring>
#include "../include/kkp.h"
#include "../include/common.h"
#include <queue>
#include <algorithm>
#include <sdsl/bit_vectors.hpp>
#include <map>


using namespace std;
using namespace sdsl;

#define DICTIONARY_SIZE 30
#define KMER_SIZE 16

class HashHeap {
  private:
      vector<size_t> heap;
      vector<size_t> pos_to_idx;
      vector<size_t> scores;
      size_t n;
      
      void heapify_up(size_t idx) {
          while (idx > 0) {
              size_t parent = (idx - 1) / 2;
              if (scores[heap[idx]] <= scores[heap[parent]])
                  break;
              swap(heap[idx], heap[parent]);
              pos_to_idx[heap[idx]] = idx;
              pos_to_idx[heap[parent]] = parent;
              idx = parent;
          }
      }
      
      void heapify_down(size_t idx) {
          size_t heap_size = heap.size();
          while (true) {
              size_t largest = idx;
              size_t left = 2 * idx + 1;
              size_t right = 2 * idx + 2;
              
              if (left < heap_size && scores[heap[left]] > scores[heap[largest]])
                  largest = left;
              if (right < heap_size && scores[heap[right]] > scores[heap[largest]])
                  largest = right;
                  
              if (largest == idx) break;
              
              swap(heap[idx], heap[largest]);
              pos_to_idx[heap[idx]] = idx;
              pos_to_idx[heap[largest]] = largest;
              idx = largest;
          }
      }
      
  public:
      HashHeap(size_t size, vector<size_t>& initial_scores) : n(size) {
          pos_to_idx.resize(n, n);
          scores = initial_scores;
          heap.reserve(n);
          
          for (size_t i = 0; i < n; i++) {
              heap.push_back(i);
              pos_to_idx[i] = i;
          }
          
          for (int i = n/2 - 1; i >= 0; i--) {
              heapify_down(i);
          }
      }
      
      void update(size_t pos, size_t new_score) {
          if (pos >= n) return;
          scores[pos] = new_score;
          size_t idx = pos_to_idx[pos];
          if (idx < heap.size()) {
              heapify_up(idx);
              heapify_down(idx);
          }
      }
      
      size_t extract_max() {
          size_t max_pos = heap[0];
          heap[0] = heap.back();
          pos_to_idx[heap[0]] = 0;
          heap.pop_back();
          pos_to_idx[max_pos] = n;
          if (!heap.empty()) heapify_down(0);
          return max_pos;
      }
      
      bool empty() {
          return heap.empty();
      }
      
      size_t size() {
          return heap.size();
      }
  };

void print_debug(const vector<pair<size_t,size_t>>& heights, const bit_vector& bv, unsigned char *text)
{

  for(pair<size_t, size_t> i : heights) cerr << i.first << ", ";
  cerr << "\n";
  for(pair<size_t, size_t> i : heights) cerr << i.second << ", ";
  cerr << "\n";
  for(pair<size_t, size_t> i : heights) cerr << text[i.second] << ", ";
  cerr << "\n";
  for(auto i : bv) cerr << i << ", ";
  cerr << "\n";

}

struct record_pos
{
  size_t sum_height = 0;
  size_t already_seen_positions = 1;
};


record_pos peek_record_positions(size_t middle_pos, const bit_vector& bv, const vector<pair<size_t, size_t>>& heights, size_t k = KMER_SIZE)
{
    record_pos recorded_positions; 
    size_t start;
    if (middle_pos > k / 2)
    {
        start = middle_pos - k / 2;
    }
    else
    {
        start = 0;
    }
    
    size_t end = middle_pos + k / 2;
    if (end > bv.size())
    {
        end = bv.size();
    }
    
    for (size_t i = start; i < end; i++)
    {
        //bv[i] = 1;

        recorded_positions.sum_height += heights[i].first;
        if(bv[i] == 1)
        {
          recorded_positions.already_seen_positions++;
        }
    }

    return recorded_positions;
}

string candidate_string(size_t candidate, bit_vector& bv, unsigned char* text, size_t k = KMER_SIZE)
{
    size_t start;
    string c = "";
    if (candidate > k / 2)
    {
        start = candidate - k / 2;
    }
    else
    {
        start = 0;
    }
    
    size_t end = candidate + k / 2;
    if (end > bv.size())
    {
        end = bv.size();
    }
    
    for (size_t i = start; i < end; i++)
    {
      if(bv[i] != 1)
      c += text[i];

      bv[i] = 1;
      
    }
    cerr << "Candidate string: " << c << "\n";
    return c;
}

size_t score(size_t candidate, bit_vector& bv, const vector<pair<size_t, size_t>>& heights, size_t k = KMER_SIZE)
{
    size_t sum = 0;
    size_t denom = 0;
    size_t start;
    if (candidate > k / 2)
    {
        start = candidate - k / 2;
    }
    else
    {
        start = 0;
    }
    
    size_t end = candidate + k / 2;
    if (end > bv.size())
    {
        end = bv.size();
    }
    
    for (size_t i = start; i < end; i++)
    {
      sum += heights[i].first;
      if(bv[i] == 1)
      {
       denom++; 
      }
    }
    return sum / max(1UL, denom);
}

int main(int argc, char **argv)
{

  if (argc != 2)
  {
    exit(EXIT_FAILURE);
  }

  unsigned char *text;
  int *sa = NULL, length;
  read_text(argv[1], text, length);

  clock_t timestamp;
  long double wtimestamp;

  wtimestamp = wclock();
  read_sa(argv[1], sa, length);
  cerr << "Running algorithm kkp2...\n";
  timestamp = clock();
  size_t n = strlen((const char*)text) - 1;
  vector<pair<size_t,size_t>> heights;
  heights.reserve(n);                    
  for (size_t i = 0; i < n; ++i)
  {
      heights.emplace_back(0, i);
  }
  
  kkp2(text, sa, length, NULL, heights); // creates height vector

  bit_vector bv(n, 0); // creates bitvector for currently selected positions
  
  
  /*
  This snippet initializes the data structures for construction of the
  reference: 
  
  1. peek their recorded positions on the bit vector
  2. score them based on [sum(h(i), k) / recorded_positions]
  3. add the positions, scored, to a heap  
  
  */
  cerr << "Running snippet 1... \n";
  vector<size_t> score_vector(n, 0);
  for(size_t i = 0; i < heights.size(); i++)
  {
    record_pos rp = peek_record_positions(heights[i].second, bv, heights);
    int score = rp.sum_height / max(1UL, rp.already_seen_positions);
    score_vector[i] = score;
  }

  HashHeap pq(n, score_vector);
  
  /*
  This snippet fills the reference.

  1. pop max element
  2. add max element to the reference
  3. find new scores of neighborhood
  4. update the neighborhood scores in the HashHeap
  5. do 1,2,3 & 4 until it fills a reference sized DICTIONARY_SIZE

  */
  cerr << "Size of Heap: " << pq.size() << "\n";
  cerr << "Running snippet 2... \n";
  string reference = "";
  while(reference.size() < DICTIONARY_SIZE && ! pq.empty())
  {
    cerr << "Finding current best candidate...\n";
    size_t candidate = pq.extract_max(); //1
    cerr << "Adding best candidate to the reference...\n";
    reference += candidate_string(candidate, bv, text); //2
    size_t start;
    cerr << "Finding new scores of neighborhood and updating hashheap...\n";
    if (candidate > KMER_SIZE / 2)
    {
        start = candidate - KMER_SIZE / 2;
    }
    else
    {
        start = 0;
    }
    
    size_t end = candidate + KMER_SIZE / 2;
    if (end > bv.size())
    {
        end = bv.size();
    }
    for (size_t i = start; i < end; i++)
    {
      size_t new_score = score(i, bv, heights);
      pq.update(i, new_score);
    }

  }

  //print_debug(heights, bv, text);
  cerr << "\nReference: " << reference << " - End of reference.\n";
  cerr << "\n";
  cerr << "CPU time: " << elapsed(timestamp) << "s\n";
  cerr << "Wallclock time including SA reading: " << welapsed(wtimestamp) << "s\n";



  // Clean up.
  if (sa) delete[] sa;
  delete[] text;
  return EXIT_SUCCESS;
}


//first -> heights
//second -> indexes
