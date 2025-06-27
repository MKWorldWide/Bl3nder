# 📚 BlenLib - Core Utility Library

## 📋 Overview

**BlenLib** (`source/blender/blenlib/`) is Blender's core utility library that provides fundamental data structures, algorithms, and utility functions used throughout the entire Blender codebase. It serves as the foundation upon which all other Blender components are built.

## 🎯 Purpose and Responsibilities

### 🧩 Core Functionality
- **Data Structures**: Custom implementations optimized for 3D data and performance
- **Mathematical Operations**: Vector, matrix, quaternion, and mathematical utilities
- **Memory Management**: Custom allocators and memory utilities
- **String Handling**: Unicode-aware string operations and utilities
- **File I/O**: Low-level file handling and data serialization
- **Time Management**: High-precision timing and animation support
- **Threading**: Multi-threading utilities and synchronization primitives

### 🔧 Design Principles
- **Performance**: Optimized for real-time 3D operations
- **Memory Efficiency**: Custom allocators for performance-critical operations
- **Cross-Platform**: Consistent behavior across Windows, macOS, and Linux
- **Type Safety**: Strong typing and compile-time checks
- **SIMD Optimization**: Vectorized operations where possible

## 📁 File Organization

### 🏗️ Core Headers

#### 📊 Data Structures
- **`BLI_vector.hh`**: Dynamic array implementation with STL-like interface
- **`BLI_set.hh`**: Set data structure with various implementations
- **`BLI_map.hh`**: Map/dictionary data structure
- **`BLI_stack.hh`**: Stack data structure
- **`BLI_span.hh`**: Non-owning view into contiguous data
- **`BLI_virtual_array.hh`**: Virtual array for lazy evaluation

#### 🧮 Mathematical Operations
- **`BLI_math_vector_types.hh`**: Vector types (float2, float3, float4, etc.)
- **`BLI_math_matrix_types.hh`**: Matrix types and operations
- **`BLI_math_rotation_types.hh`**: Quaternion and rotation utilities
- **`BLI_math_geom.h`**: Geometric operations and algorithms
- **`BLI_math_bits.h`**: Bit manipulation utilities

#### 🔤 String Operations
- **`BLI_string.h`**: Core string manipulation functions
- **`BLI_string_utf8.h`**: UTF-8 string handling
- **`BLI_string_utils.hh`**: String utility functions
- **`BLI_string_search.hh`**: String search and matching algorithms

#### 💾 Memory Management
- **`BLI_memory_utils.hh`**: Memory allocation and management utilities
- **`BLI_mempool.h`**: Memory pool allocator
- **`BLI_memarena.h`**: Memory arena for temporary allocations
- **`BLI_guardedalloc.h`**: Guarded memory allocation for debugging

#### ⏰ Time and Animation
- **`BLI_time.h`**: Time measurement utilities
- **`BLI_timecode.h`**: Timecode handling for video/animation
- **`BLI_timer.h`**: High-precision timing utilities

#### 🧵 Threading and Concurrency
- **`BLI_task.hh`**: Task scheduling and parallel execution
- **`BLI_threads.h`**: Threading utilities and synchronization
- **`BLI_mutex.hh`**: Mutex and lock utilities

## 🔧 Key Components

### 📊 Vector System

The vector system provides optimized mathematical operations for 3D graphics:

```cpp
// Vector types for different precision needs
float2 vec2;      // 2D float vector
float3 vec3;      // 3D float vector  
float4 vec4;      // 4D float vector
double2 dvec2;    // 2D double vector
double3 dvec3;    // 3D double vector

// Common operations
float3 result = vec1 + vec2;           // Vector addition
float3 result = vec1 * scalar;         // Scalar multiplication
float dot_product = dot(vec1, vec2);   // Dot product
float3 cross_product = cross(vec1, vec2); // Cross product
```

**Performance Features**:
- SIMD-optimized operations on supported platforms
- Cache-friendly memory layout
- Compile-time optimizations

### 🧮 Matrix System

