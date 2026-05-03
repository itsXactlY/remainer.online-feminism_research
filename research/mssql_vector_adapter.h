#pragma once

#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include <functional>
#include <future>
#include <immintrin.h>

#ifdef _WIN32
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#else
#include <sql.h>
#include <sqlext.h>
#endif

namespace mssql_vector {

// Vector data types
enum class VectorType : uint8_t {
    Float32 = 0,
    Float16 = 1,
    Int8 = 2,
    Binary = 3
};

// Similarity metrics
enum class SimilarityMetric : uint8_t {
    Cosine = 0,
    Euclidean = 1,
    DotProduct = 2,
    Manhattan = 3
};

// Vector batch for bulk operations
struct VectorBatch {
    std::vector<uint8_t> data;      // Contiguous vector data
    std::vector<uint64_t> offsets;   // Start offset for each vector
    std::vector<uint32_t> dims;      // Dimensions for each vector
    VectorType type;
    
    size_t size() const { return offsets.size(); }
};

// Search result
struct SearchResult {
    uint64_t vectorId;
    float similarity;
    std::vector<uint8_t> metadata;
};

// Connection configuration
struct ConnectionConfig {
    std::string server;
    std::string database;
    std::string username;
    std::string password;
    std::string driver = "ODBC Driver 18 for SQL Server";
    size_t poolMin = 5;
    size_t poolMax = 20;
    size_t timeout = 30;
    bool enableMARS = true;
    bool enableTLS = true;
    bool trustServerCertificate = false;
};

// SIMD capability flags
struct SIMDCapabilities {
    bool SSE42 = false;
    bool AVX2 = false;
    bool AVX512F = false;
    bool AVX512BW = false;
    bool AVX512VNNI = false;
    
    static SIMDCapabilities Detect();
};

// Connection pool interface
class ConnectionPool {
public:
    ConnectionPool(const ConnectionConfig& config);
    ~ConnectionPool();
    
    SQLHDBC Acquire();
    void Release(SQLHDBC conn);
    
    size_t ActiveCount() const;
    size_t IdleCount() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// SIMD compute engine
class SIMDEngine {
public:
    SIMDEngine();
    
    // Similarity computations
    float CosineSimilarity(const float* a, const float* b, size_t dim);
    float EuclideanDistance(const float* a, const float* b, size_t dim);
    float DotProduct(const float* a, const float* b, size_t dim);
    
    // Batch operations
    void BatchCosineSimilarity(const float* query, const float* vectors,
                              float* scores, size_t numVectors, size_t dim);
    
    // Vector normalization
    void NormalizeVector(float* vector, size_t dim);
    void BatchNormalize(float* vectors, size_t numVectors, size_t dim);
    
    // Capability query
    SIMDCapabilities GetCapabilities() const;
    
private:
    SIMDCapabilities caps;
    
    // SIMD implementations
    float CosineAVX2(const float* a, const float* b, size_t dim);
    float CosineAVX512(const float* a, const float* b, size_t dim);
    float CosineScalar(const float* a, const float* b, size_t dim);
};

// Bulk copy engine
class BulkCopyEngine {
public:
    BulkCopyEngine(SQLHDBC conn);
    ~BulkCopyEngine();
    
    // Initialize bulk copy for table
    bool Init(const std::string& tableName);
    
    // Bind columns
    bool BindColumn(uint16_t col, void* data, size_t length, int sqlType);
    
    // Send rows
    bool SendRow();
    bool SendBatch(const VectorBatch& batch);
    
    // Commit batch
    bool Commit();
    
    // Statistics
    size_t RowsCopied() const;
    double ThroughputMBps() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory-mapped vector file
class MMapVectorStore {
public:
    MMapVectorStore();
    ~MMapVectorStore();
    
    // Open/create vector file
    bool Open(const std::string& path, bool readOnly = true);
    bool Create(const std::string& path, uint64_t maxVectors,
                uint32_t dim, VectorType type);
    void Close();
    
    // Vector access
    const float* GetVector(uint64_t index) const;
    float* GetMutableVector(uint64_t index);
    
    // Batch access
    void ReadVectors(uint64_t start, uint64_t count, float* buffer) const;
    void WriteVectors(uint64_t start, uint64_t count, const float* buffer);
    
    // Metadata
    uint64_t NumVectors() const;
    uint32_t VectorDim() const;
    VectorType DataType() const;
    size_t FileSize() const;
    
    // Memory management
    void Prefetch(uint64_t start, uint64_t count);
    void AdviseSequential();
    void AdviseRandom();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Async query executor
class AsyncExecutor {
public:
    AsyncExecutor(size_t threadPoolSize = 4);
    ~AsyncExecutor();
    
