# 🧪 Blender Testing Documentation

## 📋 Overview

This document provides comprehensive guidance on testing in Blender, covering unit tests, integration tests, performance tests, and testing best practices. Proper testing ensures code quality, prevents regressions, and maintains Blender's reliability.

## 🎯 Testing Philosophy

### 🧩 Testing Principles
- **Comprehensive Coverage**: Test all critical functionality
- **Automated Testing**: Minimize manual testing requirements
- **Fast Execution**: Tests should run quickly for rapid feedback
- **Reliable Results**: Tests should be deterministic and stable
- **Clear Documentation**: Tests should serve as documentation

### 🔧 Testing Strategy
- **Unit Tests**: Test individual functions and classes
- **Integration Tests**: Test component interactions
- **Performance Tests**: Ensure performance requirements are met
- **Regression Tests**: Prevent reintroduction of bugs
- **User Acceptance Tests**: Validate user-facing functionality

## 📁 Test Organization

### 🏗️ Test Directory Structure

```
tests/
├── gtests/                  # Google Test framework tests
│   ├── blenlib/            # BlenLib unit tests
│   ├── blenkernel/         # BlenKernel unit tests
│   ├── editors/            # Editor unit tests
│   └── render/             # Render engine tests
├── python/                 # Python-based tests
│   ├── unit/               # Python unit tests
│   ├── integration/        # Integration tests
│   └── performance/        # Performance tests
├── performance/            # Performance benchmarks
└── files/                  # Test data files
```

### 🔧 Test Categories

#### 🧪 Unit Tests
- **Purpose**: Test individual functions and classes
- **Framework**: Google Test (C++), unittest (Python)
- **Location**: `tests/gtests/` and `tests/python/unit/`
- **Scope**: Single function or class

#### 🔗 Integration Tests
- **Purpose**: Test component interactions
- **Framework**: Custom test framework
- **Location**: `tests/python/integration/`
- **Scope**: Multiple components working together

#### ⚡ Performance Tests
- **Purpose**: Ensure performance requirements
- **Framework**: Custom benchmarking
- **Location**: `tests/performance/`
- **Scope**: Performance-critical operations

## 🧪 Unit Testing

### 📚 C++ Unit Tests (Google Test)

#### Test Structure
```cpp
// test_example.cpp
#include "gtest/gtest.h"
#include "BLI_math_vector.hh"

class MathTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup code before each test
  }
  
  void TearDown() override {
    // Cleanup code after each test
  }
};

TEST_F(MathTest, VectorAddition) {
  float3 a(1.0f, 2.0f, 3.0f);
  float3 b(4.0f, 5.0f, 6.0f);
  float3 result = a + b;
  
  EXPECT_FLOAT_EQ(result.x, 5.0f);
  EXPECT_FLOAT_EQ(result.y, 7.0f);
  EXPECT_FLOAT_EQ(result.z, 9.0f);
}

TEST_F(MathTest, VectorNormalization) {
  float3 vec(3.0f, 4.0f, 0.0f);
  float3 normalized = normalize(vec);
  
  EXPECT_NEAR(length(normalized), 1.0f, 1e-6f);
}
```

#### Running C++ Tests
```bash
# Run all C++ tests
make test

# Run specific test file
./bin/tests/gtests/blenlib/tests/BLI_math_test

# Run tests with verbose output
./bin/tests/gtests/blenlib/tests/BLI_math_test --gtest_verbose

# Run tests with specific filter
./bin/tests/gtests/blenlib/tests/BLI_math_test --gtest_filter="MathTest.*"
```

#### Test Best Practices
```cpp
// Use descriptive test names
TEST_F(MathTest, VectorAddition_WithPositiveValues_ReturnsCorrectSum)

// Test edge cases
TEST_F(MathTest, VectorNormalization_WithZeroVector_HandlesGracefully)

// Use appropriate assertions
EXPECT_EQ(expected, actual);           // Exact equality
EXPECT_NEAR(expected, actual, epsilon); // Floating point comparison
EXPECT_TRUE(condition);                // Boolean condition
EXPECT_FALSE(condition);               // Boolean condition
EXPECT_THROW(statement, exception);    // Exception testing
```

### 🐍 Python Unit Tests

