#include "concurrent_kvstore.hpp"

#include <optional>
#include <set>

bool ConcurrentKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::shared_lock guard(store.mutex_list[store.bucket(req->key)]);
  std::optional<DbItem> element = store.getIfExists(store.bucket(req->key), req->key);
  if(element != std::nullopt){
    res->value = element->value;
    return true;
  }
  return false;
}

bool ConcurrentKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
    std::unique_lock guard(store.mutex_list[store.bucket(req->key)]);
    store.insertItem(store.bucket(req->key), req->key, req->value);
    return true;
}

bool ConcurrentKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::unique_lock guard(store.mutex_list[store.bucket(req->key)]);
  std::optional<DbItem> element = store.getIfExists(store.bucket(req->key), req->key);
  if(element != std::nullopt){
    store.insertItem(store.bucket(req->key), req->key, element->value.append(req->value));
    return true;
  }
  else{
    store.insertItem(store.bucket(req->key), req->key, req->value);
  }
  
  return true;
}

bool ConcurrentKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::unique_lock guard(store.mutex_list[store.bucket(req->key)]);
  std::optional<DbItem> element = store.getIfExists(store.bucket(req->key), req->key);
  if(element == std::nullopt){
    return false;
  }
  res->value = element->value;
    store.removeItem(store.bucket(req->key), req->key);
    return true;
}

bool ConcurrentKvStore::MultiGet(const MultiGetRequest* req,
                                 MultiGetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::set<size_t> buckets;
  for (auto& key : req->keys) {
    buckets.insert(store.bucket(key));
  }

  std::set<size_t>::iterator it;
  for (it = buckets.begin(); it != buckets.end(); it++){
    store.mutex_list[*it].lock_shared();
  }

  for(auto& key : req->keys){
    std::optional<DbItem> elt = store.getIfExists(store.bucket(key), key);
    if (elt != std::nullopt) {
        res->values.push_back(elt->value);
      } 
      else {
        for (it = buckets.begin(); it != buckets.end(); it++){
          store.mutex_list[*it].unlock_shared();
        }
        return false;
      }
  }
  for (it = buckets.begin(); it != buckets.end(); it++){
    store.mutex_list[*it].unlock_shared();
  }
  
  return true;
}

bool ConcurrentKvStore::MultiPut(const MultiPutRequest* req,
                                 MultiPutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::set<size_t> buckets;
  for (auto& key : req->keys) {
    buckets.insert(store.bucket(key));
  }

  std::set<size_t>::iterator it;
  for (it = buckets.begin(); it != buckets.end(); it++){
    store.mutex_list[*it].lock();
  }

  if (req->keys.size() == req->values.size()) {
      for (size_t i = 0; i < req->keys.size(); ++i) {
        store.insertItem(store.bucket(req->keys[i]), req->keys[i], req->values[i]);
      }
      for (it = buckets.begin(); it != buckets.end(); it++){
        store.mutex_list[*it].unlock();
      }
      return true;
  }

  for (it = buckets.begin(); it != buckets.end(); it++){
    store.mutex_list[*it].unlock();
  }
  return false;
}

std::vector<std::string> ConcurrentKvStore::AllKeys() {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::set<size_t> buckets;
  for(auto& bucket : store.buckets){
    for(auto& item : bucket){
      buckets.insert(store.bucket(item.key));
    }
  }

  std::set<size_t>::iterator it;
  for (it = buckets.begin(); it != buckets.end(); it++){
    store.mutex_list[*it].lock_shared();
  }

  std::vector<std::string> keys;
  for (auto& bucket : store.buckets) {
    for(auto& item : bucket){
        keys.push_back(item.key);
    }
  }

  for (it = buckets.begin(); it != buckets.end(); it++){
    store.mutex_list[*it].unlock_shared();
  }
  return keys;
}
