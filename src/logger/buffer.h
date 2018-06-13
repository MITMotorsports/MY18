#ifndef _BUFFER_H
#define _BUFFER_H

#include <iterator>

template<typename T>
class CircBuf {
  // Modified version of https://gist.github.com/xstherrera1987/3196485

  // don't use default ctor
  CircBuf();

  const size_t size;
  T *data;
  size_t front;
  size_t count;
public:
  CircBuf(size_t);
  ~CircBuf();

  bool empty() { return count == 0; }
  bool full() { return count == size; }

  bool add(const T&);
  bool remove(T*);
};

template<typename T>
CircBuf<T>::CircBuf(size_t sz): size(sz) {
  data = new T[sz];
  front = 0;
  count = 0;
}

template<typename T>
CircBuf<T>::~CircBuf() {
  delete data;
}

// returns true if add was successful, false if the buffer is already full
template<typename T>
bool CircBuf<T>::add(const T &t) {
  if (full()) {
    return false;
  }
  else {
    // find index where insert will occur
    int end = (front + count) % size;
    data[end] = t;
    count++;
    return true;
  }
}

// returns true if there is something to remove, false otherwise
template<typename T>
bool CircBuf<T>::remove(T *t) {
  if (empty()) {
    return false;
  }
  else {
    *t = data[front];

    front = (front == size - 1) ? 0 : front + 1;
    count--;
    return true;
  }
}

#endif
