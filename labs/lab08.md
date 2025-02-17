---
title:  CITS3007 lab 8 (week 9)&nbsp;--&nbsp;Race conditions
---

## 0. Background



This lab explores *race condition* vulnerabilities.  A race condition is any situation where
the timing or order of events affects the correctness of programs or code. For a race
condition to occur, some form of *concurrency* must exist -- e.g., multiple processes or
threads of control running at the same time -- as well as some sort of mutable resource. A
race condition occurs when the same data is accessed and written by multiple threads of
control or processes.

A common sort of resource for programs to use is files in the filesystem.  If a `setuid`
program that uses files has a race condition vulnerability, attackers may be able to run a
parallel process and attempt to subvert the program behaviour.

**Question 1(a)**

:   Is a program with a race condition always guaranteed to work correctly?
    Is an attack on a program with a race condition always guaranteed to succeed?




**Question 1(b)**

:   What is a symlink attack? See if you can find out how they are typically defined,
    and how they can be protected against.
    How do they relate to race conditions? If a race condition
    is involved, identify the resource being altered.



<!--
This lab covers the following topics:

- Race condition vulnerability
- Sticky symlink protection
- Principle of least privilege

-->
<!--
lab questions to add

Checkpoint 1. What is a race condition?
Checkpoint 2. What is the general target for race condition attacks?
Checkpoint 3. What is the TOCTTOU (Time Of Check To Time Of Use) design flaw?
Checkpoint 4. What is the relationship between TOCTTOU design flaw and the race condition attack?
Checkpoint 5. Explain if a race condition is always guaranteed to succeed.
Checkpoint 6. What is a symlink / path attack?
Checkpoint 7. How is a symlink / path attack related to a race condition?
-->


## 1. Data races and ThreadSanitizer

In multithreaded programs, it may be possible for multiple threads to access some memory
location. If two threads access the same variable concurrently and at least one of the
accesses is a write, then that is a *data race*, and it is undefined behaviour in C.

Save the following program as `race1.c`, and compile it with:

```bash
gcc -std=c11 -pedantic-errors -Wall -Wextra -pthread -o race1 race1.c
```

Program `race1.c`:

```c
#include <pthread.h>
#include <stdio.h>

long GLOBAL;

void *operation1(void *x) {
  GLOBAL++;
  return NULL;
}

void *operation2(void *x) {
  GLOBAL--;
  return NULL;
}

int main() {
  pthread_t t[2];
  pthread_create(&t[0], NULL, operation1, NULL);
  pthread_create(&t[1], NULL, operation2, NULL);
  pthread_join(t[0], NULL);
  pthread_join(t[1], NULL);
}
```

This program uses the Pthreads library to control program threads. Two threads are created
using the `pthread_create` function, and `main` waits for them to finish by calling
`pthread_join`. One of the threads increments `GLOBAL`, the other decrements it.
However, they are doing so without any sort of synchronization, so this counts as a data
race and is undefined behaviour. If the program operates as the programmer might expect, then
one thread increments `GLOBAL`, another decrements it, and the end result should be that
`GLOBAL` is 0 at the end of the program.
However, because our program invokes undefined behaviour, any result is possible: the
variable could end up with other values (i.e. data corruption).

We can detect this race condition using [ThreadSanitizer][tsan] (TSan, for short).
Compile again with the following command. (When compiling, we add the
`-g` option to improve error messages printed by TSan, but you can also leave it off.)

```bash
gcc -g -std=c11 -pedantic-errors -Wall -Wextra -fsanitize=thread -pthread -o race1 race1.c
```

[tsan]: https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual 

Then run the program. You should see output something like the following:

```plain
==================
WARNING: ThreadSanitizer: data race (pid=590418)
  Read of size 4 at 0x561c214a7014 by thread T2:
    #0 operation2 /home/vagrant/race1.c:12 (race1+0x12f1)

  Previous write of size 4 at 0x561c214a7014 by thread T1:
    #0 operation1 /home/vagrant/race1.c:7 (race1+0x12ac)

  Location is global 'GLOBAL' of size 4 at 0x561c214a7014 (race1+0x000000004014)

  Thread T2 (tid=590421, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 main /home/vagrant/race1.c:19 (race1+0x1388)

  Thread T1 (tid=590420, finished) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 main /home/vagrant/race1.c:18 (race1+0x1367)

SUMMARY: ThreadSanitizer: data race /home/vagrant/race1.c:12 in operation2
==================
0
ThreadSanitizer: reported 1 warnings
```

When we compile with ThreadSanitizer, our program is instrumented (i.e., extra instructions
are added) so that it keeps track of the accesses each thread makes to memory.
By default, the last $2^{17}$, or roughly 128,000, accesses are tracked. It is possible to
alter this number when your program is invoked. The following invocation

```
$ TSAN_OPTIONS="history_size=3" ./race1
```

