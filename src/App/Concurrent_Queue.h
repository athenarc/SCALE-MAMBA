#ifndef _Concurrent_Queue
#define _Concurrent_Queue

#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

namespace sedp {

  template<typename T>
  class Concurrent_Queue { 
  public:
    void put(const T& val);
    void put(T&& val);
    void get(T& val);

  private:
    mutex mtx;
    condition_variable cond;
    queue<T> q; 
  };
}

template<typename T>
void sedp::Concurrent_Queue<T>::put(const T& val) {
  lock_guard<mutex> lck(mtx);
  q.push(val);
  cond.notify_one();
}

template<typename T>
void sedp::Concurrent_Queue<T>::put(T&& val) {
  lock_guard<mutex> lck(mtx);
  q.push(move(val));
  cond.notify_one();
}

template<typename T>
void sedp::Concurrent_Queue<T>::get(T& val) {
  unique_lock<mutex> lck(mtx);
  cond.wait(lck, [this]{ return !q.empty(); });
  val = move(q.front());
  q.pop();
}

#endif