#### Test Structure
```python
# test_example.py
import unittest
import bpy
import bmesh

class MeshTest(unittest.TestCase):
    def setUp(self):
        """Set up test environment before each test."""
        bpy.ops.wm.read_homefile(use_empty=True)
    
    def tearDown(self):
        """Clean up after each test."""
        bpy.ops.wm.read_homefile(use_empty=True)
    
    def test_cube_creation(self):
        """Test that a cube can be created successfully."""
        bpy.ops.mesh.primitive_cube_add(location=(0, 0, 0))
        obj = bpy.context.active_object
        
        self.assertIsNotNone(obj)
        self.assertEqual(obj.type, 'MESH')
        self.assertEqual(len(obj.data.vertices), 8)
        self.assertEqual(len(obj.data.polygons), 6)
    
    def test_mesh_boolean_union(self):
        """Test boolean union operation."""
        # Create two cubes
        bpy.ops.mesh.primitive_cube_add(location=(0, 0, 0))
        cube1 = bpy.context.active_object
        
        bpy.ops.mesh.primitive_cube_add(location=(1, 0, 0))
        cube2 = bpy.context.active_object
        
        # Perform boolean union
        bool_mod = cube1.modifiers.new(name="Boolean", type='BOOLEAN')
        bool_mod.object = cube2
        bool_mod.operation = 'UNION'
        
        # Apply modifier
        bpy.context.view_layer.objects.active = cube1
        bpy.ops.object.modifier_apply(modifier="Boolean")
        
        # Verify result
        self.assertGreater(len(cube1.data.vertices), 8)
```

#### Running Python Tests
```bash
# Run all Python tests
make test_python

# Run specific test file
python tests/python/unit/test_mesh.py

# Run tests with verbose output
python -m unittest tests/python/unit/test_mesh.py -v

# Run specific test method
python -m unittest tests.python.unit.test_mesh.MeshTest.test_cube_creation
```

## 🔗 Integration Testing

### 🎯 Integration Test Structure

#### Scene Integration Tests
```python
# test_scene_integration.py
import unittest
import bpy

class SceneIntegrationTest(unittest.TestCase):
    def setUp(self):
        bpy.ops.wm.read_homefile(use_empty=True)
    
    def test_object_creation_and_modification(self):
        """Test complete workflow of object creation and modification."""
        # Create object
        bpy.ops.mesh.primitive_cube_add(location=(0, 0, 0))
        obj = bpy.context.active_object
        
        # Add modifier
        subsurf = obj.modifiers.new(name="Subsurf", type='SUBSURF')
        subsurf.levels = 2
        
        # Add material
        mat = bpy.data.materials.new(name="TestMaterial")
        obj.data.materials.append(mat)
        
        # Add animation
        obj.keyframe_insert(data_path="location", frame=1)
        obj.location = (1, 1, 1)
        obj.keyframe_insert(data_path="location", frame=10)
        
        # Verify integration
        self.assertIsNotNone(obj.modifiers.get("Subsurf"))
        self.assertIsNotNone(obj.data.materials[0])
        self.assertGreater(len(obj.animation_data.action.fcurves), 0)
```

#### Render Integration Tests
```python
# test_render_integration.py
import unittest
import bpy

class RenderIntegrationTest(unittest.TestCase):
    def test_cycles_render_setup(self):
        """Test Cycles render engine setup and basic rendering."""
        # Set up scene
        bpy.context.scene.render.engine = 'CYCLES'
        bpy.context.scene.cycles.device = 'GPU'
        
        # Create test scene
        bpy.ops.mesh.primitive_cube_add(location=(0, 0, 0))
        bpy.ops.mesh.primitive_light_add(location=(5, 5, 5))
        
        # Set render settings
        bpy.context.scene.render.resolution_x = 64
        bpy.context.scene.render.resolution_y = 64
        bpy.context.scene.cycles.samples = 1
        
        # Test render
        result = bpy.ops.render.render(write_still=False)
        self.assertEqual(result, {'FINISHED'})
```

### 🔄 File I/O Integration Tests