Comprehensive matrix operations for transformations:

```cpp
// Matrix types
float3x3 mat3;    // 3x3 transformation matrix
float4x4 mat4;    // 4x4 transformation matrix

// Common operations
mat4 transform = mat4::identity();     // Identity matrix
mat4 rotation = mat4::rotation(angle, axis); // Rotation matrix
mat4 translation = mat4::translation(vec3);  // Translation matrix
mat4 combined = translation * rotation * scale; // Matrix multiplication
```

**Key Features**:
- Optimized matrix multiplication
- Transformation composition
- Inverse and transpose operations
- Perspective and orthographic projections

### 📦 Container System

Modern C++ containers optimized for Blender's needs:

```cpp
// Dynamic array with automatic resizing
Vector<float> values;
values.append(1.0f);
values.append(2.0f);

// Set with fast lookup
Set<int> unique_values;
unique_values.add(42);

// Map for key-value storage
Map<StringRef, float> parameters;
parameters.add_new("scale", 2.0f);

// Stack for LIFO operations
Stack<int> undo_stack;
undo_stack.push(operation);
```

**Performance Optimizations**:
- Small object optimization
- Memory pooling for frequent allocations
- Cache-friendly data layouts

### 🔤 String System

Unicode-aware string handling:

```cpp
// String utilities
StringRef str = "Hello World";
bool contains = str.contains("World");
StringRef substr = str.substr(0, 5);  // "Hello"

// UTF-8 handling
const char *utf8_str = "Hello 世界";
int len = BLI_strlen_utf8(utf8_str);
char *normalized = BLI_str_normalize_utf8(utf8_str);

// String search
Vector<StringRef> matches = BLI_string_search_find_all(
    query, candidates, max_results);
```

**Features**:
- UTF-8 encoding support
- Unicode normalization
- Efficient string search algorithms
- Memory-efficient string views

## ⚡ Performance Considerations

### 🚀 Optimization Strategies

#### SIMD Operations
```cpp
// Automatic SIMD optimization for vector operations
float3 result = vec1 + vec2;  // May use SSE/AVX instructions

// Explicit SIMD usage
#ifdef __SSE__
  __m128 a = _mm_load_ps(&vec1.x);
  __m128 b = _mm_load_ps(&vec2.x);
  __m128 result = _mm_add_ps(a, b);
  _mm_store_ps(&output.x, result);
#endif
```

#### Memory Management
```cpp
// Memory pool for frequent allocations
BLI_mempool *pool = BLI_mempool_create(sizeof(MyStruct), 100, 100, BLI_MEMPOOL_NOP);
MyStruct *ptr = (MyStruct *)BLI_mempool_alloc(pool);

// Memory arena for temporary allocations
MemArena *arena = BLI_memarena_new(MEM_SIZE_OPTIMAL(1 << 16));
void *ptr = BLI_memarena_alloc(arena, size);
```

#### Cache Optimization
```cpp
// Structure of Arrays (SoA) for better cache performance
struct VertexData {
  Vector<float3> positions;
  Vector<float3> normals;
  Vector<float2> uvs;
};

// Instead of Array of Structures (AoS)
struct Vertex {
  float3 position;
  float3 normal;
  float2 uv;
};
Vector<Vertex> vertices;
```

## 🔧 Usage Patterns

### 📊 Common Data Structure Usage

#### Vector Operations
```cpp
// Efficient vector operations
Vector<float3> vertices;
vertices.reserve(expected_count);  // Pre-allocate memory

// Batch operations
for (const float3 &vertex : vertices) {
  // Process vertex
}

// Memory-efficient iteration
for (const float3 &vertex : vertices.as_span()) {
  // Process vertex without copying
}
```

#### Set Operations
```cpp
// Fast lookup and uniqueness
Set<int> unique_indices;
for (int index : indices) {
  unique_indices.add(index);
}

// Set operations
Set<int> set1, set2;
Set<int> intersection = set1.intersection(set2);
Set<int> union_set = set1.union_set(set2);
```

