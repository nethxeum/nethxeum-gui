// Copyright (c) 2014-2022, The monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include "gtest/gtest.h"

#include "cryptonote_basic/cryptonote_basic_impl.h"

using namespace cryptonote;

namespace
{
  //--------------------------------------------------------------------------------------------------------------------
  class block_reward_and_already_generated_coins : public ::testing::Test
  {
  protected:
    static const size_t current_block_weight = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 / 2;

    bool m_block_not_too_big;
    uint64_t m_block_reward;
  };

  #define TEST_ALREADY_GENERATED_COINS(already_generated_coins, expected_reward)                              \
    m_block_not_too_big = get_block_reward(0, current_block_weight, already_generated_coins, m_block_reward,1); \
    ASSERT_TRUE(m_block_not_too_big);                                                                         \
    ASSERT_EQ(m_block_reward, expected_reward);

  // nethxeum: Bitcoin-like halving tests
  // Block 0: 50 NTU = 5,000,000,000 picKatie
  // No tail emission - supply strictly capped at 42,000,000 NTU
  TEST_F(block_reward_and_already_generated_coins, handles_first_values)
  {
    // Genesis block: 50 NTU initial reward (with height=0, fallback uses already_generated_coins)
    // Since already_generated_coins=0 and height=0 (fallback), we need to check what the
    // fallback logic gives us. With 0 coins generated, it finds period_reward = INITIAL_REWARD
    TEST_ALREADY_GENERATED_COINS(0, UINT64_C(5000000000));  // 50 NTU in picKatie
  }

  TEST_F(block_reward_and_already_generated_coins, supply_cap_no_tail_emission)
  {
    // When supply is exhausted, reward should be 0 (no tail emission in nethxeum)
    TEST_ALREADY_GENERATED_COINS(MONEY_SUPPLY, 0);
    TEST_ALREADY_GENERATED_COINS(MONEY_SUPPLY - 1, 1);  // Last coin
  }

  TEST_F(block_reward_and_already_generated_coins, halving_at_correct_intervals)
  {
    // Test that height-based halving works correctly
    // These tests use explicit height parameter via a helper
    uint64_t reward;
    bool ok;

    // Block 0: 50 NTU (full initial reward)
    ok = get_block_reward(0, current_block_weight, 0, reward, 1, 0);
    ASSERT_TRUE(ok);
    ASSERT_EQ(reward, UINT64_C(5000000000));  // 50 NTU

    // Block 1: still 50 NTU (before first halving at 420,000)
    ok = get_block_reward(0, current_block_weight, 5000000000ULL, reward, 1, 1);
    ASSERT_TRUE(ok);
    ASSERT_EQ(reward, UINT64_C(5000000000));  // 50 NTU

    // Block 419,999: still 50 NTU
    ok = get_block_reward(0, current_block_weight, 419999ULL * 5000000000ULL, reward, 1, 419999);
    ASSERT_TRUE(ok);
    ASSERT_EQ(reward, UINT64_C(5000000000));  // 50 NTU

    // Block 420,000: first halving -> 25 NTU
    ok = get_block_reward(0, current_block_weight, 420000ULL * 5000000000ULL, reward, 1, 420000);
    ASSERT_TRUE(ok);
    ASSERT_EQ(reward, UINT64_C(2500000000));  // 25 NTU

    // Block 840,000: second halving -> 12.5 NTU
    ok = get_block_reward(0, current_block_weight,
                          (420000ULL * 5000000000ULL) + (420000ULL * 2500000000ULL),
                          reward, 1, 840000);
    ASSERT_TRUE(ok);
    ASSERT_EQ(reward, UINT64_C(1250000000));  // 12.5 NTU

    // After 64 halvings: reward = 0
    ok = get_block_reward(0, current_block_weight, MONEY_SUPPLY - 1, reward, 1, 64 * 420000ULL);
    ASSERT_TRUE(ok);
    ASSERT_EQ(reward, 0);
  }

  //--------------------------------------------------------------------------------------------------------------------
  class block_reward_and_current_block_weight : public ::testing::Test
  {
  protected:
    virtual void SetUp()
    {
      m_block_not_too_big = get_block_reward(0, 0, already_generated_coins, m_standard_block_reward, 1);
      ASSERT_TRUE(m_block_not_too_big);
      ASSERT_LT(CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1, m_standard_block_reward);
    }

