# Salloc

Specialized memory allocators in C++

## Implementations
- Stack allocator 
- Linear allocator 
- Fixed block allocator 
- Predefined block allocator 
- General block allocator 

## Example

```c++
// Define the allocator you want to use
// In this case, Block allocator
BlockAllocator alloc;

// Allocate some memory..
void* m = alloc.Allocate(512);
alloc.Free(m, 512);

for (int i = 1; i <= 1024; i++)
{
    alloc.Allocate(i);
}

// Clear all allocations at once!
alloc.Clear();

// And reuse it from scratch
m = alloc.Allocate(128);

```

## Building and running
- Install [CMake](https://cmake.org/install/)
- Ensure CMake is in the system `PATH`
- Clone the repository `git clone https://github.com/Sopiro/Salloc`
- Run CMake build script depend on your system
  - Visual Studio: Run `build.bat`
  - Otherwise: Run `build.sh`
