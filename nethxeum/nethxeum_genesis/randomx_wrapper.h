// nethxeum RandomX Wrapper
// Provides a clean C++ interface to RandomX hashing for genesis block generation

#pragma once

#include <string>
#include <cstddef>
#include <cstdint>

// Forward declaration to avoid including randomx.h in header
struct randomx_cache;
struct randomx_dataset;
struct randomx_vm;

namespace nethxeum {

class randomx_wrapper {
public:
    randomx_wrapper();
    ~randomx_wrapper();

    // Initialize RandomX with the given seed/key
    // This creates the cache and VM needed for hashing
    bool initialize(const std::string& seed_key);

    // Calculate RandomX hash of data
    // Output must be at least 32 bytes
    void calculate_hash(const std::string& input_data, char* output_hash);

    // Calculate hash with raw data pointer
    void calculate_hash(const void* data, size_t length, char* output_hash);

    // Check if wrapper is properly initialized
    bool is_initialized() const { return initialized_; }

private:
    bool initialized_;
    randomx_cache* cache_;
    randomx_dataset* dataset_;
    randomx_vm* vm_;

    // Disable copy
    randomx_wrapper(const randomx_wrapper&) = delete;
    randomx_wrapper& operator=(const randomx_wrapper&) = delete;
};

} // namespace nethxeum
