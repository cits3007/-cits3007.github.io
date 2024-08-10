---
title:  CITS3007 lab 7 (week 8)&nbsp;--&nbsp;Injection&nbsp;--&nbsp;solutions
---

`~\vspace{-5em}`{=latex}

## 0. Introduction

The aim of this lab is to expose you to how C programs interact with the process environment
(a set of environment variables). We'll see how we can invoke other programs with a specific
environment, and how the variables defined in the environment can alter the behaviour of our
programs (sometimes in unexpected ways).

## 1. Environment variables

Every process has access to a set of *environment variables*. In C, they
are represented as the variable `char **environ` (see `man 7 environ`
for additional details): this variable allows us to read, write, and and
delete environment variables.

Let's see how this pointer-to-pointer-to-char can be used to display the current values of
environment variables. Save the following program as `print_env.c`, and compile it with `make
CFLAGS="-std=c11 -pedantic-errors -Wall -Wextra -Wconversion" print_env.o print_env`.

```C
#include <stdio.h>
#include <stdlib.h>

extern char **environ;

void printenv() {
  for(size_t i=0; environ[i] != NULL; i++) {
    printf("%s\n", environ[i]);
  }
}

int main(void) {
  printenv();
}
```

The `environ` variables represents a "list" of `char *` C strings, and as the man page for
the `environ` variable explains, the end of the list is indicated by a `char *` which is set
to `NULL`.

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Environment variables versus shell variables

:::


We can manipulate environment variables interactively, if we are using a [Unix
shell][shell-wiki] or a programming language with an [interactive top-level][wiki-repl]
(such as Python).

[shell-wiki]: https://en.wikipedia.org/wiki/Unix_shell
[wiki-repl]: https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop

In C or Python, we are unlikely to confuse environment variables with local variables from
the language we're working in. In C, environment variables are represented by the array of
strings `environ`, and in Python, they're represented by a "dictionary"-like structure,
[`os.environ`][py-environ], which we can use as in the following example:

```bash
$ python3
Python 3.8.10 (default, Mar 15 2022, 12:22:08)
[GCC 9.4.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import os
>>> path = os.environ["PATH"]
>>> print(path)
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
```

[py-environ]: https://docs.python.org/3/library/os.html#os.environ

C and Python's ways of accessing the process environment are clearly very different to how
we define and use local variables in those languages.

In Bash (and other Unix shells), however, there's just a single syntax for assigning values
to variables:

```
$ myvar=myval
```

and `myvar` *could* be a "Bash" variable (only accessible within our current shell session,
and not passed to child processes), or it *could* be an environment variable (part of the
process environment maintained by the kernel, which will be
passed to child processes) -- the same syntax is used for both. We can convert a normal
variable into an environment variable using the built-in [`export` command][bash-export]:

[bash-export]: https://www.gnu.org/software/bash/manual/bash.html#index-export

```bash
$ export somevar
```

and can turn an environment variable back into a normal variable with

```bash
$ export -n somevar
```

Bash keeps track of which variables are environment variables, and which are not.

Try the following:

```
$ myvar=myval
$ echo my var is $myvar
$ sh -c 'echo my var is $myvar'
```

Only the first `echo` command prints the expected contents of `myvar`.
The second time around, we are spawning a new shell process, and within
that process, the variable `myvar` has not been defined.

Let's try again, this time marking `myvar` as an environment variable (so it will be
inherited by child processes):

```
$ myvar=myval
$ echo my var is $myvar
$ export myvar
$ sh -c 'echo my var is $myvar'
```

This time, we should see the expected contents of `myvar` echoed twice.
We can also define and export a variable in a single step:

```
$ export myvar=myval
```

Besides the builtin Bash "`echo`" command, the "`declare`" command can
be useful for displaying variable contents as well. "`declare -p myvar`" means
to display the definition of `myvar` (note that we do *not* put a dollar
sign in front of `myvar` this time -- "`declare -p`" needs the *name* of
a variable, not its value).

```
$ declare -p myvar
declare -x myvar="myval"
```

</div>

### 1.3. Environment variables and `fork`

