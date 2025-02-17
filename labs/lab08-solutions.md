---
title:  CITS3007 lab 8 (week 9)&nbsp;--&nbsp;Race conditions&nbsp;--&nbsp;solutions
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




<div class="solutions">

::: block-caption

Sample solution

:::

The answer to both questions is "No". By definition, a program with a race condition only
works correctly when events occur in the "right" order, and they are not guaranteed to do
so. An attack on such a program depends on events ocurring in the "right" order for the
attacker, and this, also, is not guaranteed to happen.


</div>



**Question 1(b)**

:   What is a symlink attack? See if you can find out how they are typically defined,
    and how they can be protected against.
    How do they relate to race conditions? If a race condition
    is involved, identify the resource being altered.



<div class="solutions">

::: block-caption

Sample solution

:::

Symlink attacks were described in lab 4, on `setuid` vulnerabilities, and further
information is available [in the CAPEC database][capec-symlink] which describes different
attack patterns.
They occur when an attacker creates a symbolic link so that a vulnerable program accesses
the link's endpoint (say, `/etc/some_sensitive_file`) on the mistaken assumption it is
accessing a file at the link's source path.
The result is that the vulnerable program reads from or writes to an incorrect file.

[capec-symlink]: https://capec.mitre.org/data/definitions/132.html

Symlink attacks need not involve race conditions: *any* attack where the attacker induces a
program to read from or write to an incorrect file by using a symbolic link
counts as a symlink attack. However, a common sort of vulnerability is where:

i.  some privileged (e.g. `setuid`) program checks to see whether it should
    access some file *F*
#.  the program later does access file *F* (either reading or writing it)
    based on the check in step (i), and steps (i) and (ii) are not atomic
#.  an attacker can unlink file *F* and replace it with a symbolic link to a different
    file (call it *E*) which the attacker should not have access to.

