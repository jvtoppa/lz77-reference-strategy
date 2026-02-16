////////////////////////////////////////////////////////////////////////////////
// kkp.cpp
//   Implementation of main parsing functions.
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

#include <stdint.h>
#include <cstdio>
#include <ctime>
#include <cstdlib>

#include <vector>
#include <algorithm>

#include "kkp.h"
#include "SA_streamer.h"

#define STACK_BITS 16
#define STACK_SIZE (1 << STACK_BITS)
#define STACK_HALF (1 << (STACK_BITS - 1))
#define STACK_MASK ((STACK_SIZE) - 1)

// An auxiliary routine used during parsing.
int parse_phrase(unsigned char *X, int n, int i, int psv, int nsv,
    std::vector<std::pair<int, int> > *F, std::vector<std::pair<size_t,size_t>>& reference_counters);

int kkp2(unsigned char *X, int *SA, int n,
    std::vector<std::pair<int, int> > *F, std::vector<std::pair<size_t,size_t>>& reference_counters) {
  if (n == 0) return 0;
  int *CS = new int[n + 5];
  int *stack = new int[STACK_SIZE + 5], top = 0;
  stack[top] = 0;

  // Compute PSV_text for SA and save into CS.
  CS[0] = -1;
  for (int i = 1; i <= n; ++i) {
    int sai = SA[i - 1] + 1;
    while (stack[top] > sai) --top;
    if ((top & STACK_MASK) == 0) {
      if (stack[top] < 0) {
        // Stack empty -- use implicit.
        top = -stack[top];
        while (top > sai) top = CS[top];
        stack[0] = -CS[top];
        stack[1] = top;
        top = 1;
      } else if (top == STACK_SIZE) {
        // Stack is full -- discard half.
        for (int j = STACK_HALF; j <= STACK_SIZE; ++j)
          stack[j - STACK_HALF] = stack[j];
        stack[0] = -stack[0];
        top = STACK_HALF;
      }
    }

    int addr = sai;
    CS[addr] = std::max(0, stack[top]);
    ++top;
    stack[top] = sai;
  }
  delete[] stack;

  // Compute the phrases.
  CS[0] = 0;
  int nfactors = 0, next = 1, nsv, psv;
  for (int t = 1; t <= n; ++t) {
    psv = CS[t];
    nsv = CS[psv];
    if (t == next) {
      next = parse_phrase(X, n, t - 1, psv - 1, nsv - 1, F, reference_counters) + 1;
      ++nfactors;
    }
    CS[t] = nsv;
    CS[psv] = t;
  }

  // Clean up.
  delete[] CS;
  return nfactors;
}

int parse_phrase(unsigned char *X, int n, int i, int psv, int nsv,
                 std::vector<std::pair<int, int>> *F, std::vector<std::pair<size_t,size_t>>& reference_counters)
{
  int pos, len = 0;

  if (nsv == -1)
  {
    while (psv + len < n && i + len < n && X[psv + len] == X[i + len]) {
      int ref_pos = psv + len;
      if (ref_pos >= 0 && ref_pos < n) {
        ++reference_counters[ref_pos].first;
      }
      ++len;
    }
    pos = psv;
  }
  else if (psv == -1)
  {
    while (i + len < n && X[nsv + len] == X[i + len]) {
      int ref_pos = nsv + len;
      if (ref_pos >= 0 && ref_pos < n) {
        ++reference_counters[ref_pos].first;
      }
      ++len;
    }
    pos = nsv;
  }
  else
  {
    while (psv + len < n && nsv + len < n && X[psv + len] == X[nsv + len]) {
      int ref_pos = psv + len;
      if (ref_pos >= 0 && ref_pos < n) {
        ++reference_counters[ref_pos].first;
      }
      ++len;
    }

    if (i + len < n && X[i + len] == X[psv + len])
    {
      int ref_pos = psv + len;
      if (ref_pos >= 0 && ref_pos < n) {
        ++reference_counters[ref_pos].first;
      }
      ++len;
      
      while (i + len < n && X[i + len] == X[psv + len]) {
        ref_pos = psv + len;
        if (ref_pos >= 0 && ref_pos < n) {
          ++reference_counters[ref_pos].first;
        }
        ++len;
      }
      pos = psv;
    }
    else
    {
      while (i + len < n && X[i + len] == X[nsv + len]) {
        int ref_pos = nsv + len;
        if (ref_pos >= 0 && ref_pos < n) {
          ++reference_counters[ref_pos].first;
        }
        ++len;
      }
      pos = nsv;
    }
  }

  if (len == 0) {
    pos = X[i];
  }
  
  if (F) {
    F->push_back(std::make_pair(pos, len));
  }
  
  return i + std::max(1, len);
}