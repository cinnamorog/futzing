//
//  lru_cache_test.cpp
//  LRUCache
//
//  Created by Roger Tinkoff on 4/29/23.
//

#include "lru_cache_test.hpp"
#include "lru_cache.hpp"

#include <iostream>

using namespace std;

void LRU_CACHE_TEST_TWO_ITEMS() {
  // Cache with a max of two items.
  int kMaxNumItems = 2;
  LRUCache cache(kMaxNumItems);

  // Insert.
  cache.Put("rose", 10);

  // Verify that a lookup of "rose" gives us a value of 10.
  int value = cache.Get("rose");
  assert(value == 10);

  // Insert one more, both values present.
  cache.Put("mars", 20);
  value = cache.Get("rose");
  assert(value == 10);
  value = cache.Get("mars");
  assert(value == 20);

  // Insert one more, values for "mars" and "zara" are now present
  // but not "rose," who should have been purged as the LRU item.
  cache.Put("zara", 30);
  value = cache.Get("mars");
  assert(value == 20);
  value = cache.Get("zara");
  assert(value == 30);
  value = cache.Get("rose");
  assert(value == -1);
}

void LRU_CACHE_TEST_MANY_ITEMS() {
  // Cache with a bunch of random items.
  int kMaxNumItems = 10000;
  LRUCache cache(kMaxNumItems);
  vector<LinkList::Node::Contents> many_items = LinkList::Node::GetRandomItems(kMaxNumItems);

  // Insert them all.
  for (int i = 0; i < many_items.size(); ++i) {
//    cout << "item " << i << ": id " << get<0>(many_items[i]) << " value " << get<1>(many_items[i]) << endl;
    cache.Put(get<0>(many_items[i]), get<1>(many_items[i]));
  }

  for (int i = 0; i < kMaxNumItems/2; ++i) {
    // Get the list-position of a random node in the cache, that's not
    // already 0 (at the MRU position).
    string id = get<0>(many_items[rand()%kMaxNumItems]);
    int pos = 0;
    while (pos == 0) {
      pos = cache.GetPositionInListForTesting(id);
    }
    assert(pos > 0);

    // Do a Get() on id, verify that it's position afterwards is 0.
    cache.Get(id);
    pos = cache.GetPositionInListForTesting(id);
    assert(pos == 0);
  }
}

void RUN_LRU_CACHE_TESTS() {
  LRU_CACHE_TEST_TWO_ITEMS();
  LRU_CACHE_TEST_MANY_ITEMS();
}
