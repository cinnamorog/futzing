//
//  main.cpp
//  trie
//
//  Created by Roger Tinkoff on 4/24/23.
//

#include <iostream>
#include <string>

using namespace std;

// Allow only lowercase characters a-z.
constexpr int kNumCharacters = 26;

class TrieNode {
public:
  TrieNode() {
    memset(child_, 0, sizeof(child_));
    is_terminal_ = false;
  }
  virtual ~TrieNode() {}

  TrieNode* child(char letter) { return child_[letter - 'a']; }
  void set_child(char letter, TrieNode* node) { child_[letter - 'a'] = node; }
  bool is_terminal() { return is_terminal_; }
  void set_is_terminal(bool is_terminal) { is_terminal_ = is_terminal; }

private:
  TrieNode* child_[kNumCharacters];
  bool is_terminal_;
};

class Trie {
public:
  Trie() : root_(new TrieNode()) {}
  virtual ~Trie() { delete root_; }

  bool Search(const string& word) {
    TrieNode *node = root_;

    for (int i = 0; i < word.length(); ++i) {
      if (node->child(word[i])) {
        node = node->child(word[i]);
        continue;
      }

      return false;
    }

    return node->is_terminal();
  }

  void InsertWord(const string& word) {
    TrieNode *node = root_;

    for (int i = 0; i < word.length(); ++i) {
      if (!node->child(word[i])) {
        node->set_child(word[i], new TrieNode());
      }

      node = node->child(word[i]);
    }

    node->set_is_terminal(true);
  }

private:
  TrieNode *root_;
};

void RUN_ALL_TESTS() {
  Trie trie;
  const string kTrieStr = "trie";
  bool present = trie.Search(kTrieStr);
  assert(!present);
  trie.InsertWord(kTrieStr);
  present = trie.Search(kTrieStr);
  assert(present);
  const string kBoolStr = "bool";
  const string kBooleanStr = "boolean";
  trie.InsertWord(kBooleanStr);
  present = trie.Search(kBooleanStr);
  assert(present);
  present = trie.Search(kBoolStr);
  assert(!present);
}

int main(int argc, const char * argv[]) {
  RUN_ALL_TESTS();
  return 0;
}
