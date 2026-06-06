// nethxeum Transaction Builder
// Constructs the genesis coinbase transaction

#pragma once

#include <cstdint>
#include "../src/cryptonote_basic/cryptonote_basic.h"

namespace nethxeum {

// Build a coinbase transaction for the genesis block
//  - No inputs (genesis coinbase)
//  - Single output with the initial block reward
//  - Extra field contains the genesis message
//
// Parameters:
//   tx           - output transaction
//   unlock_time  - block height at which outputs become spendable
//
// Returns true on success
bool build_genesis_coinbase_tx(cryptonote::transaction& tx, uint64_t unlock_time);

} // namespace nethxeum