will double the number of accesses tracked.
If the ThreadSanitizer finds that more than one of those accesses is to the same memory
location, and at least one of those accesses was a write, then this will be flagged as being a race condition.

**Question 1(a)**

:   Find out what resources are used by a program with TSan enabled, compared with a
    program which does not have it enabled.




However, ThreadSanitizer is not infallible, as we will demonstrate.
Here is a second program -- save it as `race2.c`:

```c
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int GLOBAL;

void* operation1(void *x) {
  GLOBAL = 99;
  return x;
}

int main(void) {
  pthread_t t;
  pthread_create(&t, NULL, operation1, NULL);
  GLOBAL = 100;
  pthread_join(t, NULL);
  if (GLOBAL == 99)
    return EXIT_SUCCESS;
  else
    return EXIT_FAILURE;
}
```

Compile it as follows:

```bash
$ gcc -g -std=c11 -pedantic-errors -Wall -Wextra -pthread -o race2 race2.c
```

In this program, a thread is spawned which sets the value of `GLOBAL` to 99, while the
`main` function concurrently sets it to 100 -- this again, is a data race. Typically, the
`main` function will "win", and the value will be 100, but sometimes not. We can demonstrate
this by running the following Bash code:

```bash
$ i=0 ; while ./race2 ; do echo $i ; i=$((i+1)) ; done
```

In the cases where the `main` function "wins", `race2` will exit with exit code 1, and the
while loop will continue. However, if the thread "wins", `race2` will exit with exit code 0,
and the while loop will halt. If you run the program, you should see the `main` function
"win" many times, but eventually, the thread will succeed instead -- and the value of `i`
will show how many times we had to run the program before this happened. (Typical values are
somewhere in the thousands, but it could sometimes be higher or lower.)

Now compile the program and run it with ThreadSanitizer enabled:

```bash
$ gcc -g -std=c11 -pedantic-errors -Wall -Wextra -fsanitize=thread -pthread -o race2 race2.c
$ i=0; while (./race2 ; [ $? -ne 66 ]); do echo $i; i=$((i+1)); done
```

By default, if TSan detects a race condition, the program exits with exit
code 66 (see the [TSan options documentation][tsan-options]).
(We could alter this by invoking our program with, say, `TSAN_OPTIONS="exitcode=3" ./race2`
if we wanted to force the exit code to be 3 instead.)
Our `while` loop continues to run until TSan does detect a race condition.

[tsan-options]: https://github.com/google/sanitizers/wiki/ThreadSanitizerFlags 

You will typically see that TSan does not always detect a race condition, but eventually
does.
Why does TSan not always detect the race? Because sometimes,  
the line `GLOBAL = 100` is executed before the operating system has finished creating a new
thread at all. In that case, TSan does not "kick in" until the thread is created, and
doesn't realize that the thread is altering a variable which was also altered in `main`.

**Self-study exercise**

:   The traditional way to protect against a data race in this program would be to
    either use *atomic types* (i.e. alter the type of `GLOBAL`), or to use *locks*
    (e.g. mutexes -- "mutual exclusion locks"). See if you can amend the program to
    use one of these two approaches.

## 2. Protection against symlink attacks

Recent versions of Ubuntu (10.10 and later) come with a built-in protection
against some race condition attacks. Specifically, they mitigate against some
symbolic link (symlink)
attacks (which we saw in lectures).

In the CITS3007 development environment, we will create a new user
(in addition to the "`vagrant`" user we log in as) with their own
home directory:

```
$ sudo adduser --disabled-password --gecos '' user2
```

As that user, we'll create a new file and a symlink to it:


```
$ sudo su user2 -c 'echo hello > /home/user2/file'
$ sudo su user2 -c 'ln -s /home/user2/file /home/user2/link'
```

By default, a user's new files are world readable, so the `vagrant`
user can read the file and the symlink:

```
$ ls -l ~user2
total 4
-rw-rw-r-- 1 user2 user2  6 Sep 27 00:31 file
lrwxrwxrwx 1 user2 user2 16 Sep 27 00:32 link -> /home/user2/file
$ cat /home/user2/file
hello 
```

Note that the permissions of the symlink are "`rwx`" for user, group
and the "world" -- this is because on Linux, symlinks have no
"permissions" of their own; permissions are taken from the file being
linked to.

As `user2`, we'll try removing "world" permissions from the symlink:

```
$ sudo su user2 -c 'chmod o-r /home/user2/link'
```

Does this make a difference to the permissions of the `link` file,
as displayed by `ls`? Can the `vagrant` user still access it?



Now we'll try making a symlink again, but putting it in the `/tmp` directory:

```
$ sudo su user2 -c 'ln -s /home/user2/file /tmp/link'
```

What happens if you execute the command `cat /tmp/link` (as the `vagrant user`)?



The `tmp` directory has special permissions, on Unix-like systems. Run `ls -ld /tmp`,
and you should see output like the following:

```
$ ls -ld /tmp
drwxrwxrwt 12 root root 4096 Sep 27 00:38 /tmp
```

The "`t`" at the end of the permissions means a permission bit called the "sticky bit"
has been set for the `/tmp` directory.
When this bit is set on a directory, and some user creates a file in it,
other users (except for the owner of the directory, and of course `root`) are
prevented from deleting or renaming the file.

<!--
  TODO: exercise showing this
-->
 
The sticky bit is set on the `/tmp` directory to ensure one user's temporary
files can't be renamed or deleted by other users.
In addition to this, the Linux kernel introduced [additional protections][linux-symlinks]:
symbolic links in world-writable sticky directories (such as `/tmp`) can *only be followed*
if the follower (i.e., the user executing a command) and the directory owner (that is,
`root`, in the case of the `/tmp` directory) match the symlink owner.

[linux-symlinks]: https://lwn.net/Articles/390323/

<!--
  TODO: exercise showing this
-->

(Note that these built-in protections are **not** sufficient security for safely
creating temporary files. It's usually best to ensure that only the actual user
of a process
can even list or read temporary files: a program should create its own temporary
*directory* under `/tmp`, to which only the actual user has read, write or execute
access, and then create needed temporary files within that directory.)

This protection can be removed by running the following command, which alters kernel
parameters:

```
$ sudo sysctl -w fs.protected_symlinks=0
```

If you try the previous exercises again, you should see that this time, the `vagrant` user
*can* run `cat /tmp/link` without a "permission denied" error.

Another protection was added in Ubuntu 20.04: even root cannot write to files in `/tmp` that
are owned by others. That can be disabled by running the following command:

```
$ sudo sysctl fs.protected_regular=0
```

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Linux security modules

:::

In earlier versions of the Linux kernel (for instance,
on Ubuntu 12.04), the "symlinks in sticky-bit directories" protection
was provided by a Linux security module called "Yama", and
could be disabled using the following command:

```
$ sudo sysctl -w kernel.yama.protected_sticky_symlinks=0
```

If you aren't able to easily run the CITS3007 standard development environment
(e.g. because you are using an M-series MacOS computer), and are using an earlier version of
Ubuntu instead, then the "yama" version of the command might work instead.

The Linux kernel provides a security framework consisting of various "hooks"
which can be used by Linux security *modules*. For instance, normally
in the Linux kernel, read permissions for a file are only checked
when a file is opened.
However, the security framework provides 
"file hooks" which allow security modules to specify checks which
should be made whenever a read or write is performed on a file descriptor
(for example, to revalidate the file permissions in case they have changed).

We will not look in detail at how the security framework and
modules work, but if you are interested,
the architecture of the framework is described in a [2002 paper][linux-sec],
and a guide to some of the modules is provided [here][linux-sec-guide].

[linux-sec]: https://www.usenix.org/legacy/event/sec02/wright.html
[linux-sec-guide]: https://www.starlab.io/blog/a-brief-tour-of-linux-security-modules 

<!--
  "Yama" appears to be named after the Hindu deity: <https://lwn.net/Articles/393008/>
-->


A list of the currently enabled Linux security modules can be printed by
running

```
$ cat /sys/kernel/security/lsm
```

In more recent kernels, the "symlinks in sticky-bit directories" protection
is built into the kernel.

</div>


### 2.2. A setuid program

Consider the following program, `append.c`:

```C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
  char * filename = "/tmp/XYZ";
  char buffer[60];
  FILE *fp;

  // get user input
  printf("text to append to '%s': ", filename);
  fflush(stdout);

  scanf("%50s", buffer );

  // does `filename` exist, and can the actual user write
  // to it?
  if (!access(filename, W_OK)) {
    fp = fopen(filename, "a+");
    fwrite("\n", sizeof(char), 1, fp);
    fwrite(buffer, sizeof(char), strlen(buffer), fp);
    fclose(fp);
    exit(0);
  }

  printf("No permission\n");
  exit(1);
}
```

It's intended to be a root-owned setuid program, which takes a string
of input from a user, and appends it to the end of a temporary file
`/tmp/XYZ` (if that file exists) -- but only if the user who runs the
program would normally have permissions to write to the file.
Because the program runs with root privileges (i.e., has an effective
user ID of `0`), it normally could overwrite any file. Therefore,
the code above uses the `access` function (discussed in lectures)
to ensure the *actual* user running the program has the correct
permissions.

Save the program as `append.c`, and compile it with `make append.o append`.
Then make it a root-owned setuid program:

```
$ sudo chown root:root append
$ sudo chmod u+s append
```

**Question**

:   At first glance the program may not seem to have any problem.
    However, there is a race condition vulnerability in the program --
    can you describe what it is? How might an attacker try to exploit this program?



**Question**

:   Would the ThreadSanitizer help in detecting this problem? Why or why not? 








<!-- vim: syntax=markdown tw=92
-->