Save the following program as `child_env.c`, and compile it with
`make CFLAGS="-std=c11 -pedantic-errors -Wall -Wextra -Wconversion"  child_env.o child_env`.

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

extern char **environ;

void printenv() {
  for(size_t i=0; environ[i] != NULL; i++) {
    printf("%s\n", environ[i]);
  }
}

int main() {
  pid_t childPid;

  switch(childPid = fork()) {
    case 0:  // child process
      //printenv();
      exit(0);
    default:   // parent process
      printenv();
      exit(0);
  }
}
```

Once `fork` has been called, the code detects whether it is running in
the child process (the result of `fork()` was 0) or the parent
(the result of `fork()` id the process ID of the child process -- see
`man 2 fork`).
Currently, the *parent*'s environment is printed, using the `printenv`
function.
If you run `./child_env`, you'll see a large amount of output -- so
we'll redirect it to a file:

```
$ ./child_env > parent_env.txt
```

Now comment out the parent call to `printenv()`, and uncomment the
child's, re-compile, and then run again:

```
$ ./child_env > child_env.txt
```

**Question**

:   Do the two files, `parent_env.txt` and `child_env.txt`, differ in
    any way? How can we find out?



<div class="solutions">

::: block-caption

Sample solutions

:::


We can compare our two files using the `diff` command (or in a graphical environment,
you could use a command like `meld`).

If we do so, `diff` produces no output, indicating the files are identical:

```
$ diff parent_env.txt child_env.txt
```

So it appears the child gets an exact copy of the parent's environment.

</div>



### 1.2. Environment variables and `execve`

Linux provides the `execve()` system call for invoking other programs (see `man execve`), plus
a number of "convenience" functions which act as "wrapper" functions around the system call
(see `man execl` for a list of them). They all operate by executing a specified executable
in the current process, such that it *replaces* the currently running program (unlike
`fork`, which spawns a new child process).

We've seen that the `fork()` system call results in child programs having a copy of their
parent process's environment. The `execve()` call, on the other hand, gives us precise
control over what environment is available to the newly-executed program: we can pass no
environment at all, a copy of the existing environment, or a totally "synthetic" environment
we've created. We'll write programs to try out each of those approaches.

First, we'll write a program which uses `execve()` to invoke the `printenv` program.
(You can read about the `printenv` command by running `man printenv`: by default, it simply
prints out the contents of all environment variables, much as our `print_env.c` program
above does -- though it has extra functionality as well.)

Try running `printenv` from the command line, so you can verify what its output normally looks
like.

Then save the following program as `use_execve.c`, and compile with
`make CFLAGS="-std=c11 -pedantic-errors -Wall -Wextra -Wconversion" use_execve.o
use_execve`:


```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main(void) {
  char *myargv[] = {
    "/usr/bin/printenv",
    NULL
  };

  execve("/usr/bin/printenv", myargv, NULL);

  return 0;
}
```

Read `man 2 execve` for details of the `execve` function (which we also looked at in
lectures).
The first argument
is a program to run: when `execve` is called, this program "replaces" the
one currently running. The second argument is a list of the arguments
passed to the new program. It has the same purpose and structure as `argv`
does in `main` of a C program, and is an array of strings, terminated by
a `NULL` pointer; the *first* of these normally holds the name of the program
being executed (though this is only a convention, and programs sometimes
set `argv[0]` to other things).
The last argument is to an array of strings
representing the environment of the new program.

**Question**

:   We have set the last argument to
    `NULL` -- what do you predict the output of running the program will be?
    Run the program and see if it matches your expectations.



<div class="solutions">

::: block-caption

Answer

:::

By passing an "empty" environment to `execve`, we invoke `/usr/bin/printenv` such that it
has no environment variables set at all: so running our program should result in no
output.

</div>





Now, replace the value of `NULL` which we passed to to `execve` with `environ` instead:


```
execve("/usr/bin/env", argv, environ);
```

**Question**

:   What do you predict will be the output? Run the program and check -- does it match your
    expectations?



<div class="solutions">

::: block-caption

Answer

:::

This time, we've passed our original `environ` variable to `execve()`, so the new program
should get an exact copy of the the original program's environment variables.

</div>



**Question**

:   How would you amend the program so as to pass exactly one, specified environment
    variable -- say, the variable `FOO`, set to value `BAR`? (Ask your lab facilitator for some
    hints if you are stuck.)



<div class="solutions">

::: block-caption

Answer

:::

We can construct an "artificial" environment much the same way as we construct the `myargv`
variable, since it is in exactly the same format (an array of strings, where the last
element is set to `NULL` to indicate the end of the list).

So the following will work:

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main(void) {
  char *myargv[] = {
    "/usr/bin/printenv",
    NULL
  };

  char *myenv[] = {
    "FOO=BAR",
    NULL
  };

  execve("/usr/bin/printenv", myargv, myenv);

  return 0;
}
```