    void do_test(size_t median_block_weight, size_t current_block_weight)
    {
      m_block_not_too_big = get_block_reward(median_block_weight, current_block_weight, already_generated_coins, m_block_reward, 1);
    }

    static const uint64_t already_generated_coins = 0;

    bool m_block_not_too_big;
    uint64_t m_block_reward;
    uint64_t m_standard_block_reward;
  };

  TEST_F(block_reward_and_current_block_weight, handles_block_weight_less_relevance_level)
  {
    do_test(0, CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 - 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_current_block_weight, handles_block_weight_eq_relevance_level)
  {
    do_test(0, CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_current_block_weight, handles_block_weight_gt_relevance_level)
  {
    do_test(0, CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 + 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_current_block_weight, handles_block_weight_less_2_relevance_level)
  {
    do_test(0, 2 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 - 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
    ASSERT_LT(0, m_block_reward);
  }

  TEST_F(block_reward_and_current_block_weight, handles_block_weight_eq_2_relevance_level)
  {
    do_test(0, 2 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(0, m_block_reward);
  }

  TEST_F(block_reward_and_current_block_weight, handles_block_weight_gt_2_relevance_level)
  {
    do_test(0, 2 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 + 1);
    ASSERT_FALSE(m_block_not_too_big);
  }

  //--------------------------------------------------------------------------------------------------------------------
  class block_reward_and_last_block_weights : public ::testing::Test
  {
  protected:
    virtual void SetUp()
    {
      m_last_block_weights.push_back(3  * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1);
      m_last_block_weights.push_back(5  * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1);
      m_last_block_weights.push_back(7  * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1);
      m_last_block_weights.push_back(11 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1);
      m_last_block_weights.push_back(13 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1);

      m_last_block_weights_median = 7 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1;

      m_block_not_too_big = get_block_reward(epee::misc_utils::median(m_last_block_weights), 0, already_generated_coins, m_standard_block_reward, 1);
      ASSERT_TRUE(m_block_not_too_big);
      ASSERT_LT(CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1, m_standard_block_reward);
    }

    void do_test(size_t current_block_weight)
    {
      m_block_not_too_big = get_block_reward(epee::misc_utils::median(m_last_block_weights), current_block_weight, already_generated_coins, m_block_reward, 1);
    }

    static const uint64_t already_generated_coins = 0;

    std::vector<size_t> m_last_block_weights;
    uint64_t m_last_block_weights_median;
    bool m_block_not_too_big;
    uint64_t m_block_reward;
    uint64_t m_standard_block_reward;
  };

  TEST_F(block_reward_and_last_block_weights, handles_block_weight_less_median)
  {
    do_test(m_last_block_weights_median - 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_last_block_weights, handles_block_weight_eq_median)
  {
    do_test(m_last_block_weights_median);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_last_block_weights, handles_block_weight_gt_median)
  {
    do_test(m_last_block_weights_median + 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_last_block_weights, handles_block_weight_less_2_medians)
  {
    do_test(2 * m_last_block_weights_median - 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
    ASSERT_LT(0, m_block_reward);
  }

  TEST_F(block_reward_and_last_block_weights, handles_block_weight_eq_2_medians)
  {
    do_test(2 * m_last_block_weights_median);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(0, m_block_reward);
  }

  TEST_F(block_reward_and_last_block_weights, handles_block_weight_gt_2_medians)
  {
    do_test(2 * m_last_block_weights_median + 1);
    ASSERT_FALSE(m_block_not_too_big);
  }

  TEST_F(block_reward_and_last_block_weights, calculates_correctly)
  {
    ASSERT_EQ(0, m_last_block_weights_median % 8);

    do_test(m_last_block_weights_median * 9 / 8);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward * 63 / 64);

    // 3/2 = 12/8
    do_test(m_last_block_weights_median * 3 / 2);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward * 3 / 4);

    do_test(m_last_block_weights_median * 15 / 8);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward * 15 / 64);
  }
}
