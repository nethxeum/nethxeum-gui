// nethxeum RandomX Wrapper Implementation

#include "randomx_wrapper.h"
#include <iostream>
#include <cstring>

// Include RandomX from nethxeum source tree
#include "../external/randomx/src/randomx.h"

namespace nethxeum {

//---------------------------------------------------------------------------
randomx_wrapper::randomx_wrapper()
    : initialized_(false)
    , cache_(nullptr)
    , dataset_(nullptr)
    , vm_(nullptr)
{
}

//---------------------------------------------------------------------------
randomx_wrapper::~randomx_wrapper() {
    if (vm_) {
        randomx_destroy_vm(vm_);
        vm_ = nullptr;
    }
    if (dataset_) {
        randomx_release_dataset(dataset_);
        dataset_ = nullptr;
    }
    if (cache_) {
        randomx_release_cache(cache_);
        cache_ = nullptr;
    }
}

//---------------------------------------------------------------------------
bool randomx_wrapper::initialize(const std::string& seed_key) {
    if (initialized_) {
        return true;  // Already initialized
    }

    // Get optimal RandomX flags
    randomx_flags flags = randomx_get_flags();

    // Allocate cache
    cache_ = randomx_alloc_cache(flags);
    if (!cache_) {
        std::cerr << "ERROR: Failed to allocate RandomX cache\n";
        return false;
    }

    // Initialize cache with seed key
    randomx_init_cache(cache_, seed_key.data(), seed_key.size());

    // For genesis block generation, we use light mode (cache only, no dataset)
    // This is sufficient for generating a few hashes and doesn't require 2080MB
    // If full dataset is needed, initialize it here

    // Allocate VM with cache only (light mode)
    vm_ = randomx_create_vm(flags, cache_, nullptr);
    if (!vm_) {
        // Try without additional flags
        vm_ = randomx_create_vm(RANDOMX_FLAG_DEFAULT, cache_, nullptr);
        if (!vm_) {
            std::cerr << "ERROR: Failed to create RandomX VM\n";
            randomx_release_cache(cache_);
            cache_ = nullptr;
            return false;
        }
    }

    initialized_ = true;
    return true;
}

//---------------------------------------------------------------------------
void randomx_wrapper::calculate_hash(const std::string& input_data, char* output_hash) {
    calculate_hash(input_data.data(), input_data.size(), output_hash);
}

//---------------------------------------------------------------------------
void randomx_wrapper::calculate_hash(const void* data, size_t length, char* output_hash) {
    if (!initialized_ || !vm_) {
        std::cerr << "ERROR: RandomX wrapper not initialized\n";
        memset(output_hash, 0, 32);
        return;
    }
    randomx_calculate_hash(vm_, data, length, output_hash);
}

} // namespace nethxeum
