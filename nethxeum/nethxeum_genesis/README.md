# nethxeum Genesis Block Generator

## Overview

This tool generates the **genesis block parameters** for nethxeum (NTU), a nethxeum fork with Bitcoin-like halving emission, RandomX from block 1, and a fixed supply of 42 million NTU.

**Network**: nethxeum (NTU)  
**Founded by**: Rony Katie  
**Algorithm**: RandomX (active from block 0/1)  
**Block Time**: 5 minutes (300 seconds)  
**Initial Reward**: 50 NTU  
**Halving Interval**: 420,000 blocks (~4 years)

## Genesis Message

```
nethxeum - [May/31/2026] The Private Financial Network - Founded by Rony Katie
In a world of surveillance, privacy is not a crime, it is a right.
Block 0 - [auto-inserted compilation date/time]
```

## File Structure

```
nethxeum_genesis/
|-- CMakeLists.txt          # CMake build configuration (C++17)
|-- genesis_generator.cpp   # Main program entry point
|-- genesis_generator.h     # Genesis parameters and block building
|-- randomx_wrapper.cpp     # RandomX hashing wrapper (light mode)
|-- randomx_wrapper.h       # RandomX wrapper interface
|-- tx_builder.cpp          # Coinbase transaction construction
|-- tx_builder.h            # Transaction builder interface
|-- README.md               # This file
```

## Dependencies

- **CMake** 3.10+
- **C++17** compiler (gcc 7+, clang 5+, MSVC 2017+)
- **Boost** (system, filesystem, thread, chrono, regex, serialization)
- **nethxeum source tree** (for RandomX, cryptonote_basic, crypto libraries)

No external dependencies beyond what nethxeum already requires.

## Compilation

From the **nethxeum root directory**:

```bash
# Create build directory
mkdir nethxeum_genesis/build
cd nethxeum_genesis/build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)
```

## Usage

```bash
# Generate with default parameters (current timestamp, difficulty 1)
./nethxeum_genesis

# With custom parameters
./nethxeum_genesis --timestamp 1700000000 --difficulty 1 --nonce 0

# Show help
./nethxeum_genesis --help
```

### Options

| Option | Short | Description | Default |
|--------|-------|-------------|---------|
| `--timestamp` | `-t` | Unix timestamp for genesis block | Current time |
| `--difficulty` | `-d` | Initial mining difficulty | 1 |
| `--nonce` | `-n` | Starting nonce for mining | 0 |
| `--help` | `-h` | Show help message | - |

## Output

The generator produces:

1. **Console output** with all genesis parameters
2. **`genesis_params.txt`** file with copy-paste ready values

### Generated Parameters

| Parameter | Description | Destination |
|-----------|-------------|-------------|
| `GENESIS_TX_HEX` | Serialized coinbase transaction hex | `src/cryptonote_config.h` (`GENESIS_TX`) |
| `GENESIS_HASH` | Hash of the mined genesis block | `src/cryptonote_config.h` (comment) |
| `GENESIS_TIMESTAMP` | Unix timestamp of genesis | Hardcoded in config |
| `GENESIS_NONCE` | Nonce that produces valid hash | `src/cryptonote_config.h` (`GENESIS_NONCE`) |
| `MERKLE_ROOT` | Merkle root of coinbase tx | Blockchain validation |

### Integration Steps

1. Run the generator to get `GENESIS_TX_HEX` and `GENESIS_NONCE`
2. Edit `src/cryptonote_config.h`:
   ```cpp
   std::string const GENESIS_TX = "<GENESIS_TX_HEX>";
   uint32_t const GENESIS_NONCE = <GENESIS_NONCE>;
   ```
3. Do the same for testnet and stagenet configs
4. Rebuild nethxeum/nethxeum daemon

## Technical Details

### RandomX Initialization

The generator initializes RandomX with the genesis message as the seed key. This ensures the genesis block hash is uniquely tied to the network identity.

### Coinbase Transaction

- **Height**: 0 (genesis)
- **Reward**: 50 NTU (5,000,000,000 picKatie)
- **Unlock height**: 100 (`CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW`)
- **Extra field**: Contains the genesis message and network marker
- **No RingCT**: Genesis uses transaction version 1

### Mining

The generator performs a simple CPU mining loop to find a nonce that produces a valid block hash under the target difficulty. For difficulty 1, this is typically instantaneous.

## Notes

- The genesis block is the foundation of the blockchain. **All nodes must agree on these parameters.**
- Changing the genesis parameters after launch requires a hard fork or network restart.
- The genesis message includes the compilation timestamp, making each build unique.
- For a production network, consider using a memorable timestamp (e.g., a significant date).