When the program is run, `printenv` will find just one environment variable in the process
environment, and will print it as `FOO=BAR`.

</div>



**Question**

:   If are invoking `execve` in a program where security is important, which of the previous
    approaches is the most appropriate? Which is the least appropriate? Why?



<div class="solutions">

::: block-caption

Sample solution

:::

It depends exactly what the requirements are for the program we're invoking via `execve()` -- but
in general, if security is important, we want to exercise tight control over what
environment is passed to the programs we invoke, so passing an empty environment is better
than pussing a full copy of our original environment.

The program we're invoking might need particular environment variables to be set, or it might
not -- perhaps it needs `PATH` or `HOME` to be correctly set, for instance. If it does not
particular environment variables, we can construct those as needed, and then pass those to
`execve()`.

</div>





### 1.4. Environment variables and `system`

Save the following program as `use_system.c`, and compile with
`make CFLAGS="-std=c11 -pedantic-errors -Wall -Wextra -Wconversion" use_system.o use_system`.

```C
#include <stdio.h>
#include <stdlib.h>

int main() {
    system("/usr/bin/printenv");
    printf("back in use_system");

    return 0;
}
```

You can read about the `system` function using `man 3 system`.

**Question**

:   What do you predict will be the output? Should you see the output of `printf`?




<div class="solutions">

::: block-caption

Sample solutions

:::

Yes -- `system` spawns a new process using `fork` and returns,
so the `printf` *will* be executed.

</div>



### 1.5. `setuid` programs and `system`

Save the following program as `run_cat.c`, and compile with
`make CFLAGS="-std=c11 -pedantic-errors -Wall -Wextra -Wconversion" run_cat.o run_cat`.

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  const size_t BUF_SIZE = 1024;
  char buf[BUF_SIZE];
  buf[0] = '\0';

  if (argc != 2) {
    printf("supply a file to read\n");
    exit(1);
  }

  strcat(buf, "cat ");
  strcat(buf, argv[1]);

  system(buf);
  return 0;
}
```

If we invoke this as `./run_cat SOMEFILE`, we should be able to get the
contents of the file using the `cat` command (see `man 1 cat`).
Try running `./run_cat /etc/shadow` -- you should get a "permission
denied" error, which we expect, because `root` owns `/etc/shadow`, and
normal users do not have read access.

Now make `run_cat` a setuid program:

```
$ sudo chown root:root ./run_cat
$ sudo chmod u+s ./run_cat
```

**Question**

:   Try running `./run_cat /etc/shadow` again -- what do you see, and why?
    Instead of `cat`, you can imagine that we might instead invoke some
    other command which normally only root can run, but which we want to let
    other users run.



<div class="solutions">

::: block-caption

Sample solutions

:::

As a setuid program, `run_cat` now runs with effective user ID of 0
(that is, `root`), and *will* be able to read `/etc/shadow`.

</div>



You can find out where the `cat` command is that `run_cat` is executing
by running

```
$ which cat
```

This will look through the directories in our `PATH`, and report the
first one which contains an executable file called "`cat`".
Now we will manipulate the `PATH` environment variable so that `run_cat`
instead of accessing the normal system `cat` command, executes a command
of our choosing.

Create a file `cat.c` in the current directory, and edit with `vim`,
adding the following contents, then compile with `make CFLAGS="-std=c11 -pedantic-errors -Wall -Wextra -Wconversion" cat.o cat`.

```C
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void) {
  uid_t euid = geteuid();

  printf("DOING SOMETHING MALICIOUS, with effective user ID %d\n", euid);
}
```

Type

```
$ export PATH=$PWD:$PATH
```

and run `run_cat` again.

```
$ ./run_cat /etc/shadow
```

**Question**

:   What do you see? Why? And how would you fix this?



<div class="solutions">

::: block-caption

Sample solutions

:::

Because we have put our current working directory (`$PWD`) at the start
of `PATH`, `system` will look there first for a command called `cat`.

It will find our malicious `cat` program, and run it with effective
user ID of `root`. This means if a setuid program uses a call
like `system(some_string)` --
which has the effect of running `sh -c some_string` -- a
user can replace commands in `some_string` with malicious versions of
their own, *and* execute this with root privileges.

To fix this, there are several options:

- Use absolute paths in `some_string`, rather than letting the
  shell look for commands in the `PATH`. For more flexibility, we
  might look in several set locations (e.g. both `/bin` and `/usr/bin`),
  as not all operating systems put commands in the exact same location.

  (Ideally, it would be best to ensure those directories are writable
  only by `root`; if one of them can be written to by non-root users,
  then we haven't actually fixed the problem, since a non-root user
  could still insert a malicious binary in those locations.)

- Not use `system` at all; instead, use `fork` and `execve` ourselves,
  building up a set of arguments which can be passed to `execve`.


</div>



## 2. Building libraries

Save the following code as `mylib.c`.

```C
#include <stdio.h>

