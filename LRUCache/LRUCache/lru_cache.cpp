//
//  lru_cache.cpp
//  LRUCache
//
//  Created by Roger Tinkoff on 4/29/23.
//

#include "lru_cache.hpp"

LRUCache::LRUCache(size_t max_size) : max_size_(max_size) {
  lru_list_.Clear();
  hash_map_.clear();
}

LRUCache::~LRUCache() {
  lru_list_.Clear();
  hash_map_.clear();
}

void LRUCache::Put(string id, int value) {
  // Check the current size, if we're at capacity we need to remove
  // the least-recently-used item.
  if (hash_map_.size() == max_size_) {
    // Get the LRU node.
    shared_ptr<LinkList::Node> lru_node = lru_list_.GetTailShared();

    // Erase it from the list.
    optional<LinkList::Node::Contents> tail_contents = lru_list_.PopTail();
    assert(tail_contents.has_value());

    // Erase it from the hash map.
    hash_map_.erase(get<0>(tail_contents.value()));
  }

  // Insert contents in the LRU list, it is now the most-recently-used.
  lru_list_.PushHead(make_tuple(id, value));

  // Get the head node itself, insert it in our hash map by ID.
  shared_ptr<LinkList::Node> node = lru_list_.GetHeadShared();
  hash_map_[id] = node;
}

int LRUCache::Get(string id) {
  HashMap::iterator it = hash_map_.find(id);
  if (it != hash_map_.end()) {
    shared_ptr<LinkList::Node>& node = it->second;
    lru_list_.PromoteNodeHead(node);
    return get<1>(node->contents_);
  }

  return -1;
}

int LRUCache::GetPositionInListForTesting(string id) {
  int pos = 0;
  LinkList::Node *node = lru_list_.GetHead();

  while (node) {
    if (get<0>(node->contents_) == id) {
      return pos;
    }
    node = node->next_.get();
    ++pos;
  }

  return pos;
}

