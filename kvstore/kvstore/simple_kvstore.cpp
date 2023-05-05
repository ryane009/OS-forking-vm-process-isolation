#include "simple_kvstore.hpp"

bool SimpleKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
    std::unique_lock guard(mutex);
    if(key_value_store.count(req->key)){
    res->value = key_value_store[req->key];
    return true;
  }
  return false;
}

bool SimpleKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
    std::unique_lock guard(mutex);
    key_value_store[req->key] = req->value;
    return true;
}

bool SimpleKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  std::unique_lock guard(mutex);
  if (key_value_store.count(req->key)){
    key_value_store[req->key].append(req->value);
  }
  else {
    key_value_store[req->key] = req->value;
  }
  return true;
}

bool SimpleKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  if(key_value_store.count(req->key)){
    std::unique_lock guard(mutex);
    res->value = key_value_store[req->key];
    key_value_store.erase(req->key);
    return true;
  }
  return false;
}

bool SimpleKvStore::MultiGet(const MultiGetRequest* req,
                             MultiGetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  std::unique_lock guard(mutex);
  for(auto& key : req->keys){
    if (key_value_store.count(key)) {
        res->values.push_back(key_value_store[key]);
      } 
      else {
        mutex.unlock();
        return false;
      }
  }
  return true;
}

bool SimpleKvStore::MultiPut(const MultiPutRequest* req, MultiPutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  if (req->keys.size() == req->values.size()) {
      std::unique_lock guard(mutex);
      for (size_t i = 0; i < req->keys.size(); ++i) {
        key_value_store[req->keys[i]] = req->values[i];
      }
      return true;
    }
    return false;
}

std::vector<std::string> SimpleKvStore::AllKeys() {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  std::vector<std::string> keys;
  std::unique_lock guard(mutex);
  for (auto &key : key_value_store) {
    keys.push_back(key.first);
  }
  return keys;
}