```python
# test_file_io_integration.py
import unittest
import bpy
import tempfile
import os

class FileIOIntegrationTest(unittest.TestCase):
    def test_save_and_load_blend_file(self):
        """Test saving and loading blend files."""
        # Create test scene
        bpy.ops.mesh.primitive_cube_add(location=(0, 0, 0))
        original_obj = bpy.context.active_object
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(suffix='.blend', delete=False) as tmp:
            bpy.ops.wm.save_as_mainfile(filepath=tmp.name)
            filepath = tmp.name
        
        try:
            # Load file
            bpy.ops.wm.read_homefile(use_empty=True)
            bpy.ops.wm.open_mainfile(filepath=filepath)
            
            # Verify data integrity
            loaded_obj = bpy.context.scene.objects[0]
            self.assertEqual(loaded_obj.name, original_obj.name)
            self.assertEqual(loaded_obj.location, original_obj.location)
        finally:
            # Cleanup
            os.unlink(filepath)
```

## ⚡ Performance Testing

### 📊 Performance Test Structure

#### Benchmark Tests
```python
# test_performance.py
import unittest
import time
import bpy

class PerformanceTest(unittest.TestCase):
    def test_mesh_creation_performance(self):
        """Test performance of mesh creation operations."""
        start_time = time.time()
        
        # Create many objects
        for i in range(1000):
            bpy.ops.mesh.primitive_cube_add(location=(i, 0, 0))
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Performance assertion (should complete within 5 seconds)
        self.assertLess(duration, 5.0, f"Mesh creation took {duration:.2f} seconds")
    
    def test_modifier_performance(self):
        """Test performance of modifier operations."""
        # Create test mesh
        bpy.ops.mesh.primitive_cube_add(location=(0, 0, 0))
        obj = bpy.context.active_object
        
        # Add subdivision surface modifier
        subsurf = obj.modifiers.new(name="Subsurf", type='SUBSURF')
        subsurf.levels = 6
        
        start_time = time.time()
        
        # Apply modifier
        bpy.ops.object.modifier_apply(modifier="Subsurf")
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Performance assertion
        self.assertLess(duration, 2.0, f"Modifier application took {duration:.2f} seconds")
```

#### Memory Usage Tests
```python
# test_memory_usage.py
import unittest
import bpy
import psutil
import os

class MemoryUsageTest(unittest.TestCase):
    def test_memory_usage_under_load(self):
        """Test memory usage under heavy load."""
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss
        
        # Create heavy load
        for i in range(100):
            bpy.ops.mesh.primitive_cube_add(location=(i, 0, 0))
            obj = bpy.context.active_object
            
            # Add complex modifiers
            subsurf = obj.modifiers.new(name="Subsurf", type='SUBSURF')
            subsurf.levels = 4
            
            array_mod = obj.modifiers.new(name="Array", type='ARRAY')
            array_mod.count = 10
        
        final_memory = process.memory_info().rss
        memory_increase = final_memory - initial_memory
        
        # Memory usage assertion (should not exceed 500MB)
        self.assertLess(memory_increase, 500 * 1024 * 1024, 
                       f"Memory usage increased by {memory_increase / 1024 / 1024:.1f}MB")
```

## 🔍 Test Data Management

### 📁 Test Data Organization

#### Test Files Structure
```
tests/files/
├── blend/                   # Blend files for testing
│   ├── basic/              # Basic test scenes
│   ├── complex/            # Complex test scenes
│   └── regression/         # Files for regression testing
├── images/                 # Reference images
│   ├── expected/           # Expected output images
│   └── actual/             # Actual output images
├── scripts/                # Test scripts
└── data/                   # Other test data
```

#### Test Data Management
```python
# test_data_manager.py
import os
import bpy

class TestDataManager:
    @staticmethod
    def get_test_file_path(filename):
        """Get path to test file."""
        test_dir = os.path.join(os.path.dirname(__file__), "files")
        return os.path.join(test_dir, filename)
    
    @staticmethod
    def load_test_scene(filename):
        """Load test scene from file."""
        filepath = TestDataManager.get_test_file_path(filename)
        bpy.ops.wm.open_mainfile(filepath=filepath)
    
    @staticmethod
    def save_test_result(filename):
        """Save current scene as test result."""
        filepath = TestDataManager.get_test_file_path(filename)
        bpy.ops.wm.save_as_mainfile(filepath=filepath)
```

## 🚀 Continuous Integration Testing

### 🔄 CI/CD Pipeline

