#include "mssql_vector_adapter.h"
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>
#include <algorithm>
#include <numeric>

using namespace mssql_vector;

class Benchmark {
private:
    std::mt19937 rng;
    std::normal_distribution<float> dist;
    
public:
    Benchmark() : rng(std::random_device{}()), dist(0.0f, 1.0f) {}
    
    std::vector<float> GenerateVector(size_t dim) {
        std::vector<float> vec(dim);
        for (auto& v : vec) {
            v = dist(rng);
        }
        return vec;
    }
    
    void RunAllBenchmarks() {
        std::cout << "C++ High-Performance Vector Adapter Benchmark\n";
        std::cout << "=============================================\n\n";
        
        BenchmarkSIMD();
        BenchmarkMemoryMappedIO();
        BenchmarkConnectionPooling();
        BenchmarkBulkOperations();
        BenchmarkStreaming();
    }
    
    void BenchmarkSIMD() {
        std::cout << "1. SIMD Performance Benchmark\n";
        std::cout << "-----------------------------\n";
        
        SIMDEngine engine;
        auto caps = engine.GetCapabilities();
        
        std::cout << "SIMD Capabilities:\n";
        std::cout << "  SSE4.2: " << (caps.SSE42 ? "Yes" : "No") << "\n";
        std::cout << "  AVX2: " << (caps.AVX2 ? "Yes" : "No") << "\n";
        std::cout << "  AVX-512F: " << (caps.AVX512F ? "Yes" : "No") << "\n\n";
        
        std::vector<size_t> dimensions = {128, 256, 512, 768, 1024};
        std::vector<size_t> batchSizes = {1000, 10000, 100000};
        
        for (size_t dim : dimensions) {
            for (size_t batchSize : batchSizes) {
                BenchmarkSimilarity(engine, dim, batchSize);
            }
        }
        std::cout << "\n";
    }
    
    void BenchmarkSimilarity(SIMDEngine& engine, size_t dim, size_t batchSize) {
        auto query = GenerateVector(dim);
        std::vector<float> vectors(batchSize * dim);
        for (size_t i = 0; i < batchSize * dim; i++) {
            vectors[i] = dist(rng);
        }
        
        std::vector<float> scores(batchSize);
        
        auto start = std::chrono::high_resolution_clock::now();
        engine.BatchCosineSimilarity(query.data(), vectors.data(), scores.data(), batchSize, dim);
        auto end = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        double throughput = batchSize / elapsed * 1000;
        
        std::cout << "  Dim: " << std::setw(4) << dim
                  << " | Batch: " << std::setw(6) << batchSize
                  << " | Time: " << std::setw(8) << std::fixed << std::setprecision(2) << elapsed << " ms"
                  << " | Throughput: " << std::setw(10) << std::fixed << std::setprecision(0) << throughput << " comp/s\n";
    }
    
    void BenchmarkMemoryMappedIO() {
        std::cout << "\n2. Memory-Mapped I/O Benchmark\n";
        std::cout << "-----------------------------\n";
        
        const size_t dim = 384;
        const std::vector<size_t> vectorCounts = {1000, 10000, 100000, 1000000};
        
        for (size_t count : vectorCounts) {
            BenchmarkMMapWrite(dim, count);
            BenchmarkMMapRead(dim, count);
        }
        std::cout << "\n";
    }
    
