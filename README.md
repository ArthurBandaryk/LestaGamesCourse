# LestaGamesCourse

![Lesta Games](avatar/lesta-games.png)

## C++ game dev course provided by [`Lesta Games`](https://lesta.ru/ru) company.

Build Platform        | Status
--------------------- | ----------------------
Linux x64(gcc, clang) | [![Ubuntu](https://github.com/ArthurBandaryk/LestaGamesCourse/actions/workflows/github-ci-pipeline.yaml/badge.svg)](https://github.com/ArthurBandaryk/LestaGamesCourse/actions/workflows/github-ci-pipeline.yaml)

# Building project

```
git clone --recurse-submodules https://github.com/ArthurBandaryk/LestaGamesCourse.git
cd LestaGamesCourse/
cmake -B .build -G "Ninja" ..
cmake --build .build
cmake --install .build
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
ctest --test-dir build/path-to-dir-containing-executables-needed
```

# Visual Studio Code setup

If you are using [`Visual Studio Code`](https://code.visualstudio.com/) you can configure the project following the next steps:

1. Firstly make sure that you have installed the following plugins for better development process:

    * [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
    * [Clang-format](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format)
    * [CMake](https://marketplace.visualstudio.com/items?itemName=josetr.cmake-language-support-vscode)
    * [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
    * [cmake-format](https://marketplace.visualstudio.com/items?itemName=cheshirekow.cmake-format#:~:text=The%20cmake%2Dformat%20extension%20for,for%20cmake%2Dformat%20in%20github.)
    * [Python](https://marketplace.visualstudio.com/items?itemName=ms-python.python)

2. You can base on the following `.vscode/settings.json` to specify pathes to executables needed (clang-format, cmake-format, compilers, etc.)

```
{
    "editor.formatOnSave": true,
    "clang-format.executable": "/usr/bin/clang-format",
    "cmakeFormat.exePath": "/usr/local/bin/cmake-format",
    "cmake.cmakePath": "/home/arc1/.local/bin/cmake",
    "cmakeFormat.args": [
        "--config-file",
        "${workspaceFolder}/.cmake-format.py"
    ],
}
```

3. You need to configure debugging mode. A good example is [`here`](https://www.youtube.com/watch?v=BBPKMRR69_s&list=LL&index=25&t=953s&ab_channel=LearnQtGuide)

4. `Enjoy it :))`
