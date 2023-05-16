//
//  link_list.hpp
//  LRUCache
//
//  Created by Roger Tinkoff on 4/29/23.
//

#ifndef link_list_hpp
#define link_list_hpp

#include <memory>
#include <optional>
#include <vector>
#include <string>

using namespace std;

class LinkList {
public:
  class Node {
  public:
    using Contents = tuple<string, int>;

    static vector<Contents> GetRandomItems(int num_items);

    Node(Contents contents) : contents_(contents), next_(nullptr), prev_(nullptr) {}
    virtual ~Node() {}

    Contents contents_;
    shared_ptr<Node> next_;
    shared_ptr<Node> prev_;
  };

  LinkList();
  virtual ~LinkList();

  // FOR TESTING ONLY Performs a linear search for the shared_ptr<Node>
  // that contains contents and returns its ref count, or -1 if no
  // such node is found. Functions that return a shared_ptr<> would
  // themselves increase the reference count, and don't want that here.
  long GetNodeWithContentsRefCount(Node::Contents contents);

  // Returns a "raw" pointer to the head/tail.
  Node* GetHead() { return head_.get(); }
  Node* GetTail() { return tail_.get(); }

  // Returns a shared_ptr to the head/tail. Be careful how these
  // are used, as not calling reset() on the return value could
  // prevent the Node from being deallocated when we expect.
  shared_ptr<Node> GetHeadShared() { return head_; }
  shared_ptr<Node> GetTailShared() { return tail_; }

  bool IsEmpty();
  void Clear();

  // Returns the contents at the head.
  optional<Node::Contents> PeekHead();

  // Returns the contents at the tail.
  optional<Node::Contents> PeekTail();

  // Inserts at the head.
  void PushHead(Node::Contents contents);
  void PushHead(const vector<Node::Contents>& contents);

  // Inserts at the tail.
  void PushTail(Node::Contents contents);
  void PushTail(const vector<Node::Contents>& contents);

  // Removes from the head.
  optional<Node::Contents> PopHead();

  // Removes from the tail.
  optional<Node::Contents> PopTail();

  // Moves node from its current position to the head of the list.
  void PromoteNodeHead(shared_ptr<Node> node);

  vector<Node::Contents> WalkHeadToTail();
  vector<Node::Contents> WalkTailToHead();

private:
  // Checks if the list is empty and inserts contents if it is,
  // return true in that case, false otherwise.
  bool CheckInsertEmpty(Node::Contents contents);

  shared_ptr<Node> head_;
  shared_ptr<Node> tail_;
};

#endif /* link_list_hpp */
