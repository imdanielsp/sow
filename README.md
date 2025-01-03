![CI](https://github.com/imdanielsp/sow/actions/workflows/build.yml/badge.svg)

# SOW – Save-On-Write

The SOW library is a header-only C++ library that provides a way to save data to disk only when it is modified. This is
useful when you have a data structure that is not modified frequently, but when it is, it is important to save it to
disk immediately.

## Motivation

The SOW library was created to solve a problem I encountered while working on a project that required saving a data
structure immediately after it was modified. The data structure was not modified frequently, but when it was, it was
important to save it to disk immediately.

Normally, applications would mutate the data structure in memory and then save it to disk by explicitly invoking a save
routine.

```cpp
MyData data;
data.a = 42;
data.b = 43;

// Save the data to disk
save_data_fn(data);
```

This approach works well until someone forgets to save the data structure to disk after modifying it. This can
lead to data loss if the application crashes before the data structure is saved, or worst, unexpected behavior if the
application continues to run with the old data.

The SOW library solves this problem by only allowing mutation via a proxy object (a `SowGuard`) that saves the data to
disk when it is destroyed.

```cpp
sow::sow data { MyData{}, save_data_fn };

data->get_mut()->a = 42;
```

Where the `save_data_fn` is a function that saves the data to disk.

```cpp
void save_data_fn(const MyData& data) {
    // Save the data to disk
}
```

But what if the data structure is modified multiple times before it is saved to disk?

```cpp
sow::sow data { MyData{}, save_data_fn };

{
    auto guard = data->get_mut();
    guard->a = 42;
    guard->b = 43;
}
```

Or "sow it" (save-on-write it):

```cpp
sow::sow data { MyData{}, save_data_fn };

sow::it(data, [](auto& data) {
    data.a = 42;
    data.b = 43;
});
```

# Installation

The SOW library is a header-only library. To use it, simply include the `sow.hpp` header file in your project or run the
make install command to install it on your system.

```sh
cmake -B build -S .
cmake --install build # Needs sudo
```

# Testing

The SOW library comes with a test suite that can be run using the following commands:

```sh
cmake -B build -S .
cmake --build build
cd build
ctest
Test project /Users/dsantos/Documents/projects/sow/build
      Start  1: SowTest.SowConstGet
 1/12 Test  #1: SowTest.SowConstGet .......................   Passed    0.01 sec
      Start  2: SowTest.SowConstDeref
 2/12 Test  #2: SowTest.SowConstDeref .....................   Passed    0.00 sec
      Start  3: SowTest.SowConstArrow
 3/12 Test  #3: SowTest.SowConstArrow .....................   Passed    0.00 sec
      Start  4: SowTest.SowGuardOnce
 4/12 Test  #4: SowTest.SowGuardOnce ......................   Passed    0.00 sec
      Start  5: SowTest.SowGuardMultiple
 5/12 Test  #5: SowTest.SowGuardMultiple ..................   Passed    0.00 sec
      Start  6: SowTest.SowGuardMultipleWithFunc
 6/12 Test  #6: SowTest.SowGuardMultipleWithFunc ..........   Passed    0.00 sec
      Start  7: SowTest.SowWithDefaultConstructor
 7/12 Test  #7: SowTest.SowWithDefaultConstructor .........   Passed    0.00 sec
      Start  8: SowTest.SowWithDefaultConstructedWriter
 8/12 Test  #8: SowTest.SowWithDefaultConstructedWriter ...   Passed    0.00 sec
      Start  9: SowTest.SowWithLambdaWriter
 9/12 Test  #9: SowTest.SowWithLambdaWriter ...............   Passed    0.00 sec
      Start 10: SowTest.SowWithFreeFuncWriter
10/12 Test #10: SowTest.SowWithFreeFuncWriter .............   Passed    0.00 sec
      Start 11: SowTest.SowTrivialDataType
11/12 Test #11: SowTest.SowTrivialDataType ................   Passed    0.00 sec
      Start 12: SowTest.SowSTLType
12/12 Test #12: SowTest.SowSTLType ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 12

Total Test time (real) =   0.05 sec
```
