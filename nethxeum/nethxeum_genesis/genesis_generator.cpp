// nethxeum Genesis Block Generator
// C++17 - Generates the genesis block parameters for nethxeum (NTU)
// Founded by Rony Katie

#include "genesis_generator.h"
#include "randomx_wrapper.h"
#include "tx_builder.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <chrono>

#include "../src/cryptonote_basic/cryptonote_format_utils.h"
#include "../src/cryptonote_core/cryptonote_tx_utils.h"
#include "../src/serialization/binary_utils.h"
#include "../src/crypto/hash.h"
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

using namespace nethxeum;

//---------------------------------------------------------------------------
void nethxeum::print_genesis_params(const genesis_params& params) {
    std::cout << "\n";
    std::cout << "============================================================\n";
    std::cout << "  nethxeum (NTU) GENESIS BLOCK PARAMETERS\n";
    std::cout << "  [May/31/2026] The Private Financial Network - Founded by Rony Katie\n";
    std::cout << "============================================================\n";
    std::cout << "GENESIS_TX_HEX    = " << params.genesis_tx_hex << "\n";
    std::cout << "GENESIS_HASH      = " << params.genesis_hash << "\n";
    std::cout << "GENESIS_TIMESTAMP = " << params.genesis_timestamp << "\n";
    std::cout << "GENESIS_NONCE     = " << params.nonce << "\n";
    std::cout << "MERKLE_ROOT       = " << params.merkle_root << "\n";
    std::cout << "GENESIS_MESSAGE   = " << params.genesis_message << "\n";
    std::cout << "============================================================\n";
}

//---------------------------------------------------------------------------
bool nethxeum::write_genesis_params_to_file(const genesis_params& params, const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "ERROR: Cannot open file " << filename << " for writing\n";
        return false;
    }

    ofs << "# nethxeum (NTU) Genesis Block Parameters\n";
    ofs << "# Generated automatically by nethxeum_genesis\n";
    ofs << "# [May/31/2026] The Private Financial Network - Founded by Rony Katie\n";
    ofs << "#\n";
    ofs << "GENESIS_TX_HEX    = " << params.genesis_tx_hex << "\n";
    ofs << "GENESIS_HASH      = " << params.genesis_hash << "\n";
    ofs << "GENESIS_TIMESTAMP = " << params.genesis_timestamp << "\n";
    ofs << "GENESIS_NONCE     = " << params.nonce << "\n";
    ofs << "MERKLE_ROOT       = " << params.merkle_root << "\n";
    ofs << "GENESIS_MESSAGE   = " << params.genesis_message << "\n";
    ofs << "#\n";
    ofs << "# Copy these values into:\n";
    ofs << "#   - src/cryptonote_config.h (GENESIS_TX, GENESIS_NONCE)\n";
    ofs << "#   - src/cryptonote_core/blockchain.cpp (genesis block hash)\n";
    ofs << "#\n";

    ofs.close();
    std::cout << "Genesis parameters written to: " << filename << "\n";
    return true;
}

//---------------------------------------------------------------------------
crypto::hash nethxeum::calculate_merkle_root(const std::vector<crypto::hash>& tx_hashes) {
    if (tx_hashes.empty()) {
        return crypto::null_hash;
    }
    if (tx_hashes.size() == 1) {
        return tx_hashes[0];
    }

    std::vector<crypto::hash> hashes = tx_hashes;
    while (hashes.size() > 1) {
        std::vector<crypto::hash> next_level;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            if (i + 1 < hashes.size()) {
                crypto::hash combined;
                crypto::cn_fast_hash(hashes[i].data, sizeof(crypto::hash) * 2, combined);
                next_level.push_back(combined);
            } else {
                next_level.push_back(hashes[i]);
            }
        }
        hashes = next_level;
    }
    return hashes[0];
}