    // Execute query asynchronously
    std::future<std::vector<std::vector<uint8_t>>> ExecuteQuery(
        const std::string& query,
        const std::vector<std::string>& params = {});
    
    // Execute non-query asynchronously
    std::future<bool> ExecuteNonQuery(
        const std::string& query,
        const std::vector<std::string>& params = {});
    
    // Bulk insert asynchronously
    std::future<size_t> BulkInsertAsync(
        const std::string& tableName,
        const VectorBatch& batch);
    
    // Similarity search asynchronously
    std::future<std::vector<SearchResult>> SearchSimilarAsync(
        const float* queryVector,
        uint32_t dim,
        size_t topK,
        SimilarityMetric metric = SimilarityMetric::Cosine);
    
    // Cancel pending operations
    void CancelAll();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Streaming result interface
class VectorStream {
public:
    VectorStream(SQLHSTMT stmt, uint32_t batchSize = 1000);
    ~VectorStream();
    
    // Iteration
    bool Next();
    bool HasNext() const;
    
    // Current row access
    uint64_t CurrentVectorId() const;
    const float* CurrentVector() const;
    uint32_t CurrentDim() const;
    const std::vector<uint8_t>& CurrentMetadata() const;
    
    // Batch access
    size_t CurrentBatchSize() const;
    void GetCurrentBatch(float* buffer, uint64_t* ids, size_t maxRows);
    
    // Control
    void Reset();
    void Close();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main adapter class
class MSSQLVectorAdapter {
public:
    MSSQLVectorAdapter();
    ~MSSQLVectorAdapter();
    
    // Connection management
    bool Connect(const ConnectionConfig& config);
    void Disconnect();
    bool IsConnected() const;
    
    // Schema management
    bool CreateVectorTable(const std::string& tableName,
                          uint32_t dim,
                          VectorType type = VectorType::Float32);
    bool CreateVectorIndex(const std::string& tableName,
                          const std::string& indexName);
    
    // Single vector operations
    uint64_t InsertVector(const std::string& tableName,
                         const float* data,
                         uint32_t dim,
                         const std::vector<uint8_t>& metadata = {});
    bool UpdateVector(uint64_t vectorId,
                     const float* data,
                     uint32_t dim);
    bool DeleteVector(uint64_t vectorId);
    std::vector<float> GetVector(uint64_t vectorId);
    
    // Batch operations
    size_t InsertVectorsBatch(const std::string& tableName,
                             const VectorBatch& batch);
    size_t UpdateVectorsBatch(const VectorBatch& batch);
    size_t DeleteVectorsBatch(const std::vector<uint64_t>& ids);
    
    // Similarity search
    std::vector<SearchResult> SearchSimilar(const float* queryVector,
                                           uint32_t dim,
                                           size_t topK,
                                           SimilarityMetric metric = SimilarityMetric::Cosine);
    std::vector<SearchResult> SearchSimilarBatch(
        const std::vector<std::pair<const float*, uint32_t>>& queries,
        size_t topK,
        SimilarityMetric metric = SimilarityMetric::Cosine);
    
    // Streaming interface
    VectorStream OpenStream(const std::string& query);
    VectorStream StreamVectors(const std::string& tableName,
                              const std::string& whereClause = "");
    
    // Async operations
    AsyncExecutor& GetAsyncExecutor();
    
    // Configuration
    void SetConnectionPool(size_t min, size_t max);
    void EnableSIMD(bool enable);
    void SetCacheSize(size_t maxVectors);
    
    // Statistics
    struct Stats {
        uint64_t totalVectors;
        uint64_t totalInserts;
        uint64_t totalSearches;
        double avgInsertLatencyMs;
        double avgSearchLatencyMs;
        size_t cacheHits;
        size_t cacheMisses;
    };
    Stats GetStats() const;
    void ResetStats();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Utility functions
namespace utils {
    // Vector serialization
    std::vector<uint8_t> SerializeVector(const float* data, uint32_t dim, VectorType type);
    std::vector<float> DeserializeVector(const uint8_t* data, size_t length, uint32_t dim);
    
    // Batch creation
    VectorBatch CreateBatch(const std::vector<const float*>& vectors,
                           uint32_t dim,
                           VectorType type = VectorType::Float32);
    
    // Performance measurement
    class ScopedTimer {
    public:
        ScopedTimer(double& output);
        ~ScopedTimer();
    private:
        double& out;
        std::chrono::high_resolution_clock::time_point start;
    };
    
    // Memory alignment
    void* AlignedAlloc(size_t alignment, size_t size);
    void AlignedFree(void* ptr);
}

} // namespace mssql_vector

