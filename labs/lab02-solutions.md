---
title: |
  CITS3007 lab 2 (week 3)&nbsp;--&nbsp;Debugging&nbsp;--&nbsp;solutions
---

For this lab, from within your VM, download the source code for the lab from the
`lab-02-code.zip` zip file. (You can do this by running, for instance,
`wget https://cits3007.arranstewart.io/labs/lab-02-code.zip` from within the VM.)
You can then unzip the file using the `unzip` command, and view individual files using
`less` or `vim`.

This lab shows how you can use GDB (the **G**NU **D**e**b**ugger) program to inspect a
running program. This is important for later labs, and for the unit project. The best way of
fixing bugs in your project code will be to use GDB to step through your code and pinpoint
the source of those bugs.
Often, you will also be able to access a debugger through your IDE or graphical
editor.[^ide-debuggers] However, it's worth learning how to use GDB directly, as in
practice, you won't always have access to an IDE or graphical editor (for instance, when
debugging programs running on cloud-based virtual machines).

[^ide-debuggers]: For instance, Eclipse and VS Code will
  provide a graphical interface to GDB.

## 1. GDB basics

GDB, the GNU Debugger, lets us step through compiled C (or C++)
programs and examine the values of variables in the running program.

When compiling programs we wish to debug, we need to pass the flag `-g`
to `gcc`, which tells it to add debugging information.
It can also be helpful to pass the `-O0` option to `gcc`, which tells the
compiler *not* to optimize the compiled code.[^optim]
If we try to execute a binary, and `gcc` has heavily optimized the
machine-code instructions emitted, then the
CPU instructions being executed may not correspond very closely to the
source code we provided, making the behaviour of GDB
unexpected.[^optim-but]

[^optim]: Passing flags like
  `-O1`, `-O2` and `-O3` to `gcc` tells it to spend longer compiling the
  code, in order apply increasingly advanced optimizations; see the
  documentation for `gcc`'s [optimization options][optim-options] for more
  details.

[optim-options]: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html

[^optim-but]: On the other hand, sometimes the behaviour we're
  trying to debug might *only* appear when optimizations are
  enabled. In such a case, we will likely have to debug
  our optimized binary, and simply accept that sometimes, the
  code being executed differs from what we see in the source file.

The Makefile for this lab already includes these two flags, so running `make factorial` in
your VM is all you need to do to compile the code.
(All commands from this point on in the lab are intended to be run from the command-line in
your VM, in the cloned `lab02` directory, unless otherwise specified.)

<div style="border: solid 2pt blue; background-color: hsla(241, 100%, 50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Compiling and Makefiles

:::

The code provided for this lab includes a Makefile intended to work with the [GNU
Make][gnu-make] program, which contains pre-written rules for compiling all the sample
programs in the lab.

[gnu-make]: https://www.gnu.org/software/make/

Amongst other things, it ensures that when GCC is compiling code, it uses the options
`-std=c11 -pedantic-errors -Wall -Wextra -Wconversion` (amongst others). When compiling code
for this unit, you should always include these options, at a minimum.[^min-gcc-flags]

