// nethxeum Transaction Builder Implementation
// Constructs the genesis coinbase transaction

#include "tx_builder.h"
#include "genesis_generator.h"
#include "randomx_wrapper.h"

#include <cstring>
#include <iostream>

#include "../src/cryptonote_basic/cryptonote_basic_impl.h"
#include "../src/cryptonote_basic/cryptonote_format_utils.h"
#include "../src/crypto/crypto.h"
#include "../src/crypto/hash.h"

namespace nethxeum {

//---------------------------------------------------------------------------
bool build_genesis_coinbase_tx(cryptonote::transaction& tx, uint64_t unlock_time) {
    tx.vin.clear();
    tx.vout.clear();
    tx.extra.clear();
    tx.version = 1;  // Genesis uses v1 (no RingCT needed)

    // Create the generation input (no real input for coinbase)
    cryptonote::txin_gen in;
    in.height = 0;  // Genesis block height
    tx.vin.push_back(in);

    // Generate ephemeral keys for the output
    // For genesis, we generate deterministic keys from the genesis message
    crypto::hash genesis_msg_hash;
    crypto::cn_fast_hash(GENESIS_MESSAGE.data(), GENESIS_MESSAGE.size(), genesis_msg_hash);

    // Create a deterministic private key from the genesis message hash
    crypto::secret_key tx_key;
    memcpy(&tx_key, genesis_msg_hash.data, sizeof(crypto::secret_key));
    sc_reduce32((unsigned char*)&tx_key);

    // Derive public key
    crypto::public_key tx_pub_key;
    if (!crypto::secret_key_to_public_key(tx_key, tx_pub_key)) {
        std::cerr << "ERROR: Failed to derive tx public key from genesis hash\n";
        return false;
    }

    // Add tx public key to extra
    cryptonote::add_tx_pub_key_to_extra(tx, tx_pub_key);

    // Add genesis message to extra (as nonce/data)
    // Format: [tag_0x04] [size_2bytes] [message_data]
    tx.extra.push_back(TX_EXTRA_NONCE);  // 0x04 tag for nonce/extra
    uint16_t msg_size = static_cast<uint16_t>(GENESIS_MESSAGE.size());
    tx.extra.push_back(msg_size & 0xff);
    tx.extra.push_back((msg_size >> 8) & 0xff);
    for (char c : GENESIS_MESSAGE) {
        tx.extra.push_back(static_cast<unsigned char>(c));
    }

    // Add payment ID tag to identify as genesis
    tx.extra.push_back(TX_EXTRA_NONCE);
    tx.extra.push_back(1);  // 1 byte
    tx.extra.push_back(0x00);  // Genesis marker

    // Create the output with initial reward
    // 50 NTU = 5,000,000,000 picKatie
    uint64_t reward_amount = NTU_INITIAL_REWARD;

    // For genesis, we use a standard output
    // Derive output ephemeral public key
    crypto::key_derivation derivation;
    if (!crypto::generate_key_derivation(tx_pub_key, tx_key, derivation)) {
        std::cerr << "ERROR: Failed to generate key derivation\n";
        return false;
    }

    crypto::public_key out_eph_public_key;
    if (!crypto::derive_public_key(derivation, 0, tx_pub_key, out_eph_public_key)) {
        std::cerr << "ERROR: Failed to derive output public key\n";
        return false;
    }

    // Create output
    cryptonote::tx_out out;
    out.amount = reward_amount;
    out.target = cryptonote::txout_to_key(out_eph_public_key);
    tx.vout.push_back(out);

    // Set unlock time
    tx.unlock_time = unlock_time;

    // Validate the transaction structure
    if (tx.vin.size() != 1) {
        std::cerr << "ERROR: Coinbase must have exactly 1 input\n";
        return false;
    }
    if (tx.vout.size() != 1) {
        std::cerr << "ERROR: Genesis coinbase must have exactly 1 output\n";
        return false;
    }
    if (tx.vout[0].amount != NTU_INITIAL_REWARD) {
        std::cerr << "ERROR: Genesis reward must be exactly " << NTU_INITIAL_REWARD << " picKatie\n";
        return false;
    }

    tx.invalidate_hashes();

    std::cout << "      Genesis coinbase tx built:\n";
    std::cout << "        Reward: " << tx.vout[0].amount << " picKatie (50 NTU)\n";
    std::cout << "        Unlock: block " << unlock_time << "\n";
    std::cout << "        Inputs: " << tx.vin.size() << ", Outputs: " << tx.vout.size() << "\n";

    return true;
}

} // namespace nethxeum