This counts as a race condition because correct operation of the program will only happen
if the file is not replaced between steps (i) and (ii); but since they are not guaranteed to
be atomic, there is an interval between the steps, and the file can be replaced during that
interval.
This sort of vulnerability is sometimes called a [symlink
race](https://en.wikipedia.org/wiki/Symlink_race), and it is an example of a TOCTOU
vulnerability (which we have covered earlier).

How to defend against the attack varies on the logic of the vulnerable program, but the
general approaches are:

-   Avoid the TOCTOU bug by only accessing the file path *once*: obtain a file handle by
    opening the file, and check permissions using the open file handle.
-   Protect the target file *F* by putting it in a directory an attacker does not have
    access to.

The resource being accessed as part of the race condition is the file *F*.

</div>



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



<div class="solutions">

::: block-caption

Sample solution

:::

According to the documentation at
<https://clang.llvm.org/docs/ThreadSanitizer.html>, a sanitized program uses more memory:

> At the default settings the memory overhead is 5x plus 1Mb per each thread.

</div>




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

**Exercise**

:   The traditional way to protect against a data race in this program would be to
    either use *atomic types* (i.e. alter the type of `GLOBAL`), or to use *locks*
    (e.g. mutexes -- "mutual exclusion locks"). See if you can amend the program to
    use one of these two approaches. Which of these approaches can successfully fix the
    issue?



<div class="solutions">

::: block-caption

Sample solution

:::

One approach is to use atomic types. They were introduced in C11, and you can read about them
[here](https://en.cppreference.com/w/c/atomic).
We can write the program in that case
as follows:


```c
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <unistd.h>

atomic_int GLOBAL;

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

We have added the use of the `<stdatomic.h>` header, and changed `GLOBAL` to be of type
`atomic_int`.

We can run the following bash code to see if TSan detects a data race (hit ctrl-c to stop
it):

```bash
i=0; while (./race2-atomic ; [ $? -ne 66 ]); do echo $i; i=$((i+1)); done
```

But no data race should be detected; by using atomics, we have removed the data race, and
our program is well-defined.

::: block-caption

Mutexes

:::

Alternatively, we can use the mutex types from the Pthreads library to protect our
`GLOBAL` variable.

```c
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int GLOBAL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // initialize

void* operation1(void *x) {
    pthread_mutex_lock(&mutex); // request lock before accessing shared variable
    GLOBAL = 99;
    pthread_mutex_unlock(&mutex); // release lock
    return x;
}

int main(void) {
    pthread_t t;
    pthread_create(&t, NULL, operation1, NULL);
    
    pthread_mutex_lock(&mutex); // request lock before accessing shared variable
    GLOBAL = 100;
    pthread_mutex_unlock(&mutex); // release lock
    
    pthread_join(t, NULL);
    
    pthread_mutex_lock(&mutex); // request lock before accessing shared variable
    int tmp = GLOBAL;
    pthread_mutex_unlock(&mutex); // release lock
    
    if (tmp == 99)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}
```

We can compile and test this with TSan, and again, no data race should be detected.

::: block-caption

"Native" C11 locks

:::

C11 introduces a "native" type of thread which doesn't use the Pthreads API.
We can rewrite our program using the new API, which is found in `<threads.h>`.

```c
#include <stdio.h>
#include <threads.h>

int GLOBAL;
mtx_t mutex;

int operation1(void *x) {
    mtx_lock(&mutex); // request lock before accessing shared variable
    GLOBAL = 99;
    mtx_unlock(&mutex); // release lock
    return 0;
}

int main(void) {
    thrd_t t;
    mtx_init(&mutex, mtx_plain); // Initialize the mutex
    
    thrd_create(&t, operation1, NULL); // Create a thread
    
    mtx_lock(&mutex); // request lock before accessing shared variable
    GLOBAL = 100;
    mtx_unlock(&mutex); // release lock
    
    thrd_join(t, NULL);
    
    mtx_lock(&mutex); // request lock before accessing shared variable
    int global_value = GLOBAL; // Read the value of GLOBAL
    mtx_unlock(&mutex); // Unlock the mutex after reading the shared variable
    
    // Check the value of GLOBAL
    if (global_value == 99)
        printf("Exit Success\n");
    else
        printf("Exit Failure\n");
    
    mtx_destroy(&mutex); // Destroy the mutex
    return 0;
}
```

Unfortunately, the TSan sanitizer may not work with C11 "native" threads --
see [here](https://github.com/google/sanitizers/issues/1195).

</div>




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



<div class="solutions">

**Sample solutions**

You should observe that the listed permissions stay exactly
the same, and the `vagrant` user can still read the file contents.

</div>



Now we'll try making a symlink again, but putting it in the `/tmp` directory:

```
$ sudo su user2 -c 'ln -s /home/user2/file /tmp/link'
```

What happens if you execute the command `cat /tmp/link` (as the `vagrant user`)?



<div class="solutions">

**Sample solutions**

You should observe that a "Permission denied" error occurs.

</div>



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



<div class="solutions">

**Sample solution**

This program has a "TOCTOU" vulnerability, and uses the deprecated `access()` function.

Due to the time window between the file permissions check (`access()`)
and the file use (`fopen()`),
there is a possibility that the file used by `access()` is different from the file used by `fopen()`, even
though they have the same file name `/tmp/XYZ`.
If a malicious attacker can somehow make `/tmp/XYZ`
a symbolic link pointing to a protected file, such as `/etc/passwd`, inside the time window, the attacker
can cause the user input to be appended to `/etc/passwd` and as a result gain root privileges.

How might an attacker exploit this?

If they can alter `/etc/passwd`, they could add an extra line that looks something like this:

```
  sploit:x:0:0::/root:/bin/bash
```

Here, a new `sploit` account is created, which has root privileges since it has userid 0.

On its own, this is not quite enough, because the attacker still needs a password to access
the new `sploit` account.

On Ubuntu systems, however, a particular "magic" password value
is used for [passwordless guest accounts][guest],
and the magic value is `U6aMy0wojraho` (the
6th character is zero, not letter O).
If the attacker puts this value in the password field of a user entry, they can just 
hit the return key when prompted for a password, and then can log into the user's
account.

The attacker would still need to unlink the `/tmp/XYZ` file and replace it with a symlink to
`/etc/passwd` in the narrow time window between the "check" (`access()`) and use of the
file. Typically, they would do so by running `append` many times, 
 

</div>



**Question**

:   Would the ThreadSanitizer help in detecting this problem? Why or why not? 



<div class="solutions">

**Sample solution**

It would not. TSan assists in detecting data races, which are to do with multithreaded
access to a program variable (where at least one thread performs a write).

But the race condition here is not a data race -- the "resource" being contended for is not
a variable in a program, but a file on the file system. TSan cannot assist us in preventing
such vulnerabilities.  

</div>








<!-- vim: syntax=markdown tw=92
-->
