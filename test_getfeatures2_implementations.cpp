#include "simplified_getfeatures2_implementations.h"
#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <memory>
#include <iostream>

using namespace GetFeatures2Implementations;

// Test fixture class
class GetFeatures2Test : public ::testing::Test {
protected:
    void SetUp() override {
        mockDevice = std::make_unique<PhysicalDevice>();
        
        // Initialize test structures
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features2.pNext = &storage16BitFeatures;
        
        storage16BitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;
        storage16BitFeatures.pNext = &storage8BitFeatures;
        
        storage8BitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES;
        storage8BitFeatures.pNext = nullptr;
        
        // Initialize all feature structures to false
        ResetFeatureStructures();
    }
    
    void ResetFeatureStructures() {
        storage16BitFeatures.storageBuffer16BitAccess = VK_FALSE;
        storage16BitFeatures.uniformAndStorageBuffer16BitAccess = VK_FALSE;
        storage16BitFeatures.storagePushConstant16 = VK_FALSE;
        storage16BitFeatures.storageInputOutput16 = VK_FALSE;
        
        storage8BitFeatures.storageBuffer8BitAccess = VK_FALSE;
        storage8BitFeatures.uniformAndStorageBuffer8BitAccess = VK_FALSE;
        storage8BitFeatures.storagePushConstant8 = VK_FALSE;
    }
    
    std::unique_ptr<PhysicalDevice> mockDevice;
    VkPhysicalDeviceFeatures2 features2;
    VkPhysicalDevice16BitStorageFeatures storage16BitFeatures;
    VkPhysicalDevice8BitStorageFeatures storage8BitFeatures;
};

// Correctness Tests
TEST_F(GetFeatures2Test, CallbackApproach_CorrectStructureTraversal) {
    CallbackBasedGetFeatures2 impl(mockDevice.get());
    
    size_t result = impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), false);
    
    // Should traverse all 3 structures
    EXPECT_GT(result, 0);
}

TEST_F(GetFeatures2Test, CallbackApproach_CorrectFeatureUpdate) {
    CallbackBasedGetFeatures2 impl(mockDevice.get());
    
    // Test with updateFeatures = true
    size_t result = impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), true);
    
    // Verify that features were updated (mock implementation sets them to VK_TRUE)
    EXPECT_EQ(storage16BitFeatures.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(storage16BitFeatures.uniformAndStorageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(storage8BitFeatures.storageBuffer8BitAccess, VK_TRUE);
    EXPECT_GT(result, 0);
}

TEST_F(GetFeatures2Test, CallbackApproach_NoUpdateWhenDisabled) {
    CallbackBasedGetFeatures2 impl(mockDevice.get());
    
    // Test with updateFeatures = false
    impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), false);
    
    // Features should remain unchanged (VK_FALSE)
    EXPECT_EQ(storage16BitFeatures.storageBuffer16BitAccess, VK_FALSE);
    EXPECT_EQ(storage8BitFeatures.storageBuffer8BitAccess, VK_FALSE);
}

TEST_F(GetFeatures2Test, TemplateApproach_CorrectStructureTraversal) {
    TemplateBasedGetFeatures2 impl(mockDevice.get());
    
    size_t result = impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), false);
    
    EXPECT_GT(result, 0);
}

TEST_F(GetFeatures2Test, TemplateApproach_CorrectFeatureUpdate) {
    TemplateBasedGetFeatures2 impl(mockDevice.get());
    
    impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), true);
    
    EXPECT_EQ(storage16BitFeatures.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(storage8BitFeatures.storageBuffer8BitAccess, VK_TRUE);
}

TEST_F(GetFeatures2Test, LookupTableApproach_CorrectStructureTraversal) {
    LookupTableBasedGetFeatures2 impl(mockDevice.get());
    
    size_t result = impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), false);
    
    EXPECT_GT(result, 0);
}

TEST_F(GetFeatures2Test, LookupTableApproach_CorrectFeatureUpdate) {
    LookupTableBasedGetFeatures2 impl(mockDevice.get());
    
    impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), true);
    
    EXPECT_EQ(storage16BitFeatures.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(storage8BitFeatures.storageBuffer8BitAccess, VK_TRUE);
}

TEST_F(GetFeatures2Test, RegistryApproach_CorrectStructureTraversal) {
    RegistryBasedGetFeatures2 impl(mockDevice.get());
    
    size_t result = impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), false);
    
    EXPECT_GT(result, 0);
}

