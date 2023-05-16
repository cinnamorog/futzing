//
//  lru_cache.hpp
//  LRUCache
//
//  Created by Roger Tinkoff on 4/29/23.
//

#ifndef lru_cache_hpp
#define lru_cache_hpp

#include <string>
#include <unordered_map>

#include "link_list.hpp"

using namespace std;

class LRUCache {
public:
  LRUCache(size_t max_size);
  virtual ~LRUCache();

  void Put(string id, int value);
  int Get(string id);

  int GetPositionInListForTesting(string id);

private:
  size_t max_size_;
  LinkList lru_list_;
  using HashMap = unordered_map<string, shared_ptr<LinkList::Node>>;
  unordered_map<string, shared_ptr<LinkList::Node>> hash_map_;
};

#endif /* lru_cache_hpp */
