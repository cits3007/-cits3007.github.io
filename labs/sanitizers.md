---
title:  Quick guide to Google sanitizers
---


## What are the Google sanitizers? Why use them?


Some programming languages are [memory-safe](https://en.wikipedia.org/wiki/Memory_safety):
the language runtime prevents us from accessing uninitialised memory, from reading or writing
data beyond the bounds of an array, and from using an object after its memory has been reclaimed.

Python and Java are examples of memory-safe languages -- in those languages, if we
accidentally try to access a position outside the bounds of an array or list, the language
runtime will detect this, and throw an [exception][exception]; assuming we do
not catch it, the exception will cause the program to terminate, and an
informative [stack trace][stack-trace] will be printed, showing where the original error
occurred, and what function or method calls led up to that point.

[exception]: https://en.wikipedia.org/wiki/Exception_handling
[stack-trace]: https://en.wikipedia.org/wiki/Stack_trace

For example, suppose we write the following Python code in a file `example.py`:

```{ .python .numberLines }
arr = [1, 2, 3, 4, 5]

def print_eleventh_el():
  print(arr[10]) # try to access an out-of-bounds element

print_eleventh_el()
```

If we run this code, we'll get something like the following stack trace:

```python
Traceback (most recent call last):
  File "/home/user/example.py", line 6, in <module>
    print_eleventh_el()
  File "/home/user/example.py", line 4, in print_eleventh_el
    print(arr[10]) # try to access an out-of-bounds element
IndexError: list index out of range
```

which shows exactly where the exception was thrown (line 4), and how we got there.

In C, however, the language runtime does not check for memory-safety problems. By default,
if we read or write outside the bounds of an array, we'll be given no warning, but may
silently corrupt our program's data or [introduce security vulnerabilities][buf-overflow].
Similarly, we may be given little or no warning if we read from uninitialised memory, or from
memory that has been [deallocated and is no longer valid][dangling-pointer]. If we are
lucky, our error might cause a program [segmentation fault][segfault], but even if that does
occur, the C runtime does not display a useful stack trace.

[buf-overflow]: https://en.wikipedia.org/wiki/Buffer_overflow
[dangling-pointer]: https://en.wikipedia.org/wiki/Dangling_pointer
[segfault]: https://en.wikipedia.org/wiki/Segmentation_fault

For example, on many platforms, if we compile and run a C equivalent to the above Python code:

```{ .C .numberLines }
#include <stdio.h>

int arr[] = {1, 2, 3, 4, 5};

void print_eleventh_el() {
  printf("%d\n", arr[10]);
}

int main() {
  print_eleventh_el();
}
```

we will be given no warning at all, nor will any any sort of error be shown: the program
will simply run and produce an incorrect result (for instance, printing "0").

The [Google sanitizers][sanitizers] offer a solution. First introduced in 2012, they are
available in all the leading C compilers (GCC, Clang, XCode, and MSVC), and make diagnosing
memory errors in C *almost* as easy as it is in Java or Python.
The sanitizers work by adding extra data and machine-code instructions to your code when it is
compiled (this is called "*instrumenting*" the code), which at runtime will detect issues
such as memory misuse.

[sanitizers]: https://en.wikipedia.org/wiki/Code_sanitizer

There are several sanitizers available, each focusing on a different aspect of code safety:

- **AddressSanitizer (ASan)** helps detect memory errors such as buffer overflows, use-after-free bugs, and memory leaks.
- **UndefinedBehaviorSanitizer (UBSan)** identifies multiple sorts of [undefined
  behavior][ub] (such as division by zero or null pointer dereferencing) that can lead to
  unpredictable results and potential security risks.
- **MemorySanitizer (MSan)** detects reads from uninitialized memory.
- **ThreadSanitizer (TSan)** detects data races, potential deadlocks and [other concurrency
  issues][tsan-bugs] in multi-threaded programs.

[tsan-bugs]: https://github.com/google/sanitizers/wiki/ThreadSanitizerDetectableBugs
[ub]: https://en.wikipedia.org/wiki/Undefined_behavior

With several of these sanitizers enabled, then instead of silently producing wrong output,
our C program will diagnose the error and produce a helpful stack trace:

```
example.c:6:21: runtime error: index 10 out of bounds for type 'int [5]'
example.c:6:3: runtime error: load of address 0x55c7c36a4048 with insufficient space for an object of type 'int'
0x55c7c36a4048: note: pointer points here
 00 00 00 00  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  00 00 0b 00
              ^
=================================================================
==575038==ERROR: AddressSanitizer: global-buffer-overflow on address 0x55c7c36a4048 at pc 0x55c7c36a12da bp 0x7ffc2c602990 sp 0x7ffc2c602980
READ of size 4 at 0x55c7c36a4048 thread T0
    #0 0x55c7c36a12d9 in print_eleventh_el /home/user/example.c:6
    #1 0x55c7c36a130a in main /home/user/example.c:10
    #2 0x7f78b1781d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #3 0x7f78b1781e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #4 0x55c7c36a1184 in _start (/home/user/example+0x1184)

// ... more output follows but is omitted
```

## Enabling sanitizers in a simple C program

In your preferred editor or IDE, create the file `example.c`, copy the C program above into
the file, and save it.
On a Linux system, if using the GCC compiler, you can enable the ASan and UBSan sanitizers
by running the following commands to compile and link your code:

```bash
# compile example.c to example.o
$ gcc -g -fsanitize=undefined,address -c -o example.o example.c
# link example.o into an executable
$ gcc -fsanitize=undefined,address -o example example.o
```

The GCC option "`-fsanitize=undefined,address`" enables both UBSan and ASan in the `example`
executable. UBSan and ASan can be run together, but this is not possible with all
sanitizers. The sanitizers work by adding additional instructions to your code at compile
time, and if two sanitizers do so in incompatible ways, they cannot be used together.
An example of this is ASan and MSan: the two use incompatible sorts of instrumentation, and
cannot be used together.

The "`-g`" option instructs the compiler to add debugging information to the compiled code; this is
what allows the stack trace to include line numbers. Without it, your stack traces will be
more difficult to interpret.

If you are using a different platform and/or a different compiler, you should consult your
compiler documentation for information on how to enable the sanitizers.

Once you've compiled and linked the program with sanitizers enabled, running it should
produce the stack trace we saw before.

## Configuring the sanitizers

Typically, a sanitizer's default behaviour when encountering an error is to [display an error message
to standard error, and halt the program with a non-zero exit code][asan-usage]. Usually,
this default behaviour is sufficient for our needs, but sometimes we might want to conigure
different behaviour: for instance, we might want a [core dump][core-dump] to be generated when the
program exits (for use in debugging), or we might want the program *not* to halt but instead
attempt to continue, or we might want error messages sent to a different location.