#### GitHub Actions Configuration
```yaml
# .github/workflows/tests.yml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake build-essential python3-dev
    
    - name: Build Blender
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j$(nproc)
    
    - name: Run C++ tests
      run: |
        cd build
        make test
    
    - name: Run Python tests
      run: |
        cd build
        make test_python
    
    - name: Run performance tests
      run: |
        cd build
        python tests/performance/run_benchmarks.py
```

### 📊 Test Reporting

#### Test Results Analysis
```python
# test_reporting.py
import unittest
import json
import time

class TestReporter:
    def __init__(self):
        self.results = {
            'total_tests': 0,
            'passed': 0,
            'failed': 0,
            'skipped': 0,
            'duration': 0,
            'details': []
        }
    
    def record_test(self, test_name, status, duration, error=None):
        """Record test result."""
        self.results['total_tests'] += 1
        self.results['duration'] += duration
        
        if status == 'passed':
            self.results['passed'] += 1
        elif status == 'failed':
            self.results['failed'] += 1
        elif status == 'skipped':
            self.results['skipped'] += 1
        
        self.results['details'].append({
            'name': test_name,
            'status': status,
            'duration': duration,
            'error': str(error) if error else None
        })
    
    def generate_report(self, output_file):
        """Generate test report."""
        with open(output_file, 'w') as f:
            json.dump(self.results, f, indent=2)
```

## 🔧 Testing Best Practices

### 📋 Test Writing Guidelines

#### Test Naming Conventions
```python
# Good test names
def test_vector_addition_with_positive_values_returns_correct_sum(self):
def test_mesh_boolean_union_combines_two_cubes_correctly(self):
def test_material_assignment_to_object_works_properly(self):

# Bad test names
def test1(self):
def test_vector(self):
def test_stuff(self):
```

#### Test Organization
```python
class ComprehensiveTest(unittest.TestCase):
    def setUp(self):
        """Set up test environment."""
        bpy.ops.wm.read_homefile(use_empty=True)
    
    def test_basic_functionality(self):
        """Test basic functionality."""
        # Arrange
        expected_result = 42
        
        # Act
        actual_result = function_under_test()
        
        # Assert
        self.assertEqual(actual_result, expected_result)
    
    def test_edge_cases(self):
        """Test edge cases and error conditions."""
        # Test with empty input
        with self.assertRaises(ValueError):
            function_under_test("")
        
        # Test with None input
        with self.assertRaises(TypeError):
            function_under_test(None)
    
    def test_performance_requirements(self):
        """Test performance requirements."""
        start_time = time.time()
        function_under_test()
        duration = time.time() - start_time
        
        self.assertLess(duration, 1.0, "Function took too long")
```

### 🔍 Debugging Tests

#### Test Debugging Techniques
```python
# Add debug output
def test_complex_operation(self):
    print(f"Debug: Starting test with {len(bpy.data.objects)} objects")
    
    # Perform operation
    result = complex_operation()
    
    print(f"Debug: Operation result: {result}")
    
    # Assertions
    self.assertIsNotNone(result)

# Use debugger
def test_with_debugger(self):
    import pdb; pdb.set_trace()  # Breakpoint
    # Test code here
```

#### Test Isolation
```python
class IsolatedTest(unittest.TestCase):
    def setUp(self):
        """Ensure clean state for each test."""
        bpy.ops.wm.read_homefile(use_empty=True)
        # Clear any remaining data
        bpy.data.objects.clear()
        bpy.data.meshes.clear()
        bpy.data.materials.clear()
    
    def tearDown(self):
        """Clean up after each test."""
        bpy.ops.wm.read_homefile(use_empty=True)
```

## 🔮 Future Testing Enhancements

### 🚀 Planned Improvements
- **Automated Test Generation**: Generate tests from code analysis
- **Visual Regression Testing**: Automated visual comparison testing
- **Performance Regression Detection**: Automatic performance regression alerts
- **Test Coverage Analysis**: Comprehensive coverage reporting

### 🔧 Development Roadmap
- **Enhanced Test Frameworks**: Improved testing tools and utilities
- **Parallel Test Execution**: Faster test execution through parallelization
- **Test Data Management**: Better organization and versioning of test data
- **Continuous Testing**: Real-time testing during development

---

*Comprehensive testing is essential for maintaining Blender's quality and reliability. Follow these guidelines to write effective, maintainable tests.*

**Last Updated**: 2024-12-19
**Version**: 1.0 