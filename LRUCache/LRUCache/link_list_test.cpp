//
//  link_list_test.cpp
//  LRUCache
//
//  Created by Roger Tinkoff on 4/29/23.
//

#include "link_list_test.hpp"

#include <vector>
#include <memory>
#include <string>

#include "link_list.hpp"

bool VectorsEqual(vector<LinkList::Node::Contents>& v1, vector<LinkList::Node::Contents>& v2) {
  if (v1.size() != v2.size()) {
    return false;
  }

  for (int i = 0; i < v1.size(); ++i) {
    if (v1[i] != v2[i]) {
      return false;
    }
  }

  return true;
}

void LINK_LIST_TEST_ONE_ITEM() {
  LinkList list;

  // Empty list, no contents.
  assert(list.IsEmpty());
  assert(list.PeekHead() == nullopt);
  assert(list.PeekTail() == nullopt);

  // Push a contents at the head, verify it's there and is the only contents.
  LinkList::Node::Contents rose_node = make_tuple("rose", 10);
  list.PushHead(rose_node);
  assert(!list.IsEmpty());
  assert(list.PeekHead() == rose_node);
  assert(list.PeekTail() == rose_node);

  // Pop the contents, list is empty again.
  list.PopTail();
  assert(list.IsEmpty());
  assert(list.PeekHead() == nullopt);
  assert(list.PeekTail() == nullopt);

  // Push a contents at the tail, verify it's there and is the only contents.
  list.PushTail(rose_node);
  assert(!list.IsEmpty());
  assert(list.PeekHead() == rose_node);
  assert(list.PeekTail() == rose_node);

  // Pop the contents from the head, list is empty again.
  list.PopHead();
  assert(list.IsEmpty());
  assert(list.PeekHead() == nullopt);
  assert(list.PeekTail() == nullopt);
}

void LINK_LIST_TEST_MULTIPLE_ITEMS() {
  LinkList list;

  // Insert a vector of contents, verify that they are present in the
  // list and in the order we expect, if we walk it both fromn head
  // to tail and tail to head.
  LinkList::Node::Contents rose_node = make_tuple("rose", 10);
  LinkList::Node::Contents mars_node = make_tuple("mars", 20);
  LinkList::Node::Contents zara_node = make_tuple("zara", 30);
  vector<LinkList::Node::Contents> vIncreasing = { rose_node, mars_node, zara_node };
  vector<LinkList::Node::Contents> vDecreasing = { zara_node, mars_node, rose_node };

  // Push vIncreasing at the head, walking head to tail is vDecreasing,
  // tail to head is vIncreasing.
  list.PushHead(vIncreasing);
  vector<LinkList::Node::Contents> walked = list.WalkHeadToTail();
  assert(VectorsEqual(walked, vDecreasing));
  walked = list.WalkTailToHead();
  assert(VectorsEqual(walked, vIncreasing));

  // Test Clear().
  list.Clear();
  assert(list.IsEmpty());
  assert(list.PeekHead() == nullopt);
  assert(list.PeekTail() == nullopt);

  // Push vDecreasing at the head, walking head to tail is vIncreasing,
  // tail to head is vDecreasing.
  list.PushHead(vDecreasing);
  walked = list.WalkHeadToTail();
  assert(VectorsEqual(walked, vIncreasing));
  walked = list.WalkTailToHead();
  assert(VectorsEqual(walked, vDecreasing));

  // Same set of tests, but push at the tail instead.
  list.Clear();
  list.PushTail(vIncreasing);
  walked = list.WalkHeadToTail();
  assert(VectorsEqual(walked, vIncreasing));
  walked = list.WalkTailToHead();
  assert(VectorsEqual(walked, vDecreasing));
  list.Clear();
  list.PushTail(vDecreasing);
  walked = list.WalkHeadToTail();
  assert(VectorsEqual(walked, vDecreasing));
  walked = list.WalkTailToHead();
  assert(VectorsEqual(walked, vIncreasing));

  // Clear, insert vIncreasing, verify.
  list.Clear();
  list.PushTail(vIncreasing);
  walked = list.WalkHeadToTail();
  assert(VectorsEqual(walked, vIncreasing));
}