// Consistency Tests - All implementations should produce identical results
TEST_F(GetFeatures2Test, AllImplementations_ProduceSameResults) {
    CallbackBasedGetFeatures2 callbackImpl(mockDevice.get());
    TemplateBasedGetFeatures2 templateImpl(mockDevice.get());
    LookupTableBasedGetFeatures2 lookupImpl(mockDevice.get());
    RegistryBasedGetFeatures2 registryImpl(mockDevice.get());
    
    // Test without feature updates
    ResetFeatureStructures();
    VkPhysicalDeviceFeatures2 features2_1 = features2;
    VkPhysicalDevice16BitStorageFeatures storage16_1 = storage16BitFeatures;
    VkPhysicalDevice8BitStorageFeatures storage8_1 = storage8BitFeatures;
    features2_1.pNext = &storage16_1;
    storage16_1.pNext = &storage8_1;
    storage8_1.pNext = nullptr;
    
    size_t result1 = callbackImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2_1), false);
    
    ResetFeatureStructures();
    VkPhysicalDeviceFeatures2 features2_2 = features2;
    VkPhysicalDevice16BitStorageFeatures storage16_2 = storage16BitFeatures;
    VkPhysicalDevice8BitStorageFeatures storage8_2 = storage8BitFeatures;
    features2_2.pNext = &storage16_2;
    storage16_2.pNext = &storage8_2;
    storage8_2.pNext = nullptr;
    
    size_t result2 = templateImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2_2), false);
    
    ResetFeatureStructures();
    VkPhysicalDeviceFeatures2 features2_3 = features2;
    VkPhysicalDevice16BitStorageFeatures storage16_3 = storage16BitFeatures;
    VkPhysicalDevice8BitStorageFeatures storage8_3 = storage8BitFeatures;
    features2_3.pNext = &storage16_3;
    storage16_3.pNext = &storage8_3;
    storage8_3.pNext = nullptr;
    
    size_t result3 = lookupImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2_3), false);
    
    EXPECT_EQ(result1, result2);
    EXPECT_EQ(result2, result3);
}

// Edge Case Tests
TEST_F(GetFeatures2Test, AllImplementations_HandleNullChain) {
    CallbackBasedGetFeatures2 callbackImpl(mockDevice.get());
    TemplateBasedGetFeatures2 templateImpl(mockDevice.get());
    LookupTableBasedGetFeatures2 lookupImpl(mockDevice.get());
    
    // Test with single structure (no chain)
    VkPhysicalDeviceFeatures2 singleFeature;
    singleFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    singleFeature.pNext = nullptr;
    
    size_t result1 = callbackImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&singleFeature), false);
    size_t result2 = templateImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&singleFeature), false);
    size_t result3 = lookupImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&singleFeature), false);
    
    EXPECT_EQ(result1, result2);
    EXPECT_EQ(result2, result3);
    EXPECT_EQ(result1, sizeof(VkPhysicalDeviceFeatures2));
}

TEST_F(GetFeatures2Test, AllImplementations_HandleUnknownStructure) {
    CallbackBasedGetFeatures2 callbackImpl(mockDevice.get());
    TemplateBasedGetFeatures2 templateImpl(mockDevice.get());
    LookupTableBasedGetFeatures2 lookupImpl(mockDevice.get());
    
    // Create a structure with unknown sType
    VkStructHeaderNonConst unknownStruct;
    unknownStruct.sType = static_cast<VkStructureType>(9999); // Unknown type
    unknownStruct.pNext = nullptr;
    
    // All implementations should handle unknown structures gracefully
    size_t result1 = callbackImpl.GetFeatures2(&unknownStruct, false);
    size_t result2 = templateImpl.GetFeatures2(&unknownStruct, false);
    size_t result3 = lookupImpl.GetFeatures2(&unknownStruct, false);
    
    EXPECT_EQ(result1, 0); // Should skip unknown structures
    EXPECT_EQ(result2, 0);
    EXPECT_EQ(result3, 0);
}

// Performance Test Framework
class PerformanceTest : public GetFeatures2Test {
protected:
    static constexpr int ITERATIONS = 100000;
    
    struct PerformanceResult {
        std::string implementationName;
        double averageTimeMs;
        double totalTimeMs;
        size_t iterations;
    };
    
    PerformanceResult MeasurePerformance(const std::string& name, std::function<void()> testFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < ITERATIONS; ++i) {
            testFunc();
            ResetFeatureStructures(); // Reset for consistent state
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        double totalTimeMs = duration.count() / 1000.0;
        double averageTimeMs = totalTimeMs / ITERATIONS;
        
        return {name, averageTimeMs, totalTimeMs, ITERATIONS};
    }
};

