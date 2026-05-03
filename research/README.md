# C++ High-Performance Vector Adapter for SQL Server

## Research Summary

This repository contains research and design specifications for a C++ adapter capable of handling millions of vector operations per second with Microsoft SQL Server backend, targeting sub-millisecond latency for copy-trading platform contexts.

## Key Research Findings

### 1. ODBC vs OLE DB vs Native Client Performance

**Recommendation**: Use ODBC with Microsoft ODBC Driver 18 for SQL Server.

- **ODBC**: Best balance of performance, stability, and cross-platform support
- **OLE DB**: Deprecated, Windows-only, historical performance advantage no longer relevant
- **Native Client**: Deprecated by Microsoft in favor of ODBC

### 2. Bulk Insert/Copy Patterns

- **BCP API**: Optimal for high-volume inserts (10K-50K rows per batch)
- **Table-Valued Parameters (TVP)**: Good for complex data types
- **Memory-Mapped I/O**: Essential for large vector datasets

### 3. Streaming vs Batch Processing

- **Streaming**: For datasets > 10K vectors, use forward-only cursors
- **Batch**: For < 10K vectors, load entire dataset
- **Hybrid**: Use streaming with prefetch for optimal memory usage

### 4. Connection Pooling

- Implement custom connection pool with configurable min/max connections
- Enable ODBC connection pooling for additional optimization
- Use connection health checks and automatic recovery

### 5. Binary Vector Storage

- Use VARBINARY(8000) for vectors up to 8KB
- Enable page compression for storage optimization
- Consider FILESTREAM for vectors > 1MB
- Use columnstore indexes for analytical queries

### 6. Memory-Mapped I/O Patterns

- Essential for large vector datasets
- Use aligned memory allocation for SIMD operations
- Implement prefetching for sequential access patterns

### 7. SIMD-Accelerated Similarity Computation

- AVX2 for 8-wide float operations
- AVX-512 for 16-wide operations (where available)
- Runtime capability detection with fallbacks
- OpenMP for batch parallelization

### 8. Async Query Patterns

- Use ODBC async mode for non-blocking operations
- Implement future-based async patterns
- Thread pool for concurrent operations
- Cancellation support for long-running queries

## Performance Targets

| Operation | Target Latency | Target Throughput |
|-----------|----------------|-------------------|
| Single vector insert | < 0.5ms | 2,000 ops/sec |
| Batch insert (10K) | < 100ms | 100K vectors/sec |
| Similarity search (1M) | < 10ms | 100 searches/sec |
| Streaming 1M vectors | < 1 second | 1M vectors/sec |
| SIMD similarity | N/A | 10M comp/sec |

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    C++ Vector Adapter                        │
├─────────────────────────────────────────────────────────────┤
│  API Layer                                                  │
│  ├─ VectorStore API                                        │
│  ├─ Similarity Search API                                  │
│  └─ Batch Operations API                                   │
├─────────────────────────────────────────────────────────────┤
│  Core Engine                                                │
│  ├─ Connection Pool Manager                                │
│  ├─ Query Executor                                         │
│  ├─ SIMD Compute Engine                                    │
│  └─ Cache Manager                                          │
├─────────────────────────────────────────────────────────────┤
│  I/O Layer                                                  │
│  ├─ ODBC Driver Interface                                  │
│  ├─ Memory-Mapped File Handler                             │
│  ├─ Async I/O Scheduler                                    │
│  └─ Bulk Copy Engine                                       │
└─────────────────────────────────────────────────────────────┘
```

## Implementation Status

### Phase 1: Core Infrastructure ✓
- [x] ODBC connection wrapper with pooling
- [x] Basic vector storage schema
- [x] Simple insert/retrieve operations

### Phase 2: Performance Optimization ✓
- [x] Bulk copy implementation
- [x] SIMD acceleration (AVX2/AVX-512)
- [x] Memory-mapped I/O

### Phase 3: Advanced Features ✓
- [x] Async query patterns
- [x] Streaming interface
- [x] Similarity search optimization

### Phase 4: Testing & Tuning ✓
- [x] Performance benchmarking
- [x] Stress testing framework
- [x] Production tuning guidelines

## Files in this Repository

### Documentation
- `cpp_mssql_vector_adapter_spec.md` - Complete design specification
- `README.md` - This file

### Implementation
- `mssql_vector_adapter.h` - Main adapter header file
- `example_usage.cpp` - Usage examples
- `benchmark.cpp` - Performance benchmarking
- `CMakeLists.txt` - Build configuration

### Key Classes

#### MSSQLVectorAdapter
Main adapter class providing:
- Connection management
- Vector CRUD operations
- Similarity search
- Streaming interface
- Async operations

#### SIMDEngine
SIMD-accelerated computation:
- Cosine similarity
- Euclidean distance
- Dot product
- Batch operations

#### ConnectionPool
Connection management:
- Configurable pool size
- Health checking
- Automatic recovery
- Thread-safe operations

#### BulkCopyEngine
High-performance bulk operations:
- BCP API integration
- Batch commit optimization
- Throughput monitoring

#### MMapVectorStore
Memory-mapped file operations:
- Efficient vector storage
- Prefetch optimization
- Aligned memory access

## Building the Project

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Usage Example

```cpp
#include "mssql_vector_adapter.h"

using namespace mssql_vector;

int main() {
    MSSQLVectorAdapter adapter;
    
    ConnectionConfig config;
    config.server = "localhost";
    config.database = "VectorDB";
    config.username = "user";
    config.password = "pass";
    
    if (adapter.Connect(config)) {
        // Insert vector
        std::vector<float> vec(768);
        uint64_t id = adapter.InsertVector("Embeddings", vec.data(), 768);
        
        // Similarity search
        auto results = adapter.SearchSimilar(vec.data(), 768, 10);
        
        adapter.Disconnect();
    }
    
    return 0;
}
```

## Performance Benchmarks

Expected performance on modern hardware:

### SIMD Performance
- **AVX2**: 5-8x speedup over scalar code
- **AVX-512**: 10-16x speedup over scalar code
- **Batch 100K vectors**: < 10ms for 768-dim cosine similarity

### I/O Performance
- **Memory-mapped read**: 5-10M vectors/second
- **Bulk insert**: 500K-1M vectors/second
- **Streaming**: 1-2M vectors/second

### Connection Pooling
- **Acquire/release**: < 10 microseconds
- **1000 concurrent ops**: < 100ms total

## Dependencies

- **ODBC**: unixODBC (Linux) / Microsoft ODBC Driver 18
- **C++17**: Required for modern features
- **CMake**: 3.15+
- **SIMD**: Compiler support for AVX2/AVX-512

## Production Considerations

### Security
- Use TLS 1.2+ for connections
- Implement parameterized queries
- Regular credential rotation
- Audit logging

### Monitoring
- Connection pool utilization
- Query execution times
- SIMD instruction usage
- Memory usage patterns

### Scaling
- Horizontal: Multiple adapter instances
- Vertical: Optimize SIMD and I/O
- Caching: Result caching layer
- Partitioning: Table partitioning strategy

## Future Work

### Short Term
1. GPU acceleration for similarity computation
2. Distributed caching layer
3. Query result caching

### Long Term
1. Approximate nearest neighbor (ANN) integration
2. Vector quantization for storage optimization
3. Multi-region replication support
4. Columnstore index integration

## License

This research and code is provided as-is for evaluation purposes.

## Contact

For questions or contributions, please refer to the design specification document.

