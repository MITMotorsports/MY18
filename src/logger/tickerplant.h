#pragma once

#include <vector>

template<typename T>
class Tickerplant {
private:
  typedef void (* Subscriber)(T);
  // don't use default ctor
  Tickerplant();

public:
  std::vector<Subscriber> subscribers;

  Tickerplant(std::initializer_list<Subscriber> il): subscribers(il) {}

  void publish(T data) { for (const Subscriber s : subscribers) s(data); }
};
