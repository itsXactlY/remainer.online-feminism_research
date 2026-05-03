#include "mssql_vector_adapter.h"
#include <iostream>
#include <chrono>
#include <random>

using namespace mssql_vector;

// Generate random vectors for testing
std::vector<float> GenerateRandomVector(uint32_t dim) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<float> dist(0.0f, 1.0f);
    
    std::vector<float> vec(dim);
    for (auto& v : vec) {
        v = dist(gen);
    }
    return vec;
}

// Example 1: Basic connection and vector operations
void Example1_BasicOperations() {
    std::cout << "=== Example 1: Basic Operations ===\n";
    
    MSSQLVectorAdapter adapter;
    
    // Configure connection
    ConnectionConfig config;
    config.server = "localhost";
    config.database = "VectorDB";
    config.username = "vector_user";
    config.password = "secure_password";
    config.poolMin = 5;
    config.poolMax = 20;
    
    // Connect to database
    if (!adapter.Connect(config)) {
        std::cerr << "Failed to connect to database\n";
        return;
    }
    
    // Create vector table
    const uint32_t dim = 768;  // BERT embedding dimension
    if (!adapter.CreateVectorTable("Embeddings", dim, VectorType::Float32)) {
        std::cerr << "Failed to create vector table\n";
        return;
    }
    
    // Insert single vector
    auto vec = GenerateRandomVector(dim);
    uint64_t id = adapter.InsertVector("Embeddings", vec.data(), dim);
    std::cout << "Inserted vector with ID: " << id << "\n";
    
    // Retrieve vector
    auto retrieved = adapter.GetVector(id);
    if (!retrieved.empty()) {
        std::cout << "Retrieved vector with " << retrieved.size() << " dimensions\n";
    }
    
    adapter.Disconnect();
}

// Example 2: Batch operations with SIMD acceleration
void Example2_BatchOperations() {
    std::cout << "\n=== Example 2: Batch Operations ===\n";
    
    MSSQLVectorAdapter adapter;
    ConnectionConfig config;
    config.server = "localhost";
    config.database = "VectorDB";
    config.username = "vector_user";
    config.password = "secure_password";
    
    if (!adapter.Connect(config)) {
        std::cerr << "Failed to connect\n";
        return;
    }
    
    const uint32_t dim = 512;
    const size_t numVectors = 10000;
    
    // Create batch of vectors
    VectorBatch batch;
    batch.type = VectorType::Float32;
    batch.offsets.reserve(numVectors);
    batch.dims.reserve(numVectors);
    
    size_t totalSize = 0;
    for (size_t i = 0; i < numVectors; i++) {
        auto vec = GenerateRandomVector(dim);
        size_t vecSize = vec.size() * sizeof(float);
        
        batch.offsets.push_back(totalSize);
        batch.dims.push_back(dim);
        
        size_t oldSize = batch.data.size();
        batch.data.resize(oldSize + vecSize);
        std::memcpy(batch.data.data() + oldSize, vec.data(), vecSize);
        
        totalSize += vecSize;
    }
    
    // Bulk insert
    auto start = std::chrono::high_resolution_clock::now();
    size_t inserted = adapter.InsertVectorsBatch("Embeddings", batch);
    auto end = std::chrono::high_resolution_clock::now();
    
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Inserted " << inserted << " vectors in " << elapsed << " ms\n";
    std::cout << "Throughput: " << (inserted / elapsed * 1000) << " vectors/second\n";
    
    adapter.Disconnect();
}

// Example 3: Similarity search with SIMD
void Example3_SimilaritySearch() {
    std::cout << "\n=== Example 3: Similarity Search ===\n";
    
    MSSQLVectorAdapter adapter;
    ConnectionConfig config;
    config.server = "localhost";
    config.database = "VectorDB";
    config.username = "vector_user";
    config.password = "secure_password";
    
    if (!adapter.Connect(config)) {
        std::cerr << "Failed to connect\n";
        return;
    }
    
    const uint32_t dim = 256;
    
    // Generate query vector
    auto queryVec = GenerateRandomVector(dim);
    
    // Search for top 10 similar vectors
    auto start = std::chrono::high_resolution_clock::now();
    auto results = adapter.SearchSimilar(queryVec.data(), dim, 10, SimilarityMetric::Cosine);
    auto end = std::chrono::high_resolution_clock::now();
    
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Found " << results.size() << " similar vectors in " << elapsed << " ms\n";
    
    for (size_t i = 0; i < std::min(results.size(), size_t(5)); i++) {
        std::cout << "  " << (i+1) << ". ID: " << results[i].vectorId
                  << " Similarity: " << results[i].similarity << "\n";
    }
    
    adapter.Disconnect();
}

// Example 4: Async operations
void Example4_AsyncOperations() {
    std::cout << "\n=== Example 4: Async Operations ===\n";
    
    MSSQLVectorAdapter adapter;
    ConnectionConfig config;
    config.server = "localhost";
    config.database = "VectorDB";
    config.username = "vector_user";
    config.password = "secure_password";
    
    if (!adapter.Connect(config)) {
        std::cerr << "Failed to connect\n";
        return;
    }
    
    const uint32_t dim = 128;
    auto queryVec = GenerateRandomVector(dim);
    
    // Async similarity search
    auto future1 = adapter.GetAsyncExecutor().SearchSimilarAsync(
        queryVec.data(), dim, 5, SimilarityMetric::Cosine);
    
    // Do other work while search is running
    std::cout << "Search running asynchronously...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Get results
    auto results = future1.get();
    std::cout << "Async search completed with " << results.size() << " results\n";
    
    adapter.Disconnect();
}

