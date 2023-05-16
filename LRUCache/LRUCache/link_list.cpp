//
//  link_list.cpp
//  LRUCache
//
//  Created by Roger Tinkoff on 4/29/23.
//

#include "link_list.hpp"

#define LOG_REF_COUNT(node) \
  cout << __FUNCTION__ << " line: " << __LINE__ << " "#node".use_count() " << node.use_count() << endl;

namespace  {

string GetRandomString(int length) {
  string s("");

  for (int i = 0; i < length; ++i) {
    s.push_back('a' + rand()%('z' - 'a'));
  }

  return s;
}

bool IsContentsPresent(const vector<LinkList::Node::Contents>&items, const LinkList::Node::Contents& contents) {
  for (size_t i = 0; i < items.size(); ++i) {
    if (get<0>(items[i]) == get<0>(contents)) {
      return true;
    }
  }
  return false;
}

} // namespace

/*static*/ vector<LinkList::Node::Contents> LinkList::Node::GetRandomItems(int num_items) {
  vector<LinkList::Node::Contents> items;
  items.clear();
  constexpr int kStringLen = 6;
  constexpr int kMaxValue = 1000000;

  for (int i = 0; i < num_items; ++i) {
    bool present = true;
    LinkList::Node::Contents contents;
    while (present) {
      contents = make_tuple<string, int>(GetRandomString(kStringLen), rand()%kMaxValue);
      present = IsContentsPresent(items, contents);
    }
    items.push_back(contents);
  }

  return items;
}

LinkList::LinkList() : head_(nullptr), tail_(nullptr) {
}

LinkList::~LinkList() {
  Clear();
}

long LinkList::GetNodeWithContentsRefCount(LinkList::LinkList::Node::Contents contents) {
  shared_ptr<Node> node = head_;
  while (node) {
    if (node->contents_ == contents) {
      // Subtract 1, to account for the reference added by the search.
      return node.use_count() - 1;
    }
    node = node->next_;
  }

  return -1;
}

bool LinkList::IsEmpty() {
  return !head_ && !tail_;
}

void LinkList::Clear() {
  while (!IsEmpty()) {
    PopHead();
  }
}

// Returns the contents at the head.
optional<LinkList::Node::Contents> LinkList::PeekHead() {
  if (!head_) {
    assert(!tail_);
    return nullopt;
  }

  return head_->contents_;
}

// Returns the contents at the tail.
optional<LinkList::Node::Contents> LinkList::PeekTail() {
  if (!tail_) {
    assert(!head_);
    return nullopt;
  }

  return tail_->contents_;
}

// Inserts at the head.
void LinkList::PushHead(Node::Contents contents) {
  if (CheckInsertEmpty(contents)) {
    return;
  }

  shared_ptr<Node> node = make_shared<Node>(contents);

  if (head_ == tail_) {
    head_ = node;
    head_->next_ = tail_;
    tail_->prev_ = head_;
    return;
  }

  node->next_ = head_;
  node->next_->prev_ = node;
  head_ = node;
}

void LinkList::PushHead(const vector<LinkList::Node::Contents>& contents) {
  for (int i = 0; i < contents.size(); ++i) {
    PushHead(contents[i]);
  }
}

// Inserts at the tail.
void LinkList::PushTail(LinkList::Node::Contents contents) {
  if (CheckInsertEmpty(contents)) {
    return;
  }

  shared_ptr<Node> node = make_shared<Node>(contents);

  if (head_ == tail_) {
    tail_ = node;
    tail_->prev_ = head_;
    head_->next_ = tail_;
    return;
  }

  node->prev_ = tail_;
  node->prev_->next_ = node;
  tail_ = node;
}

void LinkList::PushTail(const vector<LinkList::Node::Contents>& contents) {
  for (int i = 0; i < contents.size(); ++i) {
    PushTail(contents[i]);
  }
}

// Removes from the head.
optional<LinkList::Node::Contents> LinkList::PopHead() {
  if (!head_) {
    // List is empty.
    assert(!tail_);
    return nullopt;
  }

  if (head_ && head_ == tail_) {
    // Only one item.
    Node* node = head_.get();
    Node::Contents contents = node->contents_;
    head_ = tail_ = nullptr;
    return contents;
  }

  Node* node = head_.get();
  Node::Contents contents = node->contents_;
  head_ = node->next_;
  tail_->prev_ = nullptr;
  return contents;
}

// Removes from the tail.
optional<LinkList::Node::Contents> LinkList::PopTail() {
  if (!tail_) {
    // List is empty.
    assert(!head_);
    return nullopt;
  }

  if (tail_ && tail_ == head_) {
    // Only one item.
    Node* node = tail_.get();
    Node::Contents contents = node->contents_;
    head_ = tail_ = nullptr;
    return contents;
  }

  Node* node = tail_.get();
  Node::Contents contents = node->contents_;
  tail_ = node->prev_;
  tail_->next_ = nullptr;
  return contents;
}

// Moves node from its current position to the head of the list.
void LinkList::PromoteNodeHead(shared_ptr<Node> node) {
  if (node == head_) {
    // Nothing to do.
    return;
  }

  if (node == tail_) {
    // Move tail up.
    tail_ = node->prev_;
  }

  if (node->prev_) {
    node->prev_->next_ = node->next_;
  }
  if (node->next_) {
    node->next_->prev_ = node->prev_;
  }

  // Attach node at the head, update head_.
  node->next_ = head_;
  node->prev_ = nullptr;
  head_->prev_ = node;
  head_ = node;
}

vector<LinkList::Node::Contents> LinkList::WalkHeadToTail() {
  vector<Node::Contents> v;
  v.clear();

  Node* node = head_.get();
  while (node) {
    v.push_back(node->contents_);
    node = node->next_.get();
  }

  return v;
}

vector<LinkList::Node::Contents> LinkList::WalkTailToHead() {
  vector<Node::Contents> v;
  v.clear();

  Node* node = tail_.get();
  while (node) {
    v.push_back(node->contents_);
    node = node->prev_.get();
  }

  return v;
}

// Checks if the list is empty and inserts contents if it is,
// return true in that case, false otherwise.
bool LinkList::CheckInsertEmpty(Node::Contents contents) {
  if (!head_) {
    assert(!tail_);
    head_ = make_shared<Node>(contents);
    tail_ = head_;
    return true;
  }

  return false;
}
