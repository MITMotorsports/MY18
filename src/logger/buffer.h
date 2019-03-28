#pragma once

template<typename T, size_t N>
class FIFOCircBuffer {
  // Modified version of https://gist.github.com/xstherrera1987/3196485

private:
  T data[N];
  size_t front;
  size_t count;
public:
  FIFOCircBuffer(): front(0), count(0) {}

  T    first() { return data[front]; }
  T    last()  { return data[(front + count) % N]; }

  bool empty() { return count == 0; }
  bool full()  { return count == N; }

  bool add(const T&);
  bool remove(T*);

  void push(const T&);
  void pop();
};

template<typename T, size_t N>
void FIFOCircBuffer<T, N>::push(const T &t) {
  // find index where insert will occur
  size_t end = (front + count) % N;
  data[end] = t;
  count++;
}

// returns true if add was successful, false if the buffer is already full
template<typename T, size_t N>
bool FIFOCircBuffer<T, N>::add(const T &t) {
  if (full()) return false;

  push(t);
  return true;
}

template<typename T, size_t N>
void FIFOCircBuffer<T, N>::pop() {
  if (!empty()) {
    front = (front == N - 1) ? 0 : front + 1;
    count--;
  }
}

// returns true if there is something to remove, false otherwise
template<typename T, size_t N>
bool FIFOCircBuffer<T, N>::remove(T *t) {
  if (empty()) return false;

  *t = first();
  pop();
  return true;
}
