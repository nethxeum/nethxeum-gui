# nethxeum Genesis Block Generator CMakeLists.txt
# C++17 required
# Links against nethxeum's internal RandomX and cryptonote libraries

enable_language(ASM)
cmake_minimum_required(VERSION 3.10)
project(nethxeum_genesis LANGUAGES C CXX)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Paths relative to nethxeum root
set(nethxeum_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(nethxeum_SRC "${nethxeum_ROOT}/src")
set(nethxeum_EXT "${nethxeum_ROOT}/external")

# Boost is required (same as nethxeum)
find_package(Boost REQUIRED COMPONENTS system filesystem thread date_time chrono regex serialization program_options)


# Compiler flags (similar to nethxeum)
add_definitions(-DWIN32_LEAN_AND_MEAN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ftemplate-depth=900")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")

# ---------------------------------------------------------------------------
# RandomX library (from nethxeum source tree)
# ---------------------------------------------------------------------------
set(RANDOMX_DIR "C:/nethxeum/nethxeum/external/randomx/src")

set(RANDOMX_SOURCES
    ${RANDOMX_DIR}/dataset.cpp
    ${RANDOMX_DIR}/vm_interpreted.cpp
    ${RANDOMX_DIR}/vm_interpreted_light.cpp
    ${RANDOMX_DIR}/randomx.cpp
    ${RANDOMX_DIR}/soft_aes.cpp
    ${RANDOMX_DIR}/vm_compiled.cpp
    ${RANDOMX_DIR}/vm_compiled_light.cpp
    ${RANDOMX_DIR}/aes_hash.cpp
    ${RANDOMX_DIR}/argon2_ref.c
    ${RANDOMX_DIR}/argon2_ssse3.c
    ${RANDOMX_DIR}/argon2_avx2.c
    ${RANDOMX_DIR}/jit_compiler_x86_static.S
    ${RANDOMX_DIR}/jit_compiler_x86.cpp
    ${RANDOMX_DIR}/bytecode_machine.cpp
    ${RANDOMX_DIR}/instructions_portable.cpp
    ${RANDOMX_DIR}/superscalar.cpp
    ${RANDOMX_DIR}/blake2/blake2b.c
    ${RANDOMX_DIR}/reciprocal.c
    ${RANDOMX_DIR}/virtual_machine.cpp
    ${RANDOMX_DIR}/allocator.cpp
    ${RANDOMX_DIR}/cpu.cpp
    ${RANDOMX_DIR}/virtual_memory.c
    ${RANDOMX_DIR}/argon2_core.c
    ${RANDOMX_DIR}/blake2_generator.cpp
    ${RANDOMX_DIR}/assembly_generator_x86.cpp
    ${RANDOMX_DIR}/instruction.cpp
)

add_library(randomx STATIC ${RANDOMX_SOURCES})
target_include_directories(randomx PUBLIC ${RANDOMX_DIR})

# Platform-specific flags for RandomX
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    target_compile_options(randomx PRIVATE -maes -mssse3)
endif()

# ---------------------------------------------------------------------------
# Crypto helper library
# ---------------------------------------------------------------------------
set(CRYPTO_HELPER_SOURCES
    ${nethxeum_SRC}/crypto/tree-hash.c
    ${nethxeum_SRC}/crypto/hash.c
    ${nethxeum_SRC}/crypto/slow-hash.c
    ${nethxeum_SRC}/crypto/oaes_lib.c
    ${nethxeum_SRC}/crypto/crypto-ops-data.c
    ${nethxeum_SRC}/crypto/crypto-ops.c
    ${nethxeum_SRC}/crypto/keccak.c
    ${nethxeum_SRC}/crypto/chacha.c
    ${nethxeum_SRC}/crypto/random.c
    ${nethxeum_SRC}/crypto/aesb.c
    ${nethxeum_SRC}/crypto/rx-slow-hash.c
    ${nethxeum_SRC}/cryptonote_basic/cryptonote_format_utils.cpp
    ${nethxeum_SRC}/cryptonote_basic/cryptonote_basic_impl.cpp
    ${nethxeum_SRC}/cryptonote_basic/account.cpp
    ${nethxeum_SRC}/cryptonote_basic/miner.cpp
    ${nethxeum_SRC}/ringct/rctSigs.cpp
    ${nethxeum_SRC}/ringct/rctOps.cpp
    ${nethxeum_SRC}/ringct/rctTypes.cpp
    ${nethxeum_SRC}/ringct/bulletproofs.cc
    ${nethxeum_SRC}/ringct/bulletproofs_plus.cc
    ${nethxeum_SRC}/common/base58.cpp
    ${nethxeum_SRC}/common/threadpool.cpp
    ${nethxeum_SRC}/common/aligned.c
    ${nethxeum_SRC}/common/util.cpp
    ${nethxeum_SRC}/common/i18n.cpp
    ${nethxeum_SRC}/crypto/crypto.cpp
    ${nethxeum_SRC}/cryptonote_core/cryptonote_tx_utils.cpp
    ${nethxeum_ROOT}/contrib/epee/src/memwipe.c
    ${nethxeum_ROOT}/contrib/epee/src/mlocker.cpp
    ${nethxeum_ROOT}/contrib/epee/src/wipeable_string.cpp
    ${nethxeum_SRC}/crypto/blake256.c
    ${nethxeum_SRC}/crypto/groestl.c
    ${nethxeum_SRC}/crypto/jh.c
    ${nethxeum_SRC}/crypto/skein.c
    ${nethxeum_SRC}/crypto/hash-extra-blake.c
    ${nethxeum_SRC}/crypto/hash-extra-groestl.c
    ${nethxeum_SRC}/crypto/hash-extra-jh.c
    ${nethxeum_SRC}/crypto/hash-extra-skein.c
    ${nethxeum_SRC}/common/dns_utils.cpp
    ${nethxeum_ROOT}/contrib/epee/src/mlog.cpp
    ${nethxeum_SRC}/ringct/rctCryptoOps.c
    ${nethxeum_SRC}/crypto/CryptonightR_JIT.c
    ${nethxeum_SRC}/crypto/CryptonightR_template.S
    ${nethxeum_EXT}/easylogging++/easylogging++.cc
    ${nethxeum_ROOT}/contrib/epee/src/hex.cpp
    ${nethxeum_ROOT}/contrib/epee/src/int-util.cpp
    ${nethxeum_ROOT}/contrib/epee/src/string_tools.cpp
    ${nethxeum_ROOT}/contrib/epee/src/portable_storage.cpp
    ${nethxeum_ROOT}/contrib/epee/src/misc_language.cpp
    ${nethxeum_ROOT}/contrib/epee/src/file_io_utils.cpp
    ${nethxeum_ROOT}/contrib/epee/src/net_utils_base.cpp
    ${nethxeum_ROOT}/contrib/epee/src/net_parse_helpers.cpp
    ${nethxeum_ROOT}/contrib/epee/src/http_base.cpp
    ${nethxeum_ROOT}/contrib/epee/src/abstract_http_client.cpp
    ${nethxeum_ROOT}/contrib/epee/src/parserse_base_utils.cpp
    ${nethxeum_SRC}/device/device.cpp
    ${nethxeum_SRC}/device/device_default.cpp
    ${nethxeum_SRC}/device/log.cpp
    ${nethxeum_SRC}/crypto/crypto_ops_builder/verify.c
    ${nethxeum_SRC}/cryptonote_basic/cryptonote_format_utils_basic.cpp
)

add_library(crypto_helpers STATIC ${CRYPTO_HELPER_SOURCES})
target_include_directories(crypto_helpers PRIVATE
    ${nethxeum_SRC}/crypto/includes
    ${nethxeum_SRC}/crypto/wallet
    ${nethxeum_SRC}/crypto
    C:/nethxeum/build/nethxeum/translations
    C:/nethxeum/build_genesis
    ${nethxeum_SRC}
    ${nethxeum_SRC}/cryptonote_basic
    ${nethxeum_SRC}/cryptonote_core
    ${nethxeum_SRC}/ringct
    ${nethxeum_SRC}/common
    ${nethxeum_ROOT}/contrib/epee/include
    ${nethxeum_EXT}/easylogging++
    ${nethxeum_EXT}/db_drivers/liblmdb
    ${Boost_INCLUDE_DIRS}
)
target_compile_definitions(crypto_helpers PRIVATE ELPP_NO_DEFAULT_LOG_FILE ELPP_FEATURE_CRASH_LOG)
target_compile_options(crypto_helpers PRIVATE -maes -mssse3)
target_link_libraries(crypto_helpers PUBLIC randomx ${Boost_LIBRARIES} ssl crypto unbound ws2_32)

# ---------------------------------------------------------------------------
# nethxeum Genesis Generator executable
# ---------------------------------------------------------------------------
set(GENESIS_SOURCES
    genesis_generator.cpp
    genesis_generator.h
    randomx_wrapper.cpp
    randomx_wrapper.h
    tx_builder.cpp
    tx_builder.h
)

add_executable(nethxeum_genesis ${GENESIS_SOURCES})
target_include_directories(nethxeum_genesis PRIVATE
    ${nethxeum_SRC}
    ${nethxeum_SRC}/cryptonote_basic
    ${nethxeum_SRC}/cryptonote_core
    ${nethxeum_SRC}/ringct
    ${nethxeum_SRC}/common
    ${nethxeum_SRC}/crypto
    ${nethxeum_ROOT}/contrib/epee/include
    ${nethxeum_EXT}/easylogging++
    C:/nethxeum/build/nethxeum/translations
    C:/nethxeum/build_genesis
    ${nethxeum_EXT}/randomx/src
)
target_link_libraries(nethxeum_genesis PRIVATE randomx crypto_helpers ${Boost_LIBRARIES} ssl crypto unbound ws2_32 bcrypt)

# ---------------------------------------------------------------------------
# Install target
# ---------------------------------------------------------------------------
install(TARGETS nethxeum_genesis DESTINATION bin)

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  nethxeum Genesis Generator")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Boost: ${Boost_VERSION}")
message(STATUS "========================================")
message(STATUS "")
