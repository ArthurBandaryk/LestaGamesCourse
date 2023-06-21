# Arcanoid game

Build Platform          |
------------------------|
Linux x64(gcc, clang)   |
Windows x64(MSVC, LLVM) |

![Arcanoid](help-imgs/game.png)

## Build steps for Windows

### Using LLVM compiler infrastructure

First of all make sure you have installed llvm. Use [`this`](https://github.com/llvm/llvm-project/releases/tag/llvmorg-15.0.7) link to download exe needed.
You may also need for [`ninja`](https://github.com/ninja-build/ninja/releases)
And of course [`cmake`](https://cmake.org/download/#latest)

```
git clone --recurse-submodules https://github.com/ArthurBandaryk/LestaGamesCourse.git
cd LestaGamesCourse/arcanoid
cmake -B build -G "Ninja" -S .
cmake --build build

```

To `run` this game just do the following:

```
cd build && arcanoid.exe

```

### Using MSVC



