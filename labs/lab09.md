---
title:  CITS3007 lab 9 (week 10)&nbsp;--&nbsp;Testing
---

This lab explores the role of testing in secure software development.

## 1. Code and tools

A header file you should use for the CITS3007 project is provided
on the CITS3007 website, at

- <https://cits3007.arranstewart.io/assessment>

We will be using the [Doxygen][doxygen] documentation tool. Install it in your VM
with

```
$ sudo apt-get update
$ sudo apt-get install --no-install-recommends doxygen graphviz
```

We will also make use of the [Check][libcheck] unit testing framework.
Install it with

```
$ sudo apt-get install check
```

[libcheck]: https://libcheck.github.io/check/index.html

<!--
git clone arran@barkley.arranstewart.info:/mnt/data2/dev-06-teaching/cits3007-sec-cod/workdir-2022/project-test/project_skeleton
alias gs="git status"
-->

## 2. Testing, documentation and APIs

The aim of testing is to identify and remove
*defects* from a project --
mistakes in the source code or configuration/data files that cause it
to deviate from its prescribed behaviour.[^defect]
*Vulnerabilities* are a particular class of defects where the
resulting failure compromises security goals for
a system.

[^defect]: We use the terms "defect" and "failure" generally
  in line with their definitions in ISO/IEC/IEEE standard
  24765("Systems and software engineering -- Vocabulary").
  A *failure* is deviation of the behaviour of a system from its
  specification, and a *defect* is an error or fault in the
  static artifacts (software code, configuration or data files,
  or hardware) of a system which, if uncorrected,
  can give rise to a failure.

To be able to test a program, or part of it, we have to know what
its intended behaviour *is*, or by definition we can't test it.
[*Documentation*][software-doc] is therefore an important part of any
software project.
The documentation for a function or other piece of code tells us what it
*should* do,
and testing tries to find situations in which the code does something
else.

[software-doc]: https://en.wikipedia.org/wiki/Software_documentation

A program specification defines the behaviour expected of an
entire program, and can be used directly for testing that program.
However, it doesn't say anything about the behaviour of individual
functions. Those are normally documented within the source file
that contains them, and the documentation for all public-facing
functions, macros and data structures forms the [API][api]
("Application Programming Interface") for that file.[^c-api-docs]

[api]: https://en.wikipedia.org/wiki/API

[^c-api-docs]: In some languages, like Java and Rust,
  the **implementations** of datatypes, functions or methods are located
  in the same place as their **specification**. Individual items
  can usually be declared *public* or *private*. \
  &nbsp;&nbsp;  In other languages, like C and C++, the implementations
  are in a different file (the `.c` or `.cpp`) file to the
  specifications (which appear in a header file, with
  extension `.h` or `.hpp`). \
  &nbsp;&nbsp; And some other languages yet are a sort of combination,
  like Ada and Haskell. In those languages, the implementation appears
  in the body of the file, and a specification near the top in a module
  or package "header". \
  &nbsp;&nbsp; Best practice in C is to document the *public* parts
  of a `.c` file in the header file, to keep the implementations in
  the `.c` file, and for everything that isn't intended to be public
  to be made `static` (private). \
  &nbsp;&nbsp; However, to keep things simple in this lab, we
  will put everything -- public and private parts -- into a single
  `.c` file.

### 2.1. Documenting an API

Typically, the specification documentation for functions
is contained in documentation *blocks*: specially formatted
comments or annotations which can be extracted and displayed by
documentation tools. For example, if you look at the `crypto.h`
header file, you will see functions have documentation blocks like this
one:

```c
/** Decrypt a given ciphertext using the Caesar cipher, using a specified key, where the
  * characters to decrypt fall within a given range (and all other characters are copied
  * over unchanged).
  *
  * Calling `caesar_decrypt` with some key $n$ is exactly equivalent to calling
  * `caesar_encrypt` with the key $-n$.
  *
  * \param range_low A character representing the lower bound of the character range to be
  *           encrypted
  * \param range_high A character representing the upper bound of the character range
  * \param key The encryption key
  * \param cipher_text A null-terminated string containing the ciphertext to be decrypted
  * \param plain_text A pointer to a buffer where the decrypted text will be stored. The
  *           buffer must be large enough to hold a C string of the same length as
  *           cipher_text (including the terminating null character).
  *
  * \pre `cipher_text` must be a valid null-terminated C string
  * \pre `plain_text` must point to a buffer of identical length to `cipher_text`
  * \pre `range_high` must be strictly greater than `range_low`.
  * \pre `key` must fall within range from 0 to `(range_high - range_low)`, inclusive.
  */
void caesar_decrypt(char range_low, char range_high, int key, const char * cipher_text, char * plain_text);
```