void useful_func(int s) {
  printf("Some very useful functionality\n");
}
```

We can build an object file `mylib.o` as follows:[^fpic]

```
$ gcc -g -fPIC -c mylib.c
```

[^fpic]: The `-fPIC` flag requests the compiler to create
  "position independent code", which can be moved around in
  memory and still work.

Now that our `useful_func` function has been compiled into object code, it can
be used in other programs. There are a few options for doing so.

We could link the `mylib.o` object file directly into a new program --
this is what we
do when we build large, multi-file C programs. When given a set of
object files, `gcc` will know it's being asked to invoke the *linker*,
and will combine multiple object files together (together with the
builtin C standard library).
When doing so, we invoke `gcc` like this:

```
$ gcc -o myprog obj1.o obj2.o ...
```

We could also build a *library* containing our new function, and make
this available to other developers. There are two options for doing so:
we can build a static library, or a shared (dynamic) library.

### 2.1. Static libraries

On Linux, a static library is a set of object files combined
into a single "`ar`"-format "archive" file. You can think of it as being
like a `.zip` or `.tar` file containing one or more "`.o`" files.
The `ar` command builds archive files in this format.
(You can look up `man ar` for more details, but they are not essential
for our purposes.)

The following command will build a static library containing our object
file, located in the directory `static-libs`:

```
$ mkdir -p static-libs
$ ar rcs ./static-libs/libmylib.a mylib.o
```

This produces the static library file `libmylib.a`. To use the static library in a
program, we need to tell the linker to link against our library,
and also where our library is located. `gcc` normally looks for
libraries in default locations -- in the standard
CITS3007 development environment, one of these locations
is the directory `/usr/lib/x86_64-linux-gnu/`. If you list the contents
of that directory, you find a number of static libraries -- one for
instance is `/usr/lib/x86_64-linux-gnu/libcrypt.a`, part of the
[libxcrypt][libxcrypt] library.

[libxcrypt]: https://salsa.debian.org/md/libxcrypt

To make our `useful_func` function easy to use by other developers,
we would normally also provide them with appropriate header
files, but in this case we will manually insert the
declarations for `useful_func`.

Insert the following into a file `usemylib.c`:

```C
#include <stdio.h>
#include <stdlib.h>

void useful_func(int s);