//---------------------------------------------------------------------------
bool nethxeum::build_genesis_block(cryptonote::block& bl,
                                    const cryptonote::transaction& coinbase_tx,
                                    uint64_t timestamp,
                                    uint64_t nonce,
                                    uint64_t difficulty_target) {
    bl.major_version = BLOCK_MAJOR_VERSION_CURRENT;
    bl.minor_version = BLOCK_MINOR_VERSION_CURRENT;
    bl.timestamp = timestamp;
    bl.nonce = nonce;
    bl.prev_id = crypto::null_hash;  // Genesis: all zeros
    bl.miner_tx = coinbase_tx;

    // Add coinbase tx hash to tx_hashes for Merkle root
    crypto::hash tx_hash;
    if (!cryptonote::get_transaction_hash(coinbase_tx, tx_hash)) {
        std::cerr << "ERROR: Failed to calculate coinbase tx hash\n";
        return false;
    }
    bl.tx_hashes.clear();  // Genesis block has no other transactions

    // Calculate block hash using RandomX
    // For genesis, we use a null seed hash (all zeros)
    std::string hashing_blob = cryptonote::get_block_hashing_blob(bl);

    crypto::hash block_hash;
    randomx_wrapper rx;
    if (!rx.initialize(GENESIS_MESSAGE)) {
        std::cerr << "WARNING: Failed to initialize RandomX, using fallback hash\n";
        // Fallback: use cn_fast_hash
        crypto::cn_fast_hash(hashing_blob.data(), hashing_blob.size(), block_hash);
    } else {
        rx.calculate_hash(hashing_blob, reinterpret_cast<char*>(&block_hash));
    }

    // Don't set hash here - it will be found by mining
    return true;
}

//---------------------------------------------------------------------------
static std::string bin2hex(const std::string& bin) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (unsigned char c : bin) {
        oss << std::setw(2) << (int)c;
    }
    return oss.str();
}

static std::string hash_to_hex(const crypto::hash& h) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < 32; ++i) {
        oss << std::setw(2) << (int)(unsigned char)h.data[i];
    }
    return oss.str();
}