// Example 5: Streaming large datasets
void Example5_StreamingResults() {
    std::cout << "\n=== Example 5: Streaming Results ===\n";
    
    MSSQLVectorAdapter adapter;
    ConnectionConfig config;
    config.server = "localhost";
    config.database = "VectorDB";
    config.username = "vector_user";
    config.password = "secure_password";
    
    if (!adapter.Connect(config)) {
        std::cerr << "Failed to connect\n";
        return;
    }
    
    // Stream all vectors from table
    auto stream = adapter.StreamVectors("Embeddings");
    
    size_t count = 0;
    auto start = std::chrono::high_resolution_clock::now();
    
    while (stream.Next()) {
        count++;
        // Process vector data
        // const float* vec = stream.CurrentVector();
        // uint32_t dim = stream.CurrentDim();
        
        if (count % 10000 == 0) {
            std::cout << "Processed " << count << " vectors...\n";
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::cout << "Streamed " << count << " vectors in " << elapsed << " ms\n";
    std::cout << "Throughput: " << (count / elapsed * 1000) << " vectors/second\n";
    
    stream.Close();
    adapter.Disconnect();
}

// Example 6: Memory-mapped file operations
void Example6_MemoryMappedIO() {
    std::cout << "\n=== Example 6: Memory-Mapped I/O ===\n";
    
    const uint32_t dim = 384;
    const uint64_t numVectors = 100000;
    const std::string filename = "/tmp/vectors.mmap";
    
    // Create memory-mapped file
    MMapVectorStore store;
    if (!store.Create(filename, numVectors, dim, VectorType::Float32)) {
        std::cerr << "Failed to create memory-mapped file\n";
        return;
    }
    
    // Write vectors
    auto start = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < numVectors; i++) {
        auto vec = GenerateRandomVector(dim);
        store.WriteVectors(i, 1, vec.data());
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Wrote " << numVectors << " vectors to mmap in " << elapsed << " ms\n";
    
    // Read vectors
    start = std::chrono::high_resolution_clock::now();
    std::vector<float> buffer(dim);
    for (uint64_t i = 0; i < numVectors; i++) {
        store.ReadVectors(i, 1, buffer.data());
    }
    end = std::chrono::high_resolution_clock::now();
    
    elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Read " << numVectors << " vectors from mmap in " << elapsed << " ms\n";
    
    store.Close();
}

// Example 7: SIMD-accelerated similarity computation
void Example7_SIMDCompute() {
    std::cout << "\n=== Example 7: SIMD Compute ===\n";
    
    SIMDEngine engine;
    auto caps = engine.GetCapabilities();
    
    std::cout << "SIMD Capabilities:\n";
    std::cout << "  SSE4.2: " << (caps.SSE42 ? "Yes" : "No") << "\n";
    std::cout << "  AVX2: " << (caps.AVX2 ? "Yes" : "No") << "\n";
    std::cout << "  AVX-512F: " << (caps.AVX512F ? "Yes" : "No") << "\n";
    
    const size_t dim = 1024;
    const size_t numVectors = 100000;
    
    // Generate vectors
    std::vector<float> query = GenerateRandomVector(dim);
    std::vector<float> vectors(numVectors * dim);
    for (size_t i = 0; i < numVectors * dim; i++) {
        vectors[i] = static_cast<float>(rand()) / RAND_MAX;
    }
    
    // Batch similarity computation
    std::vector<float> scores(numVectors);
    auto start = std::chrono::high_resolution_clock::now();
    engine.BatchCosineSimilarity(query.data(), vectors.data(), scores.data(), numVectors, dim);
    auto end = std::chrono::high_resolution_clock::now();
    
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Computed " << numVectors << " similarities in " << elapsed << " ms\n";
    std::cout << "Throughput: " << (numVectors / elapsed * 1000) << " comparisons/second\n";
    
    // Find top 5
    std::vector<size_t> indices(numVectors);
    std::iota(indices.begin(), indices.end(), 0);
    std::partial_sort(indices.begin(), indices.begin() + 5, indices.end(),
                     [&scores](size_t a, size_t b) { return scores[a] > scores[b]; });
    
    std::cout << "Top 5 similarities:\n";
    for (size_t i = 0; i < 5; i++) {
        std::cout << "  " << (i+1) << ". Index: " << indices[i]
                  << " Score: " << scores[indices[i]] << "\n";
    }
}

int main() {
    std::cout << "C++ High-Performance Vector Adapter for SQL Server\n";
    std::cout << "=================================================\n\n";
    
    try {
        Example1_BasicOperations();
        Example2_BatchOperations();
        Example3_SimilaritySearch();
        Example4_AsyncOperations();
        Example5_StreamingResults();
        Example6_MemoryMappedIO();
        Example7_SIMDCompute();
        
        std::cout << "\nAll examples completed successfully!\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