#### Map Usage
```cpp
// Key-value storage
Map<StringRef, float> parameters;
parameters.add_new("scale", 2.0f);
parameters.add_new("rotation", 45.0f);

// Safe lookup
if (float *scale = parameters.lookup_ptr("scale")) {
  *scale *= 2.0f;
}
```

### 🧮 Mathematical Operations

#### Vector Math
```cpp
// 3D vector operations
float3 position(1.0f, 2.0f, 3.0f);
float3 direction = normalize(position);
float distance = length(position);

// Vector interpolation
float3 start(0.0f, 0.0f, 0.0f);
float3 end(1.0f, 1.0f, 1.0f);
float3 interpolated = interpolate(start, end, 0.5f);
```

#### Matrix Transformations
```cpp
// Transformation matrix creation
float4x4 transform = mat4::identity();
transform = mat4::translation(float3(1.0f, 2.0f, 3.0f)) * transform;
transform = mat4::rotation(radians(45.0f), float3(0.0f, 0.0f, 1.0f)) * transform;

// Applying transformations
float3 point(0.0f, 0.0f, 0.0f);
float3 transformed = transform * point;
```

## 🔍 Debugging and Development

### 🐛 Debugging Tools

#### Memory Debugging
```cpp
// Guarded allocation for debugging
#ifdef DEBUG
  void *ptr = MEM_mallocN(size, "MyStruct");
  // Memory is tracked and validated
#else
  void *ptr = malloc(size);
#endif
```

#### Performance Profiling
```cpp
// High-precision timing
SCOPED_TIMER("MyOperation");
// Timer automatically reports duration when scope exits

// Manual timing
double start_time = BLI_time_now_seconds();
// ... operation ...
double end_time = BLI_time_now_seconds();
double duration = end_time - start_time;
```

### 🔧 Development Guidelines

#### Code Style
- Use modern C++ features (C++17/20)
- Prefer `const` correctness
- Use RAII for resource management
- Follow Blender's naming conventions

#### Performance Guidelines
- Profile before optimizing
- Use appropriate data structures
- Minimize memory allocations
- Leverage SIMD when possible

#### Testing
```cpp
// Unit test example
TEST(vector, basic_operations) {
  Vector<int> vec;
  vec.append(1);
  vec.append(2);
  EXPECT_EQ(vec.size(), 2);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
}
```

## 🔗 Integration with Other Components

### 🎨 BlenKernel Integration
BlenLib provides the foundation for BlenKernel's data management:
```cpp
// BlenKernel uses BlenLib containers
struct Mesh {
  Vector<float3> vert_positions;
  Vector<int3> face_vertices;
  // ...
};
```

### 🎬 Render Engine Integration
Render engines use BlenLib for mathematical operations:
```cpp
// Cycles and EEVEE use BlenLib math
float3 world_position = transform_point(world_matrix, local_position);
float3 world_normal = transform_direction(world_matrix, local_normal);
```

### 🐍 Python API Integration
Python API exposes BlenLib functionality:
```python
# Python can access BlenLib math operations
import bmesh
import mathutils

# BlenLib math is used internally
vertex = bmesh.ops.create_vert(bm, co=(1.0, 2.0, 3.0))
```

## 🔮 Future Enhancements

### 🚀 Planned Improvements
- **SIMD Optimization**: Enhanced vectorization for more operations
- **Memory Management**: Improved allocators and garbage collection
- **Concurrency**: Better threading and parallel processing support
- **C++20 Features**: Adoption of modern C++ features

### 🔧 Development Roadmap
- **Performance Profiling**: Enhanced profiling and optimization tools
- **Documentation**: Automated API documentation generation
- **Testing**: Comprehensive unit test coverage
- **Benchmarking**: Performance benchmarking suite

---

*BlenLib serves as the foundation of Blender's technical architecture. Its performance and reliability are critical to the entire system's success.*

**Last Updated**: 2024-12-19
**Version**: 1.0 