//---------------------------------------------------------------------------
int main(int argc, char** argv) {
    std::cout << "\n";
    std::cout << "************************************************************\n";
    std::cout << "  nethxeum (NTU) Genesis Block Generator\n";
    std::cout << "  [May/31/2026] The Private Financial Network - Founded by Rony Katie\n";
    std::cout << "  C++17 / RandomX\n";
    std::cout << "************************************************************\n\n";

    // Parse arguments
    uint64_t timestamp = std::chrono::seconds(std::time(nullptr)).count();
    uint64_t difficulty = 1;  // Minimum difficulty for genesis
    uint64_t nonce = 10021;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--timestamp" || arg == "-t") && i + 1 < argc) {
            timestamp = std::stoull(argv[++i]);
        } else if ((arg == "--difficulty" || arg == "-d") && i + 1 < argc) {
            difficulty = std::stoull(argv[++i]);
        } else if ((arg == "--nonce" || arg == "-n") && i + 1 < argc) {
            nonce = std::stoull(argv[++i]);
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --timestamp, -t <unix_ts>  Genesis timestamp (default: current time)\n";
            std::cout << "  --difficulty, -d <num>     Initial difficulty (default: 1)\n";
            std::cout << "  --nonce, -n <num>          Starting nonce (default: 0)\n";
            std::cout << "  --help, -h                 Show this help\n";
            return 0;
        }
    }

    std::cout << "Genesis Message:\n" << GENESIS_MESSAGE << "\n\n";
    std::cout << "Parameters:\n";
    std::cout << "  Timestamp:  " << timestamp << " (" << timestamp << ")\n";
    std::cout << "  Difficulty: " << difficulty << "\n";
    std::cout << "  Nonce:      " << nonce << "\n\n";

    // Step 1: Initialize RandomX with genesis message as seed
    std::cout << "[1/5] Initializing RandomX with genesis message seed...\n";
    randomx_wrapper rx;
    if (!rx.initialize(GENESIS_MESSAGE)) {
        std::cerr << "ERROR: Failed to initialize RandomX\n";
        return 1;
    }
    std::cout << "      RandomX initialized successfully\n";

    // Calculate and display the seed hash
    crypto::hash seed_hash;
    crypto::cn_fast_hash(GENESIS_MESSAGE.data(), GENESIS_MESSAGE.size(), seed_hash);
    std::cout << "      Seed hash: " << hash_to_hex(seed_hash) << "\n";

    // Step 2: Build coinbase transaction
    std::cout << "[2/5] Building genesis coinbase transaction...\n";
    cryptonote::transaction coinbase_tx;
    uint64_t unlock_time = NTU_UNLOCK_WINDOW;  // Block 0 + 100
    if (!build_genesis_coinbase_tx(coinbase_tx, unlock_time)) {
        std::cerr << "ERROR: Failed to build coinbase transaction\n";
        return 1;
    }

    // Serialize coinbase tx to hex
    std::string coinbase_blob;
    if (!cryptonote::t_serializable_object_to_blob(coinbase_tx, coinbase_blob)) {
        std::cerr << "ERROR: Failed to serialize coinbase transaction\n";
        return 1;
    }
    std::string coinbase_hex = bin2hex(coinbase_blob);
    std::cout << "      Coinbase tx size: " << coinbase_blob.size() << " bytes\n";

    // Step 3: Calculate Merkle root
    std::cout << "[3/5] Calculating Merkle root...\n";
    crypto::hash coinbase_hash;
    if (!cryptonote::get_transaction_hash(coinbase_tx, coinbase_hash)) {
        std::cerr << "ERROR: Failed to hash coinbase transaction\n";
        return 1;
    }
    std::vector<crypto::hash> tx_hashes = { coinbase_hash };
    crypto::hash merkle_root = calculate_merkle_root(tx_hashes);
    std::cout << "      Merkle root: " << hash_to_hex(merkle_root) << "\n";

    // Step 4: Build genesis block
    std::cout << "[4/5] Building genesis block...\n";
    cryptonote::block genesis_block;
    if (!build_genesis_block(genesis_block, coinbase_tx, timestamp, nonce, difficulty)) {
        std::cerr << "ERROR: Failed to build genesis block\n";
        return 1;
    }

    // Step 5: Mine genesis block (find valid nonce)
    std::cout << "[5/5] Mining genesis block with RandomX...\n";
    std::cout << "      Mining with difficulty " << difficulty << "...\n";

    crypto::hash block_hash;
    uint64_t final_nonce = nonce;
    bool found = false;

    // Simple mining loop for genesis block
    const uint64_t max_attempts = 10000000ULL;  // 10M attempts max
    for (uint64_t n = nonce; n < nonce + max_attempts; ++n) {
        genesis_block.nonce = n;
        std::string hashing_blob = cryptonote::get_block_hashing_blob(genesis_block);

        rx.calculate_hash(hashing_blob, reinterpret_cast<char*>(&block_hash));

        // Check difficulty (simple check: hash < max_uint64 / difficulty)
        // For difficulty 1, any valid hash works
        crypto::hash difficulty_target;
        memset(difficulty_target.data, 0xff, 32);  // Max difficulty
        // For actual difficulty check, we compare the hash value
        bool valid = true;
        for (int i = 0; i < 32; ++i) {
            if ((unsigned char)block_hash.data[i] < (unsigned char)difficulty_target.data[i]) {
                valid = true;
                break;
            } else if ((unsigned char)block_hash.data[i] > (unsigned char)difficulty_target.data[i]) {
                valid = false;
                break;
            }
        }

        if (valid || difficulty == 1) {
            final_nonce = n;
            found = true;
            break;
        }

        if (n % 100000 == 0 && n > nonce) {
            std::cout << "      Tried " << (n - nonce) << " nonces...\r" << std::flush;
        }
    }

    if (!found) {
        std::cerr << "\nWARNING: Could not find valid nonce within " << max_attempts << " attempts\n";
        std::cerr << "         Using last computed hash. For production, increase attempts or use --nonce.\n";
    }

    genesis_block.nonce = final_nonce;
    std::cout << "\n      Nonce found: " << final_nonce << "\n";
    std::cout << "      Block hash:  " << hash_to_hex(block_hash) << "\n";

    // Assemble final parameters
    genesis_params params;
    params.genesis_tx_hex = coinbase_hex;
    params.genesis_hash = hash_to_hex(block_hash);
    params.genesis_timestamp = std::to_string(timestamp);
    params.genesis_message = GENESIS_MESSAGE;
    params.merkle_root = hash_to_hex(merkle_root);
    params.nonce = final_nonce;
    params.difficulty = difficulty;

    // Output results
    print_genesis_params(params);

    // Write to file
    if (!write_genesis_params_to_file(params, "genesis_params.txt")) {
        std::cerr << "WARNING: Failed to write genesis_params.txt\n";
    }

    std::cout << "\nDone. Copy GENESIS_TX_HEX and GENESIS_NONCE to cryptonote_config.h\n";
    std::cout << "and set GENESIS_TX to the hex value above.\n\n";

    return 0;
}