void LINK_LIST_TEST_REF_COUNTS() {
  LinkList list;
  LinkList::Node::Contents rose_node = make_tuple("rose", 10);
  LinkList::Node::Contents mars_node = make_tuple("mars", 20);
  LinkList::Node::Contents zara_node = make_tuple("zara", 30);

  // Empty list, no contents.
  assert(list.IsEmpty());
  assert(list.PeekHead() == nullopt);
  assert(list.PeekTail() == nullopt);

  // Push a contents at the head, verify it's there and is the only contents.
  list.PushHead(rose_node);
  assert(!list.IsEmpty());
  assert(list.PeekHead() == rose_node);
  assert(list.PeekTail() == rose_node);

  // The ref count should be 2, because it's both the head and tail.
  long ref_count = list.GetNodeWithContentsRefCount(rose_node);
  assert(ref_count == 2);

  // Pop the contents, list is empty again.
  list.PopTail();
  assert(list.IsEmpty());
  assert(list.PeekHead() == nullopt);
  assert(list.PeekTail() == nullopt);

  // Push a contents at the tail, verify it's there and is the only contents,
  // and the ref count is 2.
  list.PushTail(rose_node);
  assert(!list.IsEmpty());
  assert(list.PeekHead() == rose_node);
  assert(list.PeekTail() == rose_node);
  ref_count = list.GetNodeWithContentsRefCount(rose_node);
  assert(ref_count == 2);

  // Push a second node.
  list.PushHead(mars_node);

  // First node has a ref count of 2 (list holds a ref to it as the tail,
  // the other node holds a ref as its next node.
  ref_count = list.GetNodeWithContentsRefCount(rose_node);
  assert(ref_count == 2);

  // Second node has a ref count of 2 (list holds a ref to it as the head,
  // the other node holds a ref as its previous node.
  ref_count = list.GetNodeWithContentsRefCount(mars_node);
  assert(ref_count == 2);

  // Push a third node.
  list.PushHead(zara_node);

  // Rose still has a ref count of 2 (list holds a ref to it as the tail,
  // Mars holds a ref as its next node).
  ref_count = list.GetNodeWithContentsRefCount(rose_node);
  assert(ref_count == 2);

  // Mars has a ref count of 2 (Rose holds a ref as its previous node,
  // Zara holds a ref as its next).
  ref_count = list.GetNodeWithContentsRefCount(mars_node);
  assert(ref_count == 2);

  // Zara has a ref count of 2 (list holds a ref to it as the head,
  // Mars holds a ref as its previous node).
  ref_count = list.GetNodeWithContentsRefCount(zara_node);
  assert(ref_count == 2);

  // Clear the list, verify that it's empty again.
  list.Clear();
  assert(list.IsEmpty());
  assert(list.PeekHead() == nullopt);
  assert(list.PeekTail() == nullopt);
}

void LINK_LIST_TEST_PROMOTE() {
  LinkList list;
  LinkList::Node::Contents rose_node = make_tuple("rose", 10);
  LinkList::Node::Contents mars_node = make_tuple("mars", 20);
  LinkList::Node::Contents zara_node = make_tuple("zara", 30);
  vector<LinkList::Node::Contents> vIncreasing = { rose_node, mars_node, zara_node };
  vector<LinkList::Node::Contents> vDecreasing = { zara_node, mars_node, rose_node };

  // Push nodes onto the tail, so Rose is the head and Zara the tail.
  list.PushTail(rose_node);
  list.PushTail(mars_node);
  list.PushTail(zara_node);

  // If we promote the node at the head, list should be unchanged.
  shared_ptr<LinkList::Node> node = list.GetHeadShared();
  assert(node->contents_ == rose_node);
  list.PromoteNodeHead(node);
  vector<LinkList::Node::Contents> walked = list.WalkHeadToTail();
  assert(VectorsEqual(walked, vIncreasing));

  // All ref counts should be 2 except for Rose, whose ref count
  // is 3 because we have a local shared_ptr<> to it.
  long ref_count = list.GetNodeWithContentsRefCount(rose_node);
  assert(ref_count == 3);
  ref_count = list.GetNodeWithContentsRefCount(mars_node);
  assert(ref_count == 2);
  ref_count = list.GetNodeWithContentsRefCount(zara_node);
  assert(ref_count == 2);

  // Advance to the next node (Mars), promote it, verify the order.
  node = node->next_;
  assert(node);
  assert(node->contents_ == mars_node);
  list.PromoteNodeHead(node);
  vector<LinkList::Node::Contents> vPromotedForward = { mars_node, rose_node, zara_node };
  vector<LinkList::Node::Contents> vPromotedReversed = { zara_node, rose_node, mars_node };
  walked = list.WalkHeadToTail();
  assert(VectorsEqual(walked, vPromotedForward));
  walked = list.WalkTailToHead();
  assert(VectorsEqual(walked, vPromotedReversed));

  // All ref counts should be 2 except for Mars, whose ref count
  // is 3 because we have a local shared_ptr<> to it.
  ref_count = list.GetNodeWithContentsRefCount(rose_node);
  assert(ref_count == 2);
  ref_count = list.GetNodeWithContentsRefCount(mars_node);
  assert(ref_count == 3);
  ref_count = list.GetNodeWithContentsRefCount(zara_node);
  assert(ref_count == 2);

  // Now promote the tail to the head, verify.
  node = list.GetTailShared();
  assert(node);
  assert(node->contents_ == zara_node);
  list.PromoteNodeHead(node);
  vPromotedForward = { zara_node, mars_node, rose_node };
  vPromotedReversed = { rose_node, mars_node, zara_node };
  walked = list.WalkHeadToTail();
  assert(VectorsEqual(walked, vPromotedForward));
  walked = list.WalkTailToHead();
  assert(VectorsEqual(walked, vPromotedReversed));

  // All ref counts should be 2 except for Zara, whose ref count
  // is 3 because we have a local shared_ptr<> to it.
  ref_count = list.GetNodeWithContentsRefCount(rose_node);
  assert(ref_count == 2);
  ref_count = list.GetNodeWithContentsRefCount(mars_node);
  assert(ref_count == 2);
  ref_count = list.GetNodeWithContentsRefCount(zara_node);
  assert(ref_count == 3);

  // Clear the list, verify that it's empty again.
  list.Clear();
  assert(list.IsEmpty());
  assert(list.PeekHead() == nullopt);
  assert(list.PeekTail() == nullopt);
}


void RUN_LINK_LIST_TESTS() {
  LINK_LIST_TEST_ONE_ITEM();
  LINK_LIST_TEST_MULTIPLE_ITEMS();
  LINK_LIST_TEST_REF_COUNTS();
  LINK_LIST_TEST_PROMOTE();
}