TEST_F(PerformanceTest, CompareAllImplementations_PerformanceBenchmark) {
    CallbackBasedGetFeatures2 callbackImpl(mockDevice.get());
    TemplateBasedGetFeatures2 templateImpl(mockDevice.get());
    LookupTableBasedGetFeatures2 lookupImpl(mockDevice.get());
    RegistryBasedGetFeatures2 registryImpl(mockDevice.get());
    
    std::vector<PerformanceResult> results;
    
    // Test Callback Implementation
    results.push_back(MeasurePerformance("Callback", [&]() {
        callbackImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), true);
    }));
    
    // Test Template Implementation
    results.push_back(MeasurePerformance("Template", [&]() {
        templateImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), true);
    }));
    
    // Test Lookup Table Implementation
    results.push_back(MeasurePerformance("LookupTable", [&]() {
        lookupImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), true);
    }));
    
    // Test Registry Implementation
    results.push_back(MeasurePerformance("Registry", [&]() {
        registryImpl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&features2), true);
    }));
    
    // Print results
    std::cout << "\\n=== Performance Benchmark Results ===\\n";
    std::cout << "Iterations per test: " << ITERATIONS << "\\n";
    std::cout << "Structure chain length: 3\\n\\n";
    
    // Sort results by performance (fastest first)
    std::sort(results.begin(), results.end(), 
              [](const PerformanceResult& a, const PerformanceResult& b) {
                  return a.averageTimeMs < b.averageTimeMs;
              });
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        std::cout << (i + 1) << ". " << result.implementationName 
                  << ": " << result.averageTimeMs << " ms/call"
                  << " (Total: " << result.totalTimeMs << " ms)\\n";
    }
    
    // Calculate performance differences
    if (results.size() > 1) {
        std::cout << "\\n=== Performance Comparison ===\\n";
        const double fastestTime = results[0].averageTimeMs;
        
        for (size_t i = 1; i < results.size(); ++i) {
            double slowdownFactor = results[i].averageTimeMs / fastestTime;
            double percentSlower = (slowdownFactor - 1.0) * 100.0;
            
            std::cout << results[i].implementationName 
                      << " is " << slowdownFactor << "x slower ("
                      << percentSlower << "% slower) than " 
                      << results[0].implementationName << "\\n";
        }
    }
    
    // All tests should pass - this is a performance comparison, not a pass/fail test
    SUCCEED();
}

TEST_F(PerformanceTest, MemoryUsage_Comparison) {
    // Test memory footprint of different implementations
    std::cout << "\\n=== Memory Usage Analysis ===\\n";
    
    std::cout << "CallbackBasedGetFeatures2: " << sizeof(CallbackBasedGetFeatures2) << " bytes\\n";
    std::cout << "TemplateBasedGetFeatures2: " << sizeof(TemplateBasedGetFeatures2) << " bytes\\n";
    std::cout << "LookupTableBasedGetFeatures2: " << sizeof(LookupTableBasedGetFeatures2) << " bytes\\n";
    std::cout << "RegistryBasedGetFeatures2: " << sizeof(RegistryBasedGetFeatures2) << " bytes\\n";
    
    // Create instances to measure actual memory usage
    CallbackBasedGetFeatures2 callbackImpl(mockDevice.get());
    TemplateBasedGetFeatures2 templateImpl(mockDevice.get());
    LookupTableBasedGetFeatures2 lookupImpl(mockDevice.get());
    RegistryBasedGetFeatures2 registryImpl(mockDevice.get());
    
    std::cout << "\\nNote: Actual memory usage may be higher due to dynamic allocations\\n";
    std::cout << "(hash maps, function objects, etc.)\\n";
    
    SUCCEED();
}

// Scalability Tests
TEST_F(PerformanceTest, Scalability_LongChains) {
    // Create a longer chain to test scalability
    std::vector<VkPhysicalDevice16BitStorageFeatures> longChain(10);
    
    for (size_t i = 0; i < longChain.size(); ++i) {
        longChain[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;
        longChain[i].pNext = (i < longChain.size() - 1) ? &longChain[i + 1] : nullptr;
    }
    
    CallbackBasedGetFeatures2 callbackImpl(mockDevice.get());
    TemplateBasedGetFeatures2 templateImpl(mockDevice.get());
    LookupTableBasedGetFeatures2 lookupImpl(mockDevice.get());
    
    auto measureLongChain = [&](const std::string& name, auto& impl) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 10000; ++i) {
            impl.GetFeatures2(reinterpret_cast<VkStructHeaderNonConst*>(&longChain[0]), false);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0; // Convert to milliseconds
    };
    
    double callbackTime = measureLongChain("Callback", callbackImpl);
    double templateTime = measureLongChain("Template", templateImpl);
    double lookupTime = measureLongChain("Lookup", lookupImpl);
    
    std::cout << "\\n=== Long Chain Performance (10 structures, 10k iterations) ===\\n";
    std::cout << "Callback: " << callbackTime << " ms\\n";
    std::cout << "Template: " << templateTime << " ms\\n";
    std::cout << "Lookup: " << lookupTime << " ms\\n";
    
    SUCCEED();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}