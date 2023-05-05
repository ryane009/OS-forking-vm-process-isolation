#include "synchronized_queue.hpp"

template <typename T>
size_t synchronized_queue<T>::size() {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock guard(mtx);
  return this->q.size();
}

template <typename T>
bool synchronized_queue<T>::pop(T* elt) {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock guard(mtx);
  if(is_stopped==true){
    return true;
  }
  while(q.size()==0){
    cv.wait(guard);
    if(is_stopped==true){
    return true;
  }
  }
  *elt = q.front();
  q.pop();
  return false;
}

template <typename T>
void synchronized_queue<T>::push(T elt) {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock guard(mtx);
  if(q.empty()){
    cv.notify_all();
  }
  q.push(elt);
}

template <typename T>
std::vector<T> synchronized_queue<T>::flush() {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock guard(mtx);
  std::vector<T> contents;
  while(q.size() > 0){
    contents.push_back(q.front());
    q.pop();
  }
  return contents;
}

template <typename T>
void synchronized_queue<T>::stop() {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock guard(mtx);
  is_stopped = true;
  cv.notify_all();
}

// NOTE: DO NOT TOUCH! Why is this necessary? Because C++ is a beautiful
// language:
// https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class synchronized_queue<int>;
template class synchronized_queue<std::shared_ptr<ClientConn>>;