[asan-usage]: https://clang.llvm.org/docs/AddressSanitizer.html#usage
[core-dump]: https://en.wikipedia.org/wiki/Core_dump

The behaviour of each sanitizer can be configured at runtime using an environment variable.
For instance, the AddressSanitizer is configured using the [environment variable
"`ASAN_OPTIONS`"][asan-opts]. On Linux, you can run your program as follows to find out what
configuration options your version of AddressSanitizer supports:

[asan-opts]: https://github.com/google/sanitizers/wiki/AddressSanitizerFlags#run-time-flags

```
ASAN_OPTIONS=help=1 /path/to/my-program
```

You can set multiple configuration options in the `ASAN_OPTIONS` variable by separating them
with colons. For instance, if we wanted to generate a core dump on program exit, and for
error messages to be sent to the file `my.log`, we would run our program as follows:

```
ASAN_OPTIONS=log_path=my.log:disable_coredump=0 /path/to/my-program
```

There are a set of common configuration options supported by all sanitizers, documented
[here](https://github.com/google/sanitizers/wiki/SanitizerCommonFlags), and
each sanitizer has a page on the [Google sanitizers
wiki](https://github.com/google/sanitizers/wiki) outlining any options specific to that
sanitizer -- for instance, the list of AddressSanitizer options is [here][asan-opts].

## Why not always enable the sanitizers?

Having seen how effective the sanitizers are at detecting bugs in C programs, you might ask,
Why not simply enable them all the time? The answer is that enabling the sanitizers does
come at a cost.

The extra instructions and data inserted by a sanitizer at compile-time mean
that your program will run more slowly and use more memory -- and if we are programming in
C, it's typically we because we want to make our program as time- and memory-efficient as
possible, so the use of sanitizers in production is at odds with that goal.
How much more slowly a program will run and how much
more memory will be used depends on the sanitizer: the AddressSanitizer,
for instance, makes programs run around [two times more slowly][asan-slowdown] and use
around [three times as much stack memory][asan-lims].

[asan-slowdown]: https://github.com/google/sanitizers/wiki/AddressSanitizer#introduction
[asan-lims]: https://clang.llvm.org/docs/AddressSanitizer.html#limitations

For each sanitizer, the routines for detecting bugs and printing stack traces are contained
in a library for that sanitizer which has to be linked into your program, and how it is
linked is often compiler-specific. The compiler may only support static linking of the
sanitizer (which will add extra size to your executable), or it may only support dynamic
linking (which means any machine you run your program on has to have a compatible copy of
the sanitizer libraries, and you will not be able to generate a [statically linked
executable][static-build]).

Finally, although the stack traces and error messages produced by the sanitizer as
(hopefully) informative to you, they may not be helpful to an end-user.

For all these reasons, we typically only enable the sanitizers while debugging and testing
our programs; when code is compiled for production, we typically prefer to produce
optimized, un-instrumented executables.

[static-build]: https://en.wikipedia.org/wiki/Static_build

## Other tips

Here are a few tips on developing C programs which will help you get the most value out of
the Google sanitizers:

**Enable compiler warnings**

:   All compilers can warn you about potential bugs in your program if invoked with the right options.
    If using GCC, consider always adding the options "`-Wall -Wextra -Wconversion`" to the
    command-line when compiling your code,
    which will alert you to a reasonably wide range of possible bugs. Combining these
    warnings with the use of the sanitizers will let you identify more bugs than either in
    isolation.

    To get even more warnings about potential bugs in your code, consider using a static
    analyser like [clang-tidy][clang-tidy].

[clang-tidy]: https://clang.llvm.org/extra/clang-tidy/

**Disable incompatible compiler protections**

:   Modern compilers often enable some security protections by default. For instance, GCC
    will automatically enable a form of [buffer overflow protection][gcc-stack-protector]
    for certain arrays in your C program.

    However, GCC's built-in protection isn't designed to produce helpful stack traces, so at
    least while debugging and testing your program, you are better off disabling it in
    favour of the Google sanitizers. You can do this by adding the `-fno-stack-protector`
    option when compiling your code.

[gcc-stack-protector]: https://en.wikipedia.org/wiki/Buffer_overflow_protection

**Write tests for your code, and run them frequently**

:   Although the Google sanitizers do an excellent job of detecting errors at runtime, they
    can't detect problems in code that is never actually executed. In order for the
    sanitizers to detect bugs in your code, you need to write tests that exercise your code
    thoroughly, and to run those tests frequently.

    You may find helpful [this introduction to writing tests for C code][c-test-lab], which
    uses the [Check][libcheck] testing framework. It also explains how to use the
    [Gcovr](https://gcovr.com/en/stable/) tool to generate a coverage report for your code,
    showing which code your tests executed, and which code is yet to be tested.

    [This blog post][memfault-test] by Tyler Hoffman of [Memfault][memfault] examines some
    more advanced testing techniques, including the use of [stubs, fakes, and mocks][mocks].

**Learn how to use the debugger**

:   The stack traces produced by the Google sanitizers provide a great deal of helpful
    information on what led to errors occurring in your program. But to quickly pinpoint the
    source of those errors, it's a good idea to become familiar with the [debugger][debugger]
    for your system and your compiler.

    Your editor or IDE may already offer a way of integrating with the debugger. If not,
    then on Linux, [CGDB][cgdb] offers a convenient way to debug C code, providing a
    "split-screen" terminal-based interface which shows both your code, and the current
    debugger prompt.

**Alternatives to the sanitizers**

:   Sometimes, it's either not possible or not convenient to compile and link your C code
    with the Google sanitizers. For instance, if you are writing a C extension library for
    another language ([such as Python][python-ext]), then enabling the sanitizers might mean
    re-compiling and linking the entire Python interpreter, which is inconvenient.

    In cases like that, the [Valgrind][valgrind] tool suite offers a useful alternative to
    the Google sanitizers, since it can be run directly on a compiled binary file without
    the need to recompile. A drawback is that Valgrind often runs much more slowly than the
    sanitizers would, and its feedback in case of an error may be more difficult to
    interpret.

[c-test-lab]: https://www.khoury.northeastern.edu/home/skotthe/classes/cs5600/fall/2015/labs/intro-check.html
[libcheck]: https://libcheck.github.io/check/
[memfault-test]: https://interrupt.memfault.com/blog/unit-testing-basics
[memfault]: https://memfault.com
[mocks]: https://en.wikipedia.org/wiki/Mock_object
[debugger]: https://en.wikipedia.org/wiki/Debugger
[cgdb]: https://cgdb.github.io/docs/cgdb.html
[python-ext]: https://docs.python.org/3/extending/extending.html
[valgrind]: https://valgrind.org/docs/manual/quick-start.html

<!-- vim: syntax=markdown tw=92
-->
