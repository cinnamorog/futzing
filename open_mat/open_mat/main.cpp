//
//  main.cpp
//  open_mat
//
//  Created by Roger Tinkoff on 4/22/23.
//

#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>
#include <chrono>

using namespace std;

constexpr int kNumRollers = 10;
constexpr int kMatCapacity = 6;
constexpr int kMinSuitUpTimeSec = 5;
constexpr int kSuitUpTimeVariationSec = 2;
constexpr int kMinRollTimeSec = 7;
constexpr int kRollTimeVariationSec = 2;

counting_semaphore<kMatCapacity> sema_(kMatCapacity);

void roller_loop(int id) {
  cout << __FUNCTION__ << ": (" << id << ") waiting" << endl;
  this_thread::sleep_for(std::chrono::seconds(kMinSuitUpTimeSec + rand()%kSuitUpTimeVariationSec));
  sema_.acquire();
  cout << __FUNCTION__ << ": (" << id << ") is OTM" << endl;
  this_thread::sleep_for(std::chrono::seconds(kMinRollTimeSec + rand()%kRollTimeVariationSec));
  sema_.release();
  cout << __FUNCTION__ << ": (" << id << ") has left" << endl;
}

int main(int argc, const
         char * argv[]) {
  vector<thread> rollers;
  rollers.clear();

  for (int i = 0; i < kNumRollers; ++i) {
    this_thread::sleep_for(std::chrono::seconds(1));
    rollers.emplace_back(thread(roller_loop, i));
  }

  for (auto& roller : rollers) {
    roller.join();
  }

  return 0;
}
