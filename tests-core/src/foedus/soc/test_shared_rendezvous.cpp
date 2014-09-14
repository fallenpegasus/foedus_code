/*
 * Copyright (c) 2014, Hewlett-Packard Development Company, LP.
 * The license and distribution terms for this file are placed in LICENSE.txt.
 */
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "foedus/test_common.hpp"
#include "foedus/soc/shared_rendezvous.hpp"

/**
 * @file test_shared_rendezvous.cpp
 * Testcases for foedus::soc::SharedRendezvous.
 * Yes, shamelessly copy-pasted from test_rendezvous.cpp.
 */
namespace foedus {
namespace soc {
DEFINE_TEST_CASE_PACKAGE(SharedRendezvousTest, foedus.soc);

TEST(SharedRendezvousTest, Instantiate) {
  SharedRendezvous rendezvous;
}
TEST(SharedRendezvousTest, Signal) {
  SharedRendezvous rendezvous;
  EXPECT_FALSE(rendezvous.is_signaled_weak());
  EXPECT_FALSE(rendezvous.is_signaled());
  rendezvous.signal();
  EXPECT_TRUE(rendezvous.is_signaled());
  EXPECT_TRUE(rendezvous.is_signaled_weak());
}



TEST(SharedRendezvousTest, Simple) {
  SharedRendezvous rendezvous;
  std::atomic<bool> ends(false);
  // I don't like [=] it's a bit handwavy syntax.
  std::thread another([](SharedRendezvous *rendezvous_, std::atomic<bool> *ends_){
    rendezvous_->wait();
    ends_->store(true);
  }, &rendezvous, &ends);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  EXPECT_FALSE(rendezvous.is_signaled_weak());
  EXPECT_FALSE(rendezvous.is_signaled());
  EXPECT_FALSE(ends.load());
  rendezvous.signal();
  EXPECT_TRUE(rendezvous.is_signaled());
  EXPECT_TRUE(rendezvous.is_signaled_weak());

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_TRUE(ends.load());
  another.join();
}

const int kRep = 300;
const int kClients = 4;
std::vector<SharedRendezvous*>      many_rendezvous;
std::vector< std::atomic<int>* >    many_ends;
void many_thread() {
  for (int i = 0; i < kRep; ++i) {
    many_rendezvous[i]->wait();
    EXPECT_TRUE(many_rendezvous[i]->is_signaled());
    int added = ++(*many_ends[i]);
    if (added == kClients) {
      delete many_rendezvous[i];
    }
  }
}
TEST(SharedRendezvousTest, Many) {
  // This tests 1) spurious wake up, 2) lost signal (spurious blocking), 3) and other anomalies.
  for (int i = 0; i < kRep; ++i) {
    many_rendezvous.push_back(new SharedRendezvous());
    many_ends.push_back(new std::atomic<int>(0));
  }

  std::vector<std::thread> clients;
  for (int i = 0; i < kClients; ++i) {
    clients.emplace_back(std::thread(&many_thread));
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  for (int i = 0; i < kRep; ++i) {
    EXPECT_EQ(0, many_ends[i]->load());
  }

  for (int i = 0; i < kRep; ++i) {
    EXPECT_EQ(0, many_ends[i]->load());
    many_rendezvous[i]->signal();
    if (i % 3 == 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

  for (int i = 0; i < kClients; ++i) {
    clients[i].join();
  }

  for (int i = 0; i < kRep; ++i) {
    EXPECT_EQ(kClients, many_ends[i]->load());
    delete many_ends[i];
  }
}

}  // namespace soc
}  // namespace foedus