    void BenchmarkMMapWrite(size_t dim, size_t count) {
        const std::string filename = "/tmp/benchmark_vectors_" + std::to_string(count) + ".mmap";
        
        MMapVectorStore store;
        if (!store.Create(filename, count, dim, VectorType::Float32)) {
            std::cerr << "Failed to create mmap file\n";
            return;
        }
        
        auto start = std::chrono::throughput_clock::now();
        for (size_t i = 0; i < count; i++) {
            auto vec = GenerateVector(dim);
            store.WriteVectors(i, 1, vec.data());
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        double throughput = count / elapsed * 1000;
        
        std::cout << "  Write " << std::setw(7) << count << " vectors"
                  << " | Time: " << std::setw(8) << std::fixed << std::setprecision(2) << elapsed << " ms"
                  << " | Throughput: " << std::setw(10) << std::fixed << std::setprecision(0) << throughput << " vec/s\n";
        
        store.Close();
    }
    
    void BenchmarkMMapRead(size_t dim, size_t count) {
        const std::string filename = "/tmp/benchmark_vectors_" + std::to_string(count) + ".mmap";
        
        MMapVectorStore store;
        if (!store.Open(filename, true)) {
            std::cerr << "Failed to open mmap file\n";
            return;
        }
        
        std::vector<float> buffer(dim);
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < count; i++) {
            store.ReadVectors(i, 1, buffer.data());
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        double throughput = count / elapsed * 1000;
        
        std::cout << "  Read  " << std::setw(7) << count << " vectors"
                  << " | Time: " << std::setw(8) << std::fixed << std::setprecision(2) << elapsed << " ms"
                  << " | Throughput: " << std::setw(10) << std::fixed << std::setprecision(0) << throughput << " vec/s\n";
        
        store.Close();
    }
    
    void BenchmarkConnectionPooling() {
        std::cout << "\n3. Connection Pooling Benchmark\n";
        std::cout << "-------------------------------\n";
        
        // Simulated connection pool benchmark
        const std::vector<size_t> poolSizes = {1, 5, 10, 20, 50};
        const size_t operations = 1000;
        
        for (size_t poolSize : poolSizes) {
            BenchmarkPoolOperations(poolSize, operations);
        }
        std::cout << "\n";
    }
    
    void BenchmarkPoolOperations(size_t poolSize, size_t operations) {
        // Simulate connection acquisition/release
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<std::thread> threads;
        std::atomic<size_t> completed{0};
        
        for (size_t i = 0; i < poolSize; i++) {
            threads.emplace_back([&completed, operations, poolSize]() {
                size_t opsPerThread = operations / poolSize;
                for (size_t j = 0; j < opsPerThread; j++) {
                    // Simulate work
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    completed++;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        double opsPerSec = completed / elapsed * 1000;
        
        std::cout << "  Pool Size: " << std::setw(3) << poolSize
                  << " | Completed: " << std::setw(6) << completed
                  << " | Time: " << std::setw(8) << std::fixed << std::setprecision(2) << elapsed << " ms"
                  << " | Ops/s: " << std::setw(10) << std::fixed << std::setprecision(0) << opsPerSec << "\n";
    }
    
    void BenchmarkBulkOperations() {
        std::cout << "\n4. Bulk Operations Benchmark\n";
        std::cout << "----------------------------\n";
        
        const size_t dim = 256;
        const std::vector<size_t> batchSizes = {100, 1000, 10000, 50000};
        
        for (size_t batchSize : batchSizes) {
            BenchmarkBulkInsert(dim, batchSize);
        }
        std::cout << "\n";
    }
    
    void BenchmarkBulkInsert(size_t dim, size_t batchSize) {
        // Create batch
        VectorBatch batch;
        batch.type = VectorType::Float32;
        batch.offsets.reserve(batchSize);
        batch.dims.reserve(batchSize);
        
        size_t totalSize = 0;
        for (size_t i = 0; i < batchSize; i++) {
            auto vec = GenerateVector(dim);
            size_t vecSize = vec.size() * sizeof(float);
            
            batch.offsets.push_back(totalSize);
            batch.dims.push_back(dim);
            
            size_t oldSize = batch.data.size();
            batch.data.resize(oldSize + vecSize);
            std::memcpy(batch.data.data() + oldSize, vec.data(), vecSize);
            
            totalSize += vecSize;
        }
        
        // Simulate bulk insert
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate network I/O
        std::this_thread::sleep_for(std::chrono::microseconds(batchSize * 2));
        
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        double throughput = batchSize / elapsed * 1000;
        double dataRate = (totalSize / (1024.0 * 1024.0)) / (elapsed / 1000.0);
        
        std::cout << "  Batch Size: " << std::setw(6) << batchSize
                  << " | Time: " << std::setw(8) << std::fixed << std::setprecision(2) << elapsed << " ms"
                  << " | Throughput: " << std::setw(10) << std::fixed << std::setprecision(0) << throughput << " vec/s"
                  << " | Data Rate: " << std::setw(8) << std::fixed << std::setprecision(1) << dataRate << " MB/s\n";
    }
    
    void BenchmarkStreaming() {
        std::cout << "\n5. Streaming Performance Benchmark\n";
        std::cout << "---------------------------------\n";
        
        const size_t dim = 512;
        const std::vector<size_t> streamSizes = {1000, 10000, 100000};
        const std::vector<size_t> batchSizes = {100, 500, 1000};
        
        for (size_t streamSize : streamSizes) {
            for (size_t batchSize : batchSizes) {
                BenchmarkStreamProcessing(dim, streamSize, batchSize);
            }
        }
        std::cout << "\n";
    }
    
    void BenchmarkStreamProcessing(size_t dim, size_t streamSize, size_t batchSize) {
        // Simulate streaming with different batch sizes
        size_t numBatches = streamSize / batchSize;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t batch = 0; batch < numBatches; batch++) {
            // Simulate fetching batch
            std::vector<float> buffer(batchSize * dim);
            for (size_t i = 0; i < batchSize * dim; i++) {
                buffer[i] = dist(rng);
            }
            
            // Simulate processing
            std::this_thread::sleep_for(std::chrono::microseconds(batchSize));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        double throughput = streamSize / elapsed * 1000;
        
        std::cout << "  Stream: " << std::setw(6) << streamSize
                  << " | Batch: " << std::setw(4) << batchSize
                  << " | Time: " << std::setw(8) << std::fixed << std::setprecision(2) << elapsed << " ms"
                  << " | Throughput: " << std::setw(10) << std::fixed << std::setprecision(0) << throughput << " vec/s\n";
    }
};

int main() {
    Benchmark benchmark;
    benchmark.RunAllBenchmarks();
    return 0;
}