int main(int argc, char ** argv) {
  printf("\ncalling useful_func routine:\n");
  useful_func(1);
}
```

We can compile it with `gcc -c -g usemylib.c`, and then link it against our
static library. The `-L` option to `gcc` indicates a non standard
directory where libraries can be found, and the `-l` option gives the
name of a library to link. (`gcc` by default assumes it should add
`lib` in front of this name and `.a` after, to get the filename to link
against.)

```
$ gcc  usemylib.o  -L./static-libs -lmylib -o statically-linked-usemylib
```

Run the binary with `./statically-linked-usemylib`. This executable
contains a *full copy* of the `useful_func()` binary code from our `mylib.o`
file.[^static-conts]

[^static-conts]: We can confirm this by running several commands.
  `objdump -d --source mylib.o` will show us the compiled assembly code
  for the `useful_func` function. Running `objdump -d --source
  static-libs/libmylib.a` will confirm that it has been copied into
  `static-libs/libmylib.a`.
  And `objdump -d --source statically-linked-usemylib` will confirm that
  it's been copied into
  the executable `statically-linked-usemylib` -- look for the section
  headed "`<useful_func>`", and you'll see the original assembly code.

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Static vs shared libraries

:::

Users and developers tend to favour using *statically* linked
libraries in executables. It makes the executable larger
than if it used shared
libraries (discussed in the next section), because a full copy of the
library routines is copied into the executable; but on the other hand,
it makes the executable more portable and self-contained, because there's no
need to to both download the executable,
*and* install the shared libraries needed to run it.

**Example**

:   The [`croc`][croc] and [`age`][age] projects both provide statically linked
    executables for storing and transmitting files securely on multiple operating systems.
    (They are written using the [Go](https://go.dev) language, which is especially suited
    to creating static executables.)

    Binary executables for different platforms can be downloaded by going to the "Releases"
    link (on the right-hand side of the GitHub project page), then looking under "Assets"
    for a list of binary executables which can be directly downloaded and run on a user's system.
    The executables are all statically linked, so no extra libraries are required to run
    them -- any library routines the executable uses are already "baked in" to the
    executable.

    On the CITS3007 SDE, we can see that the GDB binary executable, `/usr/bin/gdb`, on the
    other hand, makes use of many shared libraries: type `ldd /usr/bin/gdb` for a list of
    them. GDB is most conveniently installed using the system package manager, `apt-get`,
    which keeps track of what shared libraries each program requires and checks that they're
    properly installed.

    (If you download a binary for `croc` or `age` and try running `ldd` on it, what result
    do you see?)

[croc]: https://github.com/schollz/croc
[age]: https://github.com/FiloSottile/age

System administrators, on the other hand, often tend to prefer it when
executables use shared libraries. One reason is that multiple
executables can all use the same shared library, taking up less disk space.
But a more significant reason is that it's easier to fix things
if a vulnerability is found in the library.

If a new version of a shared library is installed which fixes a
vulnerability, then all executables using that shared library get the
benefit of using the new version (without needing to update the
executables). On the other hand, if there are
executables which are linked *statically* against the library, we must
ensure each one has been updated "upstream" (i.e. by the developer of
the executable) to incorporate the fixed library version, and download
and install each executable.

</div>

### 2.2. Dynamic shared libraries


We can create a *shared* library with the following commands:

```
$ mkdir -p shared-libs
$ gcc -shared mylib.o -o shared-libs/libmylib.so
```


To link against this shared library, we invoke gcc as follows:

```
$ gcc usemylib.o -L./shared-libs -lmylib -o dynamically-linked-usemylib
```

Try running `./dynamically-linked-usemylib`. You should see an error
message like the following:

```
error while loading shared libraries: libmylib.so: cannot open shared object file: No such file or directory
```

When an executable that makes use of shared libraries is run, a program
called the [dynamic linker](https://en.wikipedia.org/wiki/Dynamic_linker)
is responsible for finding the necessary
libraries[^shared-elf] and looking up the location of any requested
functions in those libraries.[^relocs]
In the present case, it doesn't know where to find the file
`libmylib.so`, so it reports an error.[^ldd]

[^shared-elf]: The process is roughly as follows (see
  [`man 8 ld.so`][ld-so] and "[The ELF format - how programs look from
  the inside][elf]"). The kernel loads the executable into memory, and
  looks to see if it contains an `INTERP` directive, which specifies an
  interpreter to use.
  Statically linked binaries don't need an interpreter. Dynamically linked programs
  use `/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2`, which runs some
  initialization code, loads shared libraries needed by the binary, and
  performs
  [*relocations*](https://en.wikipedia.org/wiki/Relocation_(computing))
  -- adjusts the code of an executable so that it looks at the right
  addresses for any functions it needs.\
  &nbsp; See for more information "[How programs get run: ELF
  binaries](https://lwn.net/Articles/631631/)".\
  &nbsp; An interesting side-effect of this setup is that you
  can use  `/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2` to run a
  binary even when it doesn't have it's "executable" permissions set.
  Remove the executable permissions from some binary `mybinary` with
  `chmod a-rx mybinary`,  and you can still run it with the command:\
  <pre><code>/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 ./xxx</code></pre>
  <!--
  See e.g.
  <https://stackoverflow.com/questions/69481807/who-performs-runtime-relocations>
  and
  <https://www.technovelty.org/linux/plt-and-got-the-key-to-code-sharing-and-dynamic-libraries.html>
  )
  -->

[^relocs]: The linked `dynamically-linked-usemylib` program contains what are called
  "relocations" -- descriptions of functions that will need to be
  "filled in" when the executable is loaded into memory and shared
  libraries are linked. Running `readelf --relocs
  ./dynamically-linked-usemylib` will tell us what these are: we should
  be able to see an entry for `printf` and `useful_func`:\
  <pre><code>
  Relocation section '.rela.plt' at offset 0x610 contains 2 entries:
  &nbsp; Offset          Info           Type           Sym. Value    Sym. Name + Addend
  000000003fc8  000200000007 R_X86_64_JUMP_SLO 0000000000000000 printf@GLIBC_2.2.5 + 0
  000000003fd0  000500000007 R_X86_64_JUMP_SLO 0000000000000000 useful_func + 0
  </code></pre> \
  The relocation tells the dynamic linker: "After the executable is loaded into
  memory, patch the address found at offset `000000003fd0` (the first column),
  and replace it with the address of symbol `useful_func`."

[ld-so]: https://man7.org/linux/man-pages/man8/ld.so.8.html
[elf]: https://www.caichinger.com/elf.html

[^ldd]: The `ldd` command can be used to find out what
  shared libraries are required by an executable.
  Run `ldd dynamically-linked-usemylib`, and you should get
  output like the following:\
  &nbsp;\
  <pre><code>$ ldd dynamically-linked-usemylib
	linux-vdso.so.1 (0x00007ffe43a66000)
	libmylib.so => not found
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f903fc18000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f903fe1b000)
  </code></pre>\
  This is telling us that one of the libraries this executable depends
  on, `libmylib.so`, cannot be found using the current search path.



We could fix this by putting the `.so` file in a standard location
(`/usr/lib/x86_64-linux-gnu/`) where the dynamic linker can find it,
or we can use the `LD_LIBRARY_PATH` environment variable to specify the
location.[^plugins] The `LD_LIBRARY_PATH` environment variable contains a list of
locations where the dynamic linker should look for shared libraries.
Run the following:

```
$ LD_LIBRARY_PATH=./shared-libs ./dynamically-linked-usemylib
```

You should see that the executable runs without error, and calls the
`useful_func` function in our shared library.

[^plugins]: A third option is that we could make use of the API
  provided by the dynamic linker to programmatically load
  shared libraries and look up particular functions we want
  by name
  (using the functions [`dlopen`][dlopen] and [`dlsym`][dlsym]).
  Effectively, we are doing manually what the dynamic linker
  does automatically when an executable that uses shared libraries is
  run.\
  &nbsp; This functionality is often used to make "plugins" for a
  program -- modules which can be downloaded and installed to augment
  the program's functionality. (For instance, a graphics editing program
  might use plugins to allow it so save images in a new format.)
  See ["Dynamically Loaded (DL)
  Libraries"](https://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html)
  for more details.\
  &nbsp; Making use of plugins comes with risks, however: a plugin can
  perform arbitrary actions at runtime, and it is very difficult
  to ensure in advance that those actions are "safe".

[dlopen]: https://linux.die.net/man/3/dlopen
[dlsym]: https://man7.org/linux/man-pages/man3/dlsym.3.html

Now let's imagine some adversary has created a version of the
`mylib` library which contains malicious code.

Create the following file, `evil_lib.c`, and compile it with
`gcc -g -fPIC -c evil_lib.c`.


```C
#include <stdio.h>