<!--
 *k
-->

Documentation blocks normally have some way of formatting
the documentation for easy reading, of documenting particular
parts of a function (like parameters or the return value),
and of referring to other, related functions.
In this lab, we will use the [Doxygen][doxygen] tool,
which is expressly designed for extracting API documentation from
C and C++ files. It uses [Markdown][markdown] conventions
for formatting, and special tags (like `\param`, `\return`
and `\ref`) to pick out particular portions of a function -- these are
described in the Doxygen tool's [documentation][doxy-tags].
You likely have encountered similar tools to Doxygen
previously for
other languages: Java uses the [`javadoc`][javadoc] tool,
Python projects typically use [`pydoc`][pydoc] or [`sphinx`][sphinx],
Rust uses [`rustdoc`][rustdoc], and Haskell uses [`haddock`][haddock].

[markdown]: https://daringfireball.net/projects/markdown/syntax
[doxy-tags]: https://doxygen.nl/manual/commands.html
[doxygen]: https://doxygen.nl
[javadoc]: https://www.oracle.com/au/technical-resources/articles/java/javadoc-tool.html
[pydoc]: https://docs.python.org/3/library/pydoc.html
[sphinx]: https://www.sphinx-doc.org/
[rustdoc]: https://doc.rust-lang.org/rustdoc/what-is-rustdoc.html
[haddock]: https://haskell-haddock.readthedocs.io/en/latest/

Note that documentation blocks do *not* serve the same purpose
as inline comments (comments contained within the body of a function).
(In fact, in some languages, documentation blocks may not be
comments at all. Python uses [strings instead of comments][docstring],
and Rust internally uses the [`#[doc]` annotation][rust-annot].)
Documentation blocks should *always* be included for any function that forms part of an API,
so that other programmers know how to *use* that function, and documentation blocks can
be as extensive as needed.
If you are using a C library -- say, the FLAC library, which
allows you to encode, decode and manipulate audio files in the FLAC
format -- then your primary way of knowing what the functions
in that library do is by referring to the [API documentation][flac-api].
Not only do you not need to know what any inline comments say,
but for commercial software libraries, you might not have any access
to them or to the source code at all.[^commercial-example]

[docstring]: https://peps.python.org/pep-0257/
[rust-annot]: https://doc.rust-lang.org/rustdoc/write-documentation/the-doc-attribute.html
[flac-api]: https://xiph.org/flac/api/group__flac.html

[^commercial-example]: For
  an example of such a C library, see the Intel [IPP
  multimedia library][ipp]. Although the library is
  free for use, the source code is properietary
  and not available.

[ipp]: https://www.intel.com/content/www/us/en/developer/tools/oneapi/ipp.html

In contrast to documentation blocks,
*inline* comments are *only* for the use of programmers who need
to fix or enhance existing functions, and typically should be
used sparingly -- excessive inline commenting makes code harder
to read. In general, inline comments should not say *what*
the code is doing -- anyone who understands the programming language
should
be able to see that -- but rather *why* it is doing it.

<div style="border: solid 2pt orange; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

::: block-caption

Inline commenting of your code

:::

Do not over-comment your code. Excessive inline comments make your
code harder to read, and will result in a lower mark for code clarity
and style.

In general, you should assume that the person marking your code knows
how to program in C, and does not need to have it explained to them.

</div>

### 2.2. Running `doxygen`

Change directory to your directory containing the `crypto.h` file,
and run `doxygen -g`. It generates a file called `Doxyfile`, used to
configure the exact contents and formatting of the API documentation for
a project. To work well with a C project, a few changes are needed --
you can download a Doxyfile with these changes in the code .zip file for
this lab.

Download and extract the `Doxyfile` from that zip code, and run the
command `doxygen`.
The Doxygen tool will use the configuration contained
in the `Doxyfile` configuration file to generate HTML documentation
contained in the `docs/html` subdirectory.

