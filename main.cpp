#include <boost/stacktrace.hpp>
#include <iostream>
#include <stdexcept>
#include <dlfcn.h>

// Thread-local storage for the stacktrace
thread_local boost::stacktrace::stacktrace current_stacktrace;

#include <typeinfo>

// Pointer to the original __cxa_throw function
using cxa_throw_type = void (*)(void*, std::type_info*, void (*) (void*));
cxa_throw_type original_cxa_throw = nullptr;

extern "C" {
    // Our custom __cxa_throw function
    void __cxa_throw(void* thrown_exception, std::type_info* tinfo, void (*dest)(void*)) {
        // Capture the stacktrace
        current_stacktrace = boost::stacktrace::stacktrace();

        // Find and call the original __cxa_throw
        if (!original_cxa_throw) {
            original_cxa_throw = (cxa_throw_type)dlsym(RTLD_NEXT, "__cxa_throw");
        }
        original_cxa_throw(thrown_exception, tinfo, dest);
    }
}

void add(int a, int b) {
  // Throw a standard exception without any modification
  throw std::runtime_error("This is a test exception");
}

int main() {
  try {
    add(1, 2);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << "Stacktrace from throw site:" << std::endl;
    std::cerr << current_stacktrace;
  }
  return 0;
}