void useful_func(int s) {
    // we could now run arbitrary code and cause damage.
    printf("Malicious things -- bwahaha!\n");
}
```

Build a library from it using the following commands:

```
$ mkdir -p evil-shared-libs
$ gcc -shared evil_lib.o -o evil-shared-libs/libmylib.so
```

And run our existing dynamically linked binary, but with
a different `LD_LIBRARY_PATH`:

```
$ LD_LIBRARY_PATH=./evil-shared-libs ./dynamically-linked-usemylib
```

What happens, and what are the security implications of this?



<div class="solutions">

::: block-caption

Sample solutions

:::

The function in the malicious version of the library
is called.
If a user can control the value of `LD_LIBRARY_PATH`, they
can arrange for a malicious version of existing libraries to be run.

</div>



In principle, we could use this technique even to override
functions in `libc`, the standard C library.[^libc-override]
But note that in the normal case, code will only be run with a user's
normal privileges. This is still a security issue (malicious libraries
could, for instance, email copies of the user's private files), but
doesn't give superuser access to a machine.
However, what happens if the binary is a setuid executable?

[^libc-override]: This can be used, for instance, to test performance of
  alternative implementations of those functions, without having to recompile
  our program. The `LD_LIBRARY_PATH` and `LD_PRELOAD` environment variables are
  both useful for this purpose. `LD_LIBRARY_PATH` contains a list of directories in which to
  search for libraries, but `LD_PRELOAD` contains a list of specific library files to be
  loaded before any other libraries are. The documentation for both is in `man ld.so`, and
  a blog post discussing their use for testing can be found [here][lib_testing].

[lib_testing]: https://web.archive.org/web/20170503183448/https://samanbarghi.com/blog/2014/09/05/how-to-wrap-a-system-call-libc-function-in-linux/

### 2.2. `LD_LIBRARY_PATH` and setuid

Try making `dynamically-linked-usemylib` a root-owned setuid program,
and then running it with a specified `LD_LIBRARY_PATH`:

```
$ sudo chown root:root ./dynamically-linked-usemylib
$ sudo chmod u+s ./dynamically-linked-usemylib
$ LD_LIBRARY_PATH=./shared-libs ./dynamically-linked-usemylib
```

What do you observe?



<div class="solutions">

::: block-caption

Sample solutions

:::

An error should occur, stating that libmylib.so can't be found.

</div>



Let's find out why this occurs. Create the following program,
`print_ld_env.c`, and compile it with
`make CFLAGS="-std=c11 -pedantic-errors -Wall -Wextra -Wconversion" print_ld_env.o print_ld_env`:


```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

