# C++ Exception Stack Trace Demo

This repository demonstrates an advanced, non-intrusive technique for capturing a stack trace at the moment an exception is thrown in C++. This is a common requirement for debugging and logging, as it provides crucial context about where an error originated without requiring any changes to the application's source code at the `throw` sites.

## Prerequisites

- A C++17 compliant compiler that uses the Itanium C++ ABI (e.g., GCC, Clang on macOS or Linux)
- [CMake](https://cmake.org/) (version 3.10 or later)
- [Boost](https://www.boost.org/) libraries (specifically `stacktrace`)
- [Ninja](https://ninja-build.org/) build system (or you can use Make)

## Building and Running

1.  **Configure with CMake:**
    ```sh
    cmake -B build
    ```

2.  **Build with Ninja:**
    ```sh
    ninja -C build
    ```

3.  **Run the application:**
    ```sh
    ./build/my_app
    ```
    The output will show the exception message followed by the stack trace captured from the location where the exception was thrown.

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