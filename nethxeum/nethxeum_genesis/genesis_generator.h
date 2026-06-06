// nethxeum Genesis Block Generator
// Founded by Rony Katie

#pragma once
#include <ctime>
#include <string>

#include <cstdint>
#include <string>
#include <vector>
#include "../src/cryptonote_basic/cryptonote_basic.h"
#include "../src/crypto/hash.h"

namespace nethxeum {

// Genesis message with compilation timestamp
inline std::string make_genesis_message() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%b %d %Y %H:%M:%S", std::localtime(&t));
    return std::string("nethxeum - [May/31/2026] The Private Financial Network - Founded by Rony Katie\n")
         + "In a world of surveillance, privacy is not a crime, it is a right.\n"
         + "Block 0 - " + buf;
}
static const std::string GENESIS_MESSAGE = make_genesis_message();

// nethxeum constants (must match cryptonote_config.h)
static const uint64_t NTU_MAX_SUPPLY = 4200000000000000ULL;  // 42M NTU in picKatie
static const uint64_t NTU_INITIAL_REWARD = 5000000000ULL;     // 50 NTU in picKatie
static const uint64_t NTU_HALVING_INTERVAL = 420000ULL;
static const uint64_t NTU_COIN = 100000000ULL;               // 1 NTU = 10^8 picKatie
static const uint64_t NTU_UNLOCK_WINDOW = 100;               // Bitcoin-like

// Output structure for genesis parameters
struct genesis_params {
    std::string genesis_tx_hex;
    std::string genesis_hash;
    std::string genesis_timestamp;
    std::string genesis_message;
    std::string merkle_root;
    uint64_t    nonce;
    uint64_t    difficulty;
};

// Print genesis parameters to console
void print_genesis_params(const genesis_params& params);

// Write genesis parameters to file
bool write_genesis_params_to_file(const genesis_params& params, const std::string& filename);

// Build coinbase transaction for genesis block
bool build_genesis_coinbase_tx(cryptonote::transaction& tx, uint64_t unlock_time);

// Calculate Merkle root from transaction hashes
crypto::hash calculate_merkle_root(const std::vector<crypto::hash>& tx_hashes);

// Build the genesis block
bool build_genesis_block(cryptonote::block& bl,
                         const cryptonote::transaction& coinbase_tx,
                         uint64_t timestamp,
                         uint64_t nonce,
                         uint64_t difficulty_target);

} // namespace nethxeum