int main(void) {
  printf("some environment variables:\n");
  for (char **var = environ; *var != NULL; var++) {
    if (strncmp(*var, "LD", 2) == 0) {
      printf("var %s\n", *var);
    }
  }
}
```

Run it with several environment variables set:

```
$ LD_LIBRARY_PATH=./shared-libs LD_SOME_RANDOM_VAR=xxx ./print_ld_env
```

What do you see? What is the program doing?



<div class="solutions">

::: block-caption

Sample solutions

:::

The program is printing out the name and contents of any
environment
variables that begin with the letters "LD".

</div>



Make the `print_ld_env` a setuid executable, and run it again:

```
$ sudo chown root:root ./print_ld_env
$ sudo chmod u+s ./print_ld_env
$ LD_LIBRARY_PATH=./shared-libs LD_SOME_RANDOM_VAR=xxx ./print_ld_env
```

What do you observe? Why might this happen?
(Hint: check the `man 8 ld.so` man page, and look under
"secure-execution mode".)



<div class="solutions">

::: block-caption

Sample solutions

:::

When the dynamic linker detects that a process's real and effective user
IDs differ (as they do, for a setuid executable), it ignores the value
of environment variables which would normally alter the linker's
behaviour (like `LD_LIBRARY_PATH`); furthermore, it strips those
variables out of the environment.

However, `LD_SOME_RANDOM_VAR` is not one of those variables, so it
remains in the environment.

</div>





<!--
  vim: syntax=markdown tw=92 :
-->
