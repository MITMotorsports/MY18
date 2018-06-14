#pragma once

#include <array>

template<typename T, size_t N>
class FIFOCircBuffer {
  // Modified version of https://gist.github.com/xstherrera1987/3196485

private:
  std::array<T, N> data;
  size_t front;
  size_t count;
public:
  FIFOCircBuffer(): front(0), count(0) {}

  T    first() { return data[front]; }
  T    last()  { return data[(front + count) % data.size()]; }

  bool empty() { return count == 0; }
  bool full()  { return count == data.size(); }

  bool add(const T&);
  bool remove(T*);
};

// returns true if add was successful, false if the buffer is already full
template<typename T, size_t N>
bool FIFOCircBuffer<T, N>::add(const T &t) {
  if (full()) {
    return false;
  }
  else {
    // find index where insert will occur
    size_t end = (front + count) % data.size();
    data[end] = t;
    count++;
    return true;
  }
}

// returns true if there is something to remove, false otherwise
template<typename T, size_t N>
bool FIFOCircBuffer<T, N>::remove(T *t) {
  if (empty()) {
    return false;
  }
  else {
    *t = data[front];

    front = (front == data.size() - 1) ? 0 : front + 1;
    count--;
    return true;
  }
}