<!--
You can see a copy of this documentation on the CITS3007 website 
[here](https://cits3007.arranstewart.io/assignments/docs/html).
-->

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Doxygen features

:::

Doxygen has a number of features we will not use, but which are very
useful for exploring the structure of large codebases. It can create
UML-style diagrams of the classes or functions contained in the code, as
well as showing graphically which functions or methods make use of
others. You can read more about those features [here][doxy-diags].

[doxy-diags]: https://www.doxygen.nl/manual/diagrams.html 

</div>


If you're unfamiliar with good practices for writing API documentation,
take a look at

- the [code documentation guidelines][lsst-coding] for data management at the
  [Vera C. Rubin Observatory][rubin-obs] 
  <!-- in Chile, which houses the [Large Synoptic Survey Telescope][lsst] -->
- the [coding and documentation style][cmu-style]
  for Carnegie Mellon University course 15-410/605, "Operating System
  Design and Implementation", or
- the Oracle [writing Javadoc documentation][javadoc-guide] guide --
  although written for the Java language rather than C, nearly
  all the general principles discussed here still apply.

[lsst-coding]: https://developer.lsst.io/cpp/api-docs.html
[rubin-obs]: https://www.lsst.org
[lsst]: https://en.wikipedia.org/wiki/Vera_C._Rubin_Observatory
[cmu-style]: https://www.cs.cmu.edu/~410/doc/doxygen.html
[javadoc-guide]: https://web.archive.org/web/20190426200914/http://www.oracle.com/technetwork/java/javase/documentation/index-137868.html

### 2.3. Writing your project code

The .zip file for this lab also contains a Makefile you may find useful for the project. A
good way to start is to write a `crypto.c` file which initially, just contains empty
"skeleton" definitions for the functions you need -- it should also contain a line to
`#include "crypto.h"`. You should end up with something like this:

```c
#include "crypto.h"

void caesar_encrypt(char range_low, char range_high, int key, const char * plain_text, char * cipher_text) {
}

void caesar_decrypt(char range_low, char range_high, int key, const char * cipher_text, char * plain_text) {

}

void vigenere_encrypt(char range_low, char range_high, const char *key,
                      const char *plain_text, char *cipher_text
) {

}

void vigenere_decrypt(char range_low, char range_high, const char * key, const char *
cipher_text, char * plain_text) {

}

int cli(int argc, char ** argv) {
  return 0;
}
```

Once you've started your `crypto.c` file, you can compile your skeleton code by `cd`-ing to
your code directory and running `make all`.  The output you see from `make` will be quite
"noisy", because our empty file implementations have a lot of unused parameters in them.

The following command filters out those unused parameter warnings and
makes the output easier to follow:

```
$ make CFLAGS="-Wno-unused-parameter" clean all
```

Take a look at the Makefile. The `CFLAGS` variable specifies what arguments to supply to the
compiler, and `CC` specifies what compiler to use. You can alter these in the Makefile
itself, or from the command line as we did just now. By default, the Makefile compiles your
project with no optimization, and no sanitizers. When testing your project, you'll need to
compile your code with a *range* of optimizations and sanitizers (as well as making use of
the static analysers we have looked at).

### 2.4. Writing and running tests

Once a specification is available for a function (or even before then!), it's possible to
start writing tests for it. A test is meant to look at the behaviour of a system or function
in response to some input, and make sure that it aligns with what we expect.

It can be useful to think of a test as being composed of three parts

1. **Arrange**
2. **Act**
3. **Assert**

In C, we often also need to add a fourth part, "**Cleanup**".
Some languages have [automatic memory and resource management][gc] --
when open files or allocated memory are no longer in use, they
are "garbage collected" -- but C is not one of these. In C, it's up to
the programmer to ensure they dispose of resources after use (for
instance by `free()`-ing allocated memory and closing open files).

[gc]: https://en.wikipedia.org/wiki/Garbage_collection_(computer_science)

**Arrange** 

:   means preparing whatever resources are required for our test. This
    could include initializing data structures needed, creating
    and populating files or database, or starting programs running
    (say, a webserver).

**Act**

:   means invoking the behavior we want to test. In C, this will
    typically mean calling a function, which we call the *function
    under test*.

**Assert**

:   means to look at the resulting state of the system and see if it is
    what we expected. If a function returns a value, it might mean
    checking to make sure that value is the correct one.
    If the function instead writes data to a file or database, it might
    mean examining the file or database to see whether
    the changes made are the ones we expected. Sometimes **assert**ing
    just requires comparing two values, but other times
    we might need to make a more thorough investigation.


In C, **cleanup** means to dispose of any used resources, and to
make sure the test we've just run won't interfere with the results
of any future tests.

C is a particularly challenging language to write tests for,
because a misbehaving function under test can overwrite the stack frame
of the function that's calling it, meaning we can no longer rely
on the results of our test.

It's a good idea, therefore, to enable any dynamic checks we can
that will help us catch misbehaviour like this -- for instance, by using
the [Google sanitizers][sanitizers] or tools like [Valgrind][valgrind].
Additionally, the [Check][libcheck] unit-testing framework, which we use
in this lab,
by default uses the `fork()` system call to run tests in a separate
address space from the test framework, which prevents the framework from
being affected by any memory corruption that occurs.

[sanitizers]: https://github.com/google/sanitizers
[valgrind]: https://valgrind.org

If you run `make test`, the Makefile will

- compile your code (if not already done)
- use the [`checkmk`][checkmk] tool to generate "`.c`" files from the
  tests contained in `check_crypto.ts`, and
- compile and run the tests.

[checkmk]: https://manpages.ubuntu.com/manpages/focal/man1/checkmk.1.html

The use of `checkmk` isn't necessary -- we could write the tests
by hand in C if we wanted -- but it saves us having to write some
repetitive boilerplate code.

Try running

```
$ make test
```

to see the Check framework in action. You should see that several tests were run, that one
test (`arithmetic_testcase:arithmetic_works`) passed, and that the others failed.

Check can output results in [multiple formats][test-output].  You might find the output of
the following command slightly more readable:

```
$ make check_crypto && CK_TAP_LOG_FILE_NAME=- prove --verbose ./check_crypto
```

Here, `make check_crypto` builds our test-runner program. `CK_TAP_LOG_FILE_NAME=-`
tells it to output results using the ["TAP" format][tap] for test
results, and `prove` is a Perl program which formats those results
and summarizes them (see [`man prove`][prove] for details).
Leaving off the "`--verbose`" flag to `prove` will result in just
a summary being printed.

[test-output]: https://libcheck.github.io/check/doc/check_html/check_4.html#Test-Logging
[tap]: https://testanything.org
[prove]: https://linux.die.net/man/1/prove

One of the tests contains an *intentional* failure -- you can use it
to see one of the Google sanitizers in action. It is just below the following code in the
`.ts` file:

```bash
#tcase deliberately_bad_testcase

#test go_out_of_bounds
```

Try and adjust the Makefile so that your code is compiled and run with the UBSan and ASan
sanitizers (look at previous labs for hints on how to do so).
Then run `make test` again. The `strlen()` function
in that test goes out of bounds due to an unterminated string,
but the bad behaviour is detected by the AddressSanitizer.
If the test *doesn't* fail, that means you haven't got the AddressSanitizer properly working
-- discuss with lab teaching staff how to do so.

It's a good idea to enable the sanitizers while developing your project. If they detect a
memory errors, you may
need to debug your program using `gdb`. The output of
AddressSanitizer includes a stack trace which reveals where
the bug was detected. (Include the "-g" option to gcc for better information). If running

```
$ gdb -tui ./check_crypto
```

to track down what causes a bug, you probably will want to set the
environment variable `CK_FORK` to "`no`", like this:

```
$ CK_FORK=no gdb -tui ./check_crypto
```

This inhibits Check's usual behaviour of `fork`ing off a separate
process in which to run each test.

### 2.5. Project work

Although your project should be completed on your own, discussing with other students and
the lab facilitators how you might *test* your code is completely fine, and encouraged.
Besides the tests contained in the .ts file, what additional tests will you need?
How will you ensure your test expectations are correct? (Hint: there are a number of
websites that implement Caesar and Vigenere encryption -- you can make use of them for
devising your own tests.

<!--

### 2.6. "Mock" objects

It's often a good idea to make our tests fast, so they're quick and easy
to run. If tests take too long to run, developers will avoid running
them, which leads to poor quality code. When writing *unit* tests --
tests which examine the behaviour of a very smal part of the system --
it's also often a good idea to minimize our tests' dependencies on
external resources, like files and database.

For that reason, it can be useful to use "mock" resources, which "stand
in" for a real resource, but are quicker to create. 
The Check framework does not provide any special support for creating
mocks, but we can use some of the Linux kernel's features to
build our own.
On Linux, the [`memfd_create`][memfd] system call creates a "file"-like
object which exists purely in memory. Accessing such files is faster
than making accesses to disk.
In the skeleton code for the CITS3007 project, the
`mock_file()` function in `tests/test_utils.c` uses `memfd_create`
to create an in-memory file with any desired content.
(The protopye and documentation are in tests/test_utils.h`).

[memfd]: https://man7.org/linux/man-pages/man2/memfd_create.2.html

Other unit testing frameworks provide more extensive support
for mock objects. For instance, the [cmocka][cmocka] test framework
allows for *functions* to be mocked: the framework allows
a C function to be "swapped out", and replaced by a "mock" function
generated by the framework. The [LWN.net][lwn] site has an article on
using mock objects in C, at <https://lwn.net/Articles/558106/>.
We don't cover the cmocka framework in detail in this unit,
but provide it as a resource you may find useful when tackling the
project.

[cmocka]: https://cmocka.org  
[lwn]: https://lwn.net/

### 2.7. Other tests

The tests provided in the skeleton Curdle code
do not test the `crypto()` function itself.
The specification for that function says that it uses a particular
file, "` /var/lib/curdle/scores`", to store scores in, that it should
be run as part of a setuid program, and that the `curdle` user
should be owner of the program executable.

How might we test these requirements?

-->

<!-- vim: syntax=markdown tw=92
-->
