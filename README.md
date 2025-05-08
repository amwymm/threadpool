# C++ ThreadPool

A high-performance, feature-rich thread pool implementation in C++ for managing thread resources and task execution. This thread pool library offers flexible thread management with fixed and dynamic sizing modes, task queue management, and asynchronous task execution with return value retrieval.

## Features

- **Multiple Thread Pool Modes**:
  - `MODE_FIXED`: Maintains a fixed number of threads
  - `MODE_CACHED`: Dynamically adjusts thread count based on workload, with intelligent thread recycling

- **Thread Resource Management**:
  - Automatic thread creation and recycling in cached mode
  - Configurable idle thread timeout (default: 60 seconds)
  - Thread safety with mutex and condition variable synchronization

- **Task Queue**:
  - Configurable task queue size limits
  - Thread-safe task submission and execution
  - Blocking with timeout for task submission

- **Task Execution**:
  - Asynchronous task execution with customizable task implementations
  - Result retrieval mechanism with semaphore synchronization
  - Support for any return type using the `Any` type-erasure class

## Requirements

- C++11 or later
- Standard library support for:
  - `std::thread`
  - `std::mutex`
  - `std::condition_variable`
  - `std::function`
  - Smart pointers (`std::unique_ptr`, `std::shared_ptr`)

## Usage

### Basic Example

```cpp
#include "threadpool.h"
#include <iostream>
#include <chrono>

// Define a custom task by inheriting from Task
class MyTask : public Task {
public:
    MyTask(int id) : id_(id) {}
    
    // Override the run method to define your task logic
    Any run() override {
        // Simulate some work
        std::cout << "Task " << id_ << " is running on thread " 
                  << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return id_ * 10; // Return a result
    }
    
private:
    int id_;
};

int main() {
    // Create thread pool
    ThreadPool pool;
    
    // Optional: Configure the pool
    pool.setMode(PoolMode::MODE_CACHED);
    pool.setTaskQueMaxThreshHold(1000);
    pool.setThreadSizeThreshHold(10);
    
    // Start the pool with 4 initial threads
    pool.start(4);
    
    // Submit tasks and get results
    std::vector<Result> results;
    for (int i = 0; i < 10; i++) {
        auto task = std::make_shared<MyTask>(i);
        results.push_back(pool.submitTask(task));
    }
    
    // Get results from completed tasks
    for (int i = 0; i < 10; i++) {
        int result = results[i].get().cast_<int>();
        std::cout << "Task " << i << " result: " << result << std::endl;
    }
    
    // ThreadPool destructor will clean up resources
    return 0;
}
```

### Thread Pool Modes

```cpp
// Fixed mode: maintains a constant number of threads
ThreadPool fixedPool;
fixedPool.setMode(PoolMode::MODE_FIXED);
fixedPool.start(8); // Start with 8 threads

// Cached mode: dynamically adjusts thread count based on workload
ThreadPool cachedPool;
cachedPool.setMode(PoolMode::MODE_CACHED);
cachedPool.setThreadSizeThreshHold(16); // Set maximum thread limit
cachedPool.start(4); // Start with 4 threads, can grow up to 16
```

### Task with Custom Return Type

```cpp
class StringTask : public Task {
public:
    Any run() override {
        // Perform string operations
        std::string result = "Processed data";
        return result;
    }
};

// Submit task and get string result
auto task = std::make_shared<StringTask>();
Result result = pool.submitTask(task);
std::string strResult = result.get().cast_<std::string>();
```

## Key Components

### Thread Pool

The `ThreadPool` class manages a collection of worker threads and a task queue. It provides methods for configuring the pool, submitting tasks, and starting/stopping the pool.

### Task

The `Task` class is an abstract base class that defines the interface for tasks that can be submitted to the thread pool. Users should inherit from this class and override the `run()` method to define custom task logic.

### Result

The `Result` class provides a mechanism for retrieving the return value of a task. It uses a semaphore to synchronize between the task execution thread and the thread that requests the result.

### Any

The `Any` class is a type-erasure class that allows storing and retrieving values of any type. It is used to support returning arbitrary data types from tasks.

## Advanced Configuration

### Thread Count

```cpp
// Set initial thread count (default is hardware concurrency)
pool.start(8);

// Set maximum thread count for cached mode
pool.setThreadSizeThreshHold(20);
```

### Task Queue Size

```cpp
// Set maximum task queue size (default is INT32_MAX)
pool.setTaskQueMaxThreshHold(1000);
```

## Thread Safety

The ThreadPool implementation is thread-safe. Multiple threads can simultaneously:
- Submit tasks to the pool
- Retrieve results from completed tasks
- Adjust pool settings (before starting the pool)

## Performance Considerations

- The thread pool is optimized for both small, quick tasks (in cached mode) and longer-running tasks (in fixed mode)
- Idle threads in cached mode are automatically recycled after 60 seconds of inactivity
- Task submission will block (with timeout) if the task queue is full
- Task result retrieval will block until the task completes

## License

This thread pool implementation is provided as-is with no warranty.

## Acknowledgments

Original implementation by 王强 (Wang Qiang)