[^min-gcc-flags]: The `-std=c11` option instructs the compiler to use the C11 standard.
  The [`-pedantic-errors` option][pedantic] instructs the compiler to disallow GCC-specific
  extensions to that standard, and to report an error if any of them are used. (By default,
  GCC tends to be fairly "generous", and attempts to compile programs that use extensions,
  even when we've specified we want to use the C11 standard.) The other options (`-Wall
  -Wextra -Wconversion`) enable warnings about problematic constructs in our source code.

[pedantic]: https://gcc.gnu.org/onlinedocs/gcc/gcc-command-options/options-to-request-or-suppress-warnings.html#cmdoption-pedantic-errors

Of course, it is possible to invoke GCC "by hand" to compile our code, but GNU Make

- will detect when particular source files need to be re-compiled, and re-compile
  *only* those source files (important for large projects, where there may be hundreds or
  even thousands of source files, and re-compiling them all would be slow)
- helps ensure that other developers using our code can invoke the compiler with *exactly*
  the options we intend.

We can even use GNU Make *without* having a Makefile present -- it has many "built-in" rules
about how to compile C and C++ programs, which mean that if we have a C source file
`my_program.c` present, we can use the following single command to compile it:

```bash
$ make CFLAGS="-std=c11 -pedantic-errors -Wall -Wextra -Wconversion" my_program.o my_program
```

Here, we've instructed Make to build the object file `my_program.o` and the executable
`my_program`, and we've specified compiler options that we want provided, but we've left it
up to Make's built-in rules to work out exactly what programs need
to be invoked. For small, single-file projects, Make's built-in rules are often all we need.

</div>



### 1.1. Factorial results

Read the API comments for the `factorial` function in
`factorial.c`, and build the `factorial` program with the
command `make factorial`.

Try executing the `factorial` program with various arguments
from 0 to 20 (the valid range) and outside it. Does the program
print the correct result? (If you're not sure what the factorial of some
number is, then Googling "factorial 10", for example, should give you
an answer.)

See if you can spot the cause of the error in `factorial.c`.
If you can, don't fix it yet -- we're going to use the program to
experiment with debugging using GDB.

### 1.2. Running GDB


Launch the debugger by running

```text
$ gdb ./factorial
```

You should see some welcome messages from GDB, then it will display
the debugger prompt `(gdb)`. As the welcome messages say, you can type
`help` at this prompt to get help, but the online help is unfortunately
not especially useful unless you already have some familiarity with
GDB. (If you *do* know the first letter of a command you're interested
in, then GDB has an "autocomplete" feature -- type `l` and then the
`tab` key a couple of times, to see commands beginning with `l`.)

Some of the commands you can run from the GDB prompt include:

- <code>list <em>[LINENUM]</em></code>, which lists the code around
  (before and after) <code><em>LINENUM</em></code>. If you leave off
  <code><em>LINENUM</em></code>, GDB will "page" it's way through
  the current file. You can type just `l` as shorthand for `list`.
- <code>run</code>, which runs the loaded program.

Try both of these commands. When you run the program, you should
see it print the error message

```text
Error: expected 1 command-line argument (an INT), but got 0
```

since by default, GDB runs the program with no command-line
arguments. (GDB should also print a message saying that our
program `exited with code 01`. By convention, programs on Unix-like
platforms exit with a non-zero code to indicate an error.)

Set the programs arguments by running the following command
(don't type the `(gdb)` prompt):

```text
(gdb) set args 6
```

and then running the program again.

Now, exit the debugger by typing `quit` or `ctrl-d`, and start it again.
This time, we'll use GDB's TUI (text-based user interface).

Type `ctrl-x` and then the `a` key immediately afterward. A "window"
should open in your terminal; run the `list` command, and you should
see something like this:

`<div style="display: flex; justify-content: center;">`{=html}
![](images/gdb_tui.png "gdb TUI")
`</div>`{=html}

The arrow keys and the `pageup` and `pagedown` keys on your keyboard
should now move you around in the source listing window, and `ctrl-i`
will refresh the display if at any point it seems to get out of sync
with what you're doing. (The `ctrl-x a` sequence toggles between GDBs normal
mode and TUI mode; hitting it repeatedly will take you back and forth
between them.)

The <code>breakpoint <em>LINENUM</em></code> command (`b` for short)
will set a breakpoint in the code (and the source listing will indicate
this with a "`b+`" in the code margin).

Run the command `b 26` to set a breakpoint at line 26 (containing the
statement `argc--`), and `r` to run the program.

GDB will highlight the line about to be executed. Some other useful
commands:

- <code>print <em>EXPRESSION</em></code> (`p` for short): print the
  value of a C variable or expression. `p argv` will print the type
  (`char **`) and value of `argv`, one of the arguments to main -- the
  value should be a location in virtual memory, something like
  `0x7fffffffe468`.

  You can print elements within a struct or array using the normal
  C syntax for accessing those elements -- for instance, `print argv[0]`
  will (unsurprisingly) print the value of `argv[0]`.

  You aren't limited to variables -- try printing the value of `3 * 2 + 1`
  or `"foo"[0]` and see what gets displayed.

- <code>ptype <em>EXPRESSION</em></code>: print the *type* of a C
  variable or expression.

- `step` (`s` for short): step "into" the current instruction. That is,
  if the current instruction is a function call, GDB will go "into"
  that function and start stepping through its instructions. (Note that
  if the function you're trying to step into was, say, a library
  function compiled without debugger symbols, then typing `s` will
  result in you stepping "into" assembly language routines -- see the box
  entitled "[s]tep vs [n]ext" for more details.)

- `next` (`n` for short): step "over" the current instruction; if it
  is a function call, the function will be executed without GDB going
  "into" that function and stepping through it in detail.

- `finish`: Step "out" of the current function (run to the end).

- `continue` (`c` for short): continue running until a breakpoint is
  hit.

- `kill` (`k` for short): abort execution of the program, but don't exit
  GDB.

- `info args`: print the arguments of the function you're in.

- `info locals`: print the local variables of the function or block
  you're in.

- `clear`: remove all breakpoints.

- `backtrace` (`bt` for short): print information on the stack frames
  currently on the call stack -- i.e., what "chain" of functions called the
  function you're currently in, and with what arguments.

For some additional commands and advanced features, see
the [Hitchikers Guide To The GDB][hitch-gdb] and the
GDB tutorial series [here][gdb-tutorial] and [here][gdb-tutorial-2] from RedHat.
GDB "cheat sheets" are available [here][gdb-cheat] (PDF)
and
[here][gdb-cheat-2].

[hitch-gdb]: https://apoorvaj.io/hitchhikers-guide-to-the-gdb
[gdb-tutorial]: https://developers.redhat.com/blog/2021/04/30/the-gdb-developers-gnu-debugger-tutorial-part-1-getting-started-with-the-debugger
[gdb-tutorial-2]: https://developers.redhat.com/articles/2022/01/10/gdb-developers-gnu-debugger-tutorial-part-2-all-about-debuginfo
[gdb-cheat]: https://darkdust.net/files/GDB Cheat Sheet.pdf
[gdb-cheat-2]: https://gist.github.com/rkubik/b96c23bd8ed58333de37f2b8cd052c30

<div style="border: solid 2pt blue; background-color: hsla(241, 100%, 50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Dynamic printf (`dprintf`) -- no more stray `printf`s!

:::

A common method of debugging C programs is to add `printf()` invocations at various
points in the program to show what the value program variables take on at different
times. A disadvantage of this approach is that it requires you to re-compile your program,
and you must remember to remove the calls to `printf()` from your final code.

However, GDB will let you add `printf()` invocations without recompiling the program
using the [`dprintf` (dynamic printf)][dprintf] command.

<details>

<summary><span class="only-open">
...click for more
</span></summary>

Issuing the <code>dprintf <em>LINENUM</em>,
<em>FORMAT-STRING</em>, <em>EXPRESSION</em></code> command has the effect of adding a breakpoint at
<code><em>LINENUM</em></code>, as well as inserting a call to `printf` which prints the specified
expression using a specified printf-style format string.

So, for example, the command `dprintf myprogram.c:8, "Num elements: %d\n", n` would allow
you to insert `printf` calls that nicely display the value of the variable `n` at line 8 of a
program.

If you're interested in using the `dprintf` command, you can find a tutorial on how to use
it [here][dprintf-tut].

[dprintf]: https://sourceware.org/gdb/current/onlinedocs/gdb#Dynamic-Printf
[dprintf-tut]: https://abstractexpr.com/2024/03/03/dynamic-printf-debugging-with-gdb/

</details>

</div>


### 1.3. `argc` and `argv`

The first thing the `factorial` program does in `main` is execute the
following statements:

```C
argc--;
argv++;
```

If you're running an instance of the `factorial` program, kill it
with `k`, use `set args 6` to set the command-line arguments of the
program, and run it with `r`. (Your breakpoint at line 26 should still
be showing; execute the command `b 26` to set if you've accidentally
exited GDB and come back in.)

Step through the program, examining the values of `argc`, `argv`,
and elements of `argv` (like `argv[0]` and `argv[1]`) at various points
in the program.

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,
50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

[s]tep vs [n]ext

:::

In general, when you're stepping through code, the command you
want to use is "n" ("next"), which steps *over* function calls
when it encounters them.

When you encounter a call to a function you've defined elsewhere
in the program, and want to step "into" that function, then "s" ("step")
is the command to use.

If you try and invoke "s" on a function that is part of the
C runtime, however, like `strtol`, then GDB will print an error
something like this:

```
  (gdb) s
  __strtol (nptr=0x7fffffffe730 "6", endptr=0x7fffffffe3a0, base=10) at ../stdlib/strtol.c:105
  ../stdlib/strtol.c: No such file or directory.
```

Here, GDB is telling you that it *can't* "step into" the code for
`strtol`, because it can't find the original source code for that
function,
nor can it find any "debugging symbols" for it.
(To save disk space, C runtime
libraries are normally shipped without either of those – though it is
possible to install them if you wish.)

<details>

<summary><span class="only-open">
...click for more
</span></summary>

A quick fix is to type `f` for `finish`, which will finish running the current function, and so should get you
back to the C code the function was called from.

You'll get a similar error if you try to "step into" the `errno`
variable. `errno` isn't a library *function*, but  is a global symbol defined
in the C runtime, and thus causes the same sort of error messages if you try to step "into" it.

The takeaway here is: usually, you can only "step into" functions that
*you've* defined, and only when you compiled your code using the "`-g`" option
which causes `gcc` to include debugging symbols.

</details>

</div>


What is the effect of the two statements we listed above? Why would we
use them?



<div class="solutions">

::: block-caption

Sample solutions

:::

The statements are used to "ignore" the value of `argv[0]`, which
contains the filename of the program being executed. `argc` is
decremented by one (so the number of arguments is reduced by one),
and `argv` is incremented by one -- the effect is that
`argv[0]` *after* the statements are executed points to what
used to be `argv[1]`.

This is commonly done in programs where we are interested in the
command-line arguments of the program, but have no especial interest
in the name of the executable.

</div>



### 1.4. `strtol`

In the file `factorial.c`, we use the function `strtol` to convert the
program's first command-line argument into a `long`, despite the
fact that the `factorial` function only takes an `int`, and we then
cast the `long` into an `int`.

However, C11 has a function `atoi`, which converts strings to `int`s, so
it seems we could have used that.
Read the documentation for

- `strtol` (`man strtol` in your VM, or
<https://en.cppreference.com/w/c/string/byte/strtol>), and
- `atoi` (`man atoi` in your VM, or
<https://en.cppreference.com/w/c/string/byte/atoi>)

and summarize
what the differences are. Why might we prefer `strtol` over `atoi`?



<div class="solutions">

::: block-caption

Sample solutions

:::

`strtol` provides more information and allows for more precise
error-checking than `atoi`. When `atoi` can't convert the string it
sees, it returns the value `0`: this is unhelpful, since it means we
have no way of knowing whether it encountered a conversion error, or was
actually supplied with the string `"0"`.

`atoi` also has unhelpful behaviour when the number encountered would
fall outside the bounds of an `int` (i.e. is too large in magnitude,
whether positive or negative): its behaviour is simply undefined.
`strtol` in contrast is more helpful: it sets the global value `errno`
(which you can read about [here][errno]) to the value `ERANGE` to
indicate an "out of range" error, and still returns *some* potentially
useful value (either the highest or lowest value a `long` can hold).

[errno]: https://en.cppreference.com/w/c/error/errno

Because of this, many C style guides recommend that `strtol` be
used instead of `atoi`.

</div>



### 1.5. Diagnosing and fixing the `factorial` bug

Kill the `factorial` program, set a breakpoint somewhere in the
`factorial` function (e.g. line 18), and use the `run` and/or
`continue` commands to get to your breakpoint.

Step through execution of the `factorial` function, and examine
the values of the local variables (using either `print` or `info
locals`). What is the bug in `factorial`? Fix it.



<div class="solutions">

::: block-caption

Sample solutions

:::

The fix is that the line

```
for (int i = n; i >= 0; i--) {
```

should be changed to

```
for (int i = n; i > 0; i--) {
```

In other words, this is an ["off-by-one" error][off-by-one].

[off-by-one]: https://en.wikipedia.org/wiki/Off-by-one_error

</div>



<div style="border: solid 2pt blue; background-color: hsla(241, 100%,
50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Recommendation -- keep lab notes

:::

It's recommended you keep online notes of useful commands, coding best practices, useful
links etc.  you come across in the unit, as a reminder to yourself of what we've covered.
You could keep a Word or text document, if you like, using [Google
Docs](https://docs.google.com/), but another option is to store your notes in a "Gist" -- a
single text file versioned by GitHub.

If you have a GitHub account and are logged in, then click on the "+" symbol in the top
right of any GitHub page, and select "New gist". Give your gist a description (e.g. "My
CITS3007 notes") and a filename (e.g. "notes.md"). Then click "Create secret gist" (or
"public", if you wish to make it public).

Gists support formatting your file using
[Markdown](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax) --
for instance, use asterisks ("`*`") to surround words intended to be
italic, and start paragraphs which should be part of a list with a
hyphen and space ("`<code>- </code>`{=html}"). Clicking the "Preview" tab will show you
what your notes look like converted to HTML.

</div>


## 2. Segmentation faults

Compile the `segfault` program by running `make segfault` and then run
it with `./segfault`. The intended behaviour is that it should accept a
line of input from the user, and echo this back.

However, when it is run and some text entered, it produces a
[*segmentation fault*][segfault]. A
segmentation fault is caused when the CPU detects that a program has
attempted to access memory which it is not permitted to access.

[segfault]: https://en.wikipedia.org/wiki/Segmentation_fault

Try running the program using GDB. (Hint: you can get GDB to
start in TUI mode by running `gdb -tui ./segfault`.) Start GDB
and run the program with the `run` command, and enter some text.
Once the segfault occurs,
run the `backtrace` command to see the current stack trace.

You should see something like

```
#1  0x00007ffff7e2a96c in __GI__IO_getline (fp=fp@entry=0x7ffff7f93980 <_IO_2_1_stdin_>, buf=buf@entry=0x0,
    n=n@entry=1023, delim=delim@entry=10, extract_delim=extract_delim@entry=1) at iogetline.c:34
#2  0x00007ffff7e296ca in _IO_fgets (buf=0x0, n=1024, fp=0x7ffff7f93980 <_IO_2_1_stdin_>) at iofgets.c:53
#3  0x0000555555555209 in main () at segfault.c:9
```

Each stack frame shows the values of the arguments to the function
called for that frame. Do any of them look suspicious?



<div class="solutions">

::: block-caption

Sample solutions

:::

The argument `buf` (in stack frames #1 and #2) is equal to 0 -- that is,
the `NULL` pointer. Trying to dereference the `NULL` pointer in C -- on
platforms that have hardware memory protection -- will generally result in a
segmentation fault.

However, not all platforms *do* implement memory protection, so this is not
guaranteed.

</div>



Try printing the value of `buf` (with the command `print buf`) before
and after it
has been allocated, and see what result you get.



<div class="solutions">

::: block-caption

Sample solutions

:::

After the allocation, `buf` will be set to 0 (the `NULL` pointer),
which is not what we want -- we expect it should point to
newly-allocated memory.

</div>



Try using the `print` command to see how the "bitwise left shift"
("`<<`") operator works.

Try `p 1 << 2`, `p 1 << 10`, and a few other values, then try `p 1 <<
31`. What result do you get? Why might this occur? (Hint: read the
cppreference.com page on arithmetic operators, in particular the section
on "overflow":
<https://en.cppreference.com/w/cpp/language/operator_arithmetic>.
Also try the `ptype` command for the various values you typed above, to
see what their type is.) How can the program be fixed?





<div class="solutions">

::: block-caption

Sample solutions

:::

The behaviour occurs because the type of `1 << 32` is type `int`,
which is a signed integer type, and only has 4 bytes (32 bits) on the
platform we're using. The left shift operator `<<` has the effect of
multiplying a number by two $n$ times (where $n$ is the right-hand
operand). For an `int`, multiplying by two 32 times leads to overflow,
and the `int` "wraps around" to a negative number.

The immediate problem with the program can be fixed by:

- choosing a much smaller buffer (say, `1 << 16`, or `65535`)
- putting a lowercase "`l`" after the `1` (i.e. `1l << 32`) -- this
  makes it a `long`, which on our platform can store 8 bytes, or 64
  bits)

</div>



## 3. C refresher no. 2

On [Moodle][moodle], you will find an unassessed quiz entitled "C refresher no.
2". It's recommended you complete this (either now, or in your own
time) to check your knowledge of C control flow structures and
data types.

[moodle]: https://quiz.jinhong.org/course/view.php?id=19

<div style="margin: 2rem 0; line-height: 2;">&nbsp;</div>





<!--
  vim: syntax=markdown tw=92 smartindent :
-->
