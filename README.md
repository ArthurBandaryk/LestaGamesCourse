# LestaGamesCourse

![Lesta Games](avatar/lesta-games.png)

## C++ game dev course provided by [`Lesta Games`](https://lesta.ru/ru) company.

Build Platform        | Status
--------------------- | ----------------------
Linux x64(gcc, clang) | [![Ubuntu](https://github.com/ArthurBandaryk/LestaGamesCourse/actions/workflows/github-ci-pipeline.yaml/badge.svg)](https://github.com/ArthurBandaryk/LestaGamesCourse/actions/workflows/github-ci-pipeline.yaml)

# Building project

```

git clone https://github.com/ArthurBandaryk/LestaGamesCourse.git
cd LestaGamesCourse/
mkdir -p build && cd build
cmake -G Ninja ..
cmake --build .
cmake --install .

```

## Running executables

When building phase is done you can easily find executables needed in `build` directory `or` in `bin/install` directory. For example you can run executable from `bin/install` directory in the following way:

```

.bin/install/hello-bin-dynamic


```

or if you are already in `bin/install` directory:

```
./hello-bin-dynamic

```

# Running `CMake` tests

You can run `CMake` tests from the root directory in the following way:

```
ctest --test-dir build/01-homework-hello/01-1-simple-hello/

```
