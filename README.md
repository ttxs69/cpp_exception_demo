# C++ Exception Stack Trace Demo

This repository demonstrates an advanced, non-intrusive technique for capturing a stack trace at the moment an exception is thrown in C++. This is a common requirement for debugging and logging, as it provides crucial context about where an error originated without requiring any changes to the application's source code at the `throw` sites.

## Dependencies

You will need the following tools and libraries to build and run this project.

1.  **C++17 Compiler**: A compiler that supports C++17 and the Itanium C++ ABI.
    *   **macOS**: Apple Clang (available via Xcode Command Line Tools)
    *   **Linux**: GCC or Clang (`sudo apt-get install build-essential g++`)

2.  **CMake**: A modern build system generator (version 3.10 or later).
    *   **macOS**: `brew install cmake`
    *   **Linux**: `sudo apt-get install cmake`

3.  **Ninja**: A fast, small build system (optional, but recommended).
    *   **macOS**: `brew install ninja`
    *   **Linux**: `sudo apt-get install ninja-build`

4.  **Boost**: The Boost C++ libraries are required, specifically the `stacktrace` component. The recommended way to install Boost is with vcpkg, as described in the example below.

## Full Example: Building with vcpkg

This section provides a complete, step-by-step guide to building the project using the vcpkg package manager.

**Step 1: Install vcpkg**

First, clone the vcpkg repository and run its bootstrap script. This only needs to be done once.

```sh
# Clone the vcpkg repository
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

# Run the bootstrap script
./bootstrap-vcpkg.sh
```

**Step 2: Set up Environment Variable**

For CMake to find the vcpkg toolchain, you need to set the `VCPKG_ROOT` environment variable.

```sh
# Set the variable for the current terminal session
export VCPKG_ROOT=$(pwd)

# Optional: Add the line above to your shell profile (~/.bashrc, ~/.zshrc, etc.)
# to make it permanent.
```

**Step 3: Install Boost Dependency**

Use vcpkg to install the `boost-stacktrace` package.

```sh
# The 'vcpkg' executable is now in the root of the vcpkg directory
./vcpkg install boost-stacktrace
```

**Step 4: Clone and Build This Project**

Now that the dependencies are ready, you can clone and build the `exception_demo` project.

```sh
# Navigate to a different directory to clone the project
cd ..
git clone https://github.com/your-username/exception_demo.git # Replace with the actual URL
cd exception_demo

# Configure with CMake. It will automatically use vcpkg.
cmake -B build -G Ninja

# Build with Ninja
ninja -C build

# Run the application
./build/my_app
```

## Example Output

Running the application will produce output similar to the following. The exact paths and addresses will vary depending on your system.

```
This is a test exception
Stacktrace from throw site:
 0# __cxa_throw in .../build/my_app
 1# add(int, int) in .../build/my_app
 2# main in .../build/my_app
```

## How It Works: Global Interception via `__cxa_throw`

This project uses a powerful, low-level technique called **symbol interposition** to automatically capture a stack trace for any exception thrown in the application.

The core idea is to provide our own implementation of `__cxa_throw`, which is a special, low-level function in the C++ ABI that the compiler internally calls for every `throw` statement.

The process is as follows:

1.  **Override `__cxa_throw`**: We define a function with the exact same signature as the system's `__cxa_throw`. At link time, the linker sees our version first and redirects all calls to it.

2.  **Capture the Stack Trace**: Inside our custom function, the first thing we do is capture the current call stack using `boost::stacktrace::stacktrace`.

3.  **Store the Trace**: The captured stack trace is stored in a `thread_local` variable. This is crucial for ensuring the mechanism is safe in multi-threaded applications, as each thread gets its own separate storage for the stack trace.

4.  **Chain to the Original Function**: We use `dlsym(RTLD_NEXT, "__cxa_throw")` to find the address of the *original*, system-provided `__cxa_throw` function. We then call it, passing along the original arguments, so that the normal exception handling process can continue.

5.  **Retrieve in `catch` block**: In the `main` function's `catch` block, we can now access the `thread_local` variable to retrieve and print the stack trace that was captured at the throw site.

This approach is completely transparent to the application code, but it is **not portable**. It relies on the specifics of the Itanium C++ ABI and will not work on platforms with different ABIs, such as Windows with MSVC.
