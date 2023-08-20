---
title: |
  CITS3007 lab 4 (week 5)&nbsp;--&nbsp;`setuid` vulnerabilities
include-before: |
  ```{=html}
  <style>
  figcaption {
    font-weight: bold;
    text-align: center;
    font-style: italic;
  }

  </style>
  ```
---

It's recommended you complete this lab in pairs, if possible, and
discuss your results with your partner. Any exercises you don't complete
during the lab, you should finish in your own time.

## 1. `setuid`

Recall from last week's lab that `setuid` ("set user identity") is an
important security mechanism on Unix operating systems,
and that `setuid` programs execute with the privileges of the *owner of
the executable file* rather than the privileges of the user executing
the command.

We looked at how you can add `setuid` functionality to an executable
file (using `chmod u+s`), and some best practices to keep in mind when
writing `setuid` programs --
the ["Principle of Least Privilege"][secure-howto] (POLP), and a number of
guidelines from the [Software Engineering Institute][permission-relinq]
(SEI) at Carnegie Mellon University.

[secure-howto]: https://dwheeler.com/secure-programs/3.012/Secure-Programs-HOWTO/minimize-privileges.html
[permission-relinq]: https://wiki.sei.cmu.edu/confluence/display/c/POS37-C.+Ensure+that+privilege+relinquishment+is+successful

One of the SEI guidelines is an important part of secure
programming, independent of its application to setuid programs:
[*always* check the return value][check-ret] of any C function that can
fail. If you don't check the return value from such a function, then
it may not have done what you expected, putting your program into
an unknown and potentially unsafe state.

[check-ret]: https://wiki.sei.cmu.edu/confluence/display/c/EXP12-C.+Do+not+ignore+values+returned+by+functions


### 1.1 File permissions and the POLP

One aspect of the Unix access control system can come in handy when
trying to apply the Principle of Least Privilege:

- File permissions are checked when a file is *opened*, not when an open
  file is used.

In fact, once you have obtained a descriptor (the more general
term is "file handle") to an open file
on Unix systems, you can generally continue to read or write via that
file descriptor regardless of what happens to the original file -- the
file may be renamed, have its permissions changed, or even be deleted,
and it won't affect your access to the file contents.

This is partly a side effect of the way filesystems work on Unix: on
Unix systems, there's a structure called an *inode* which you can think
of as being an intermediary between a file path and the file content.
The inode specifies things like the file owner and permissions, and
"points to" a set of blocks on disk which is the file content.  Multiple
file paths can point to the same inode (they are called "hard links");
deleting a file path deletes its directory entry, but the inode still
exists (as does the file content) as long as at least one directory
name or open file-handle still points to that inode.

`<div style="display: flex; justify-content: center;">`{=html}

![inodes in a Unix file system](images/inodes.svg ""){ width=80% }

`</div>`{=html}

Let's demonstrate that this is the case.

1.  Compile the following program, `keep_open.c`:[^compiling] [^safety]

    ```{.c .numberLines}
    // keep_open.c

    #include <stdlib.h>
    #include <stdio.h>

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fcntl.h>

    #define BUF_SIZE 1024

    char buf[BUF_SIZE];

    void fail(const char* mesg) {
      perror(mesg);
      exit(EXIT_FAILURE);
    }

    int main(int argc, char **argv) {
      argc--;
      argv++;

      if (argc != 1) {
        fprintf(stderr, "expected 1 arg, FILENAME\n");
        exit(EXIT_FAILURE);
      }

      printf("opening file\n");
      int fd = open(argv[0], O_RDWR);
      if (fd == -1)
        fail("couldn't open file");

      printf("running 'tail'\n");
      system("tail -f /dev/null");

      // read up to a buffer's worth
      ssize_t read_res = read(fd, buf, BUF_SIZE);
      if (read_res == -1)
        fail("couldn't open file");

      printf("contents read: %s\n", buf);

      close(fd);
    }
    ```

    This program opens a file specified on the command line (line 28);
    it then keeps it open, and calls `system()` to run the command
    "`tail --follow /dev/null`". This invocation of "tail" won't exit until
    the `tail` process is killed -- it tries to continually wait for new
    input ("`--follow`") from a file that has nothing in it ("`/dev/null`").
    (Using `tail` in this fashion is a common way of keeping a program
    running that would otherwise exit.)

[^compiling]: From this point on, we will assume you know how
  to create a new directory to store files for a lab,
  how to create, compile and link a C program using GCC and/or GNU Make,
  and how to pass appropriate compilation flags to GCC.\
  If you are unsure, refer back to the previous labs.

[^safety]: This program does not validate its input, and makes
  use of the `system()` function -- but it's assumed you are
  the only person using the program, and can tolerate the risk.

2.  Create a file called "myfile" using the `dd` program:[^dd]

    ```bash
    echo hello world > myfile
    # append 1GB of zeros to the file - may take a minute to run
    dd status=progress oflag=append conv=notrunc if=/dev/zero bs=1M of=myfile count=1024
    ```

[^dd]: (See `man dd` for details of the `dd` command, which is used for
  copying file content. `oflag=append` tells `dd` to append the data
  it gets from `/dev/zero` to the output file, and `conv=notrunc`
  tells it not to *truncate* the output file when it calls `open()`.

3.  Check your current disk usage, using `df -h .`.

    You should see something like the following output (it may vary
    somewhat depending on what virtualisation software you're using):

    ```bash
    $ df -h .
    Filesystem      Size  Used Avail Use% Mounted on
    /dev/vda3       124G  5.4G  111G   5% /
    ```

    This tells us that the filesystem has a capacity of 124GB, and
    that 5.4GB worth of files already exist (1GB of which will be
    the file we just created).

4.  Run your compiled program:

    ```
    $ ./keep_open
    opening file
    running 'tail'

    ```

    The program should then "hang" -- this is expected. Open a new
    terminal window and/or start a new SSH session to your VM in
    order to complete the next steps -- ideally, keep an eye also
    on what is happening in the original terminal window.

5.  Change the ownership of `myfile` to root, and allow only root
    to read or write to it:

    ```
    $ sudo chown root:root myfile
    $ sudo chmod g-rwx,o-rwx myfile
    $ ls -al myfile
    -rw------- 1 root    root    2147483660 Aug 20 10:42 myfile
    ```

    And delete it:

    ```
    $ sudo rm myfile
    ```

    The file should now be completely inaccessible (outside of the
    use of disk forensic techniques) -- once we ran `chmod`, no-one but
    `root` could read
    or write to it, and in any case the file is deleted.

    But if you check how much disk space is being used, you'll see
    that despite deleting the file, disk usage hasn't changed:

    ```
    $ df -h .
    Filesystem      Size  Used Avail Use% Mounted on
    /dev/vda3       124G  5.4G  111G   5% /
    ```

6.  Now, we'll kill the `tail` command that is running:

    ```
    $ pkill -f 'tail -f /dev/null'
    ```

    In the terminal where your `keep_open` program was running, you
    should now see the following:

    ```
    opening file
    running 'tail'
    contents read: hello world

    ```

    The program already had a "handle" to the inode where `myfile`'s
    metadata was stored, and had no difficulty reading a line from the
    file, despite the permissions having been changed and the file
    deleted.

    If we *now* check the disk space on our filesystem:

    ```
    $ df -h .
    Filesystem      Size  Used Avail Use% Mounted on
    /dev/vda3       124G  4.4G  111G   4% /
    ```

    then we will see it has decreased by 1GB (from 5.4GB to 4.4GB, in
    the example above). Once the open file-handle was closed, the
    kernel discovered that the inode for the file was unused --
    no other programs had it open, and no directory entries "pointed"
    to it.

    Therefore, the inode was removed, and the disk blocks used by it
    were reclaimed.

#### Consequences for software security

What are the consequences of all this for software security?
Several things:

*Permissions are only needed (and checked) at `open` time*

:   We need appropriate permissions to open a file, but once it's
    been opened, no permissions are needed to read or write to
    the open file via its file descriptor.

    (In fact, the file descriptor acts as a sort of *capability* --
    we can actually pass it from program to program, and it carries
    with it the "rights" to read and write from the open
    file.[^passing-fd])

    So for a setuid program: if the only reason we needed elevated
    privileges was to open a file for reading or writing, then once
    the file is open -- we can drop the privileges.

\

*Permission changes can't be retrospective*

:   Any changes you make to a file's permissions will have
    no effect on programs that already have the file open
    (and if they already have the file open, they may
    have *exfiltrated* the data in it -- sent it to an
    attacker-controlled system).

    You can find out what programs have a file open using
    the `lsof` ("list open files") program, but can't "retract"
    their permissions to use their open files -- the best you
    can do is kill the process.\

\

*File paths are unreliable -- do not trust them*

:   The *path* to a file is not a very good way of identifying it
    reliably and uniquely over a period of time. The *inode*
    is the best representation of what we think of as "the file",
    and a file descriptor gives us a "handle" to that inode.

    Consider the following scenario.
    You have a root-owned, `setuid` program -- call it PDFizer --
    running as a server, which is intended
    to typeset and convert the contents of files to PDF when users send a
    request for it to do so -- but only *if* that user would have
    had permissions to read the original file.

    The PDFizer program needs to run as root, because otherwise
    it wouldn't be able to access files owned by different users.

    Suppose our PDFizer program receives a request from user `bob`, who wants
    to typeset the file `/home/bob/myfile.txt`.

    And let us suppose our program implements the following
    logic:

    1. Look at `/home/bob/myfile.txt` and see whether `bob` has
       permission to read it -- we could use the [`stat()` function][stat]
       to do this (the information we're after is in the `st_mode`
       struct member).
    2. If so, `open()` the file `/home/bob/myfile.txt`, convert it
       to PDF, and send the result to `bob`.

    Assuming that the path `/home/bob/myfile.txt` represents the
    same file during steps (1) and steps (2) is a *bad* assumption.

    In between steps one and two, Bob could have deleted `/home/bob/myfile.txt`
    and replaced it with a symbolic link to `/etc/shadow` (which
    contains users' passwords). Since our PDFizer program is executing
    as root, it will have no difficulty executing step 2, opening a file
    which Bob should not have access to, and sending it to Bob as a PDF.

    You *cannot rely on a file path pointing to the same "file" at
    two different times*. Read the recommendation of the Software
    Engineering Institute (SEI) about this:
    ["FIO01-C. Be careful using functions that use file names for
    identification"][sei-fname].

    This sort of vulnerability is called a "TOCTOU" ("Time of check
    vs time of use") vulnerability -- between steps (1) and (2)
    is a time window that attackers can take advantage of.

    If you need secure and reliable access to a file, then the
    standard Unix approach is to open the file *once*
    (giving you a file pointer or file descriptor that
    links to the file's inode), and then
    to perform all actions (reading, writing, checking file
    permissions) on that file "handle".

    Instead of using the function `stat()` in step 1
    (which takes as argument a file name), we should have
    opened the file *first*, obtaining a file descriptor,
    and then called `fstat()`
    (which takes as argument a file descriptor) to check
    the user's permissions.

    <div style="border: solid 2pt orange; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

    <center>**Secure coding practices**</center>

    In the project for CITS3007, it will be up to *you* to ensure you
    follow good secure coding practices -- dropping privileges when
    appropriate, calling `fstat()` instead of `stat()`, and checking
    the return values of functions that can fail -- and following these
    practices will comprise a significant proportion of your mark.

    Static analysis programs and bug-finders may identify some of these
    problems (and it will be up to you to use them appropriately), but
    not all.

    The best way to ensure you remember good secure coding practices
    while completing the project is to

    1. practice them beforehand -- write code that does and doesn't
       follow a particular secure coding practice
    2. take notes when you see a practice mentioned, and do a [*code
       review*][code-review] of your project code before submitting
       to make sure you followed them all.

    (Ideally, a code review should be performed by someone other than
    the original developer. There is still benefit, however, to
    reviewing your own code. It's a good idea to (1) wait a while
    between working on code and reviewing it, and (2) don't review the code
    using the same display device your wrote it on. Instead, print it
    out, or try reading it from a tablet instead of a PC. Otherwise,
    there's a strong tendency to "see what you expect to see" instead
    of what's actually there.)


    </div>

[code-review]: https://en.wikipedia.org/wiki/Code_review


[stat]: https://linux.die.net/man/2/stat
[sei-fname]: https://wiki.sei.cmu.edu/confluence/display/c/FIO01-C.+Be+careful+using+functions+that+use+file+names+for+identification

[^passing-fd]: There are two main ways file descriptors
  can be passed between programs: (1) a child process
  inherits any un-closed file descriptors from its parent upon a
  `fork()`;
  and (2) file descriptors can be passed using the
  [`sendmesg`][sendmesg] and [`recvmesg`][recvmesg] functions to
  a completely unrelated process.\
  Method (2) is a fairly fiddly process -- a library,
  ["Ancillary"][ancillary] exists which simplifies it.


[sendmesg]: https://linux.die.net/man/2/sendmsg
[recvmesg]: https://linux.die.net/man/3/recvmsg
[ancillary]: http://www.normalesup.org/~george/comp/libancillary/

### 1.2. Relinquishing privileges

In last week's lab, we looked at strategies for applying the
Principle of Least Privilege to setuid programs.
We noted that once privileges have been used for
whatever purpose they were needed, a program should relinquish
them using the `setuid()` function (see `man 2 setuid`).
We also saw that it's easy to make mistakes when relinquishing
privileges -- the SEI's [web page on relinquishing
permissions][permission-relinq] identifies some of the issues.

[permission-relinq]: https://wiki.sei.cmu.edu/confluence/display/c/POS37-C.+Ensure+that+privilege+relinquishment+is+successful

Save the following program as `privileged.c` and compile it.


```{ .c .numberLines }
// privileged.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
  int fd;

  // Assume that /etc/zzz is an important system file,
  // and it is owned by root with permission 0644.
  // Before running this program, you should create
  // the file /etc/zzz first.
  fd = open("/etc/zzz", O_RDWR | O_APPEND);
  if (fd == -1) {
     printf("Cannot open /etc/zzz\n");
     exit(0);
  }

  // Simulate the tasks conducted by the program
  sleep(1);

  // After the task, the root privileges are no longer needed,
  // it's time to relinquish the root privileges permanently.
  setuid(getuid());  // getuid() returns the real uid

  if (fork()) { // In the parent process
    close (fd);
    exit(0);
  } else { // in the child process
    // perform unprivileged tasks

    // Now, assume that the child process is compromised, malicious
    // attackers have injected the following statements
    // into this process

    write (fd, "Malicious Data\n", 15);
    close (fd);
  }
}
```

In the code above, we assume that the file `/etc/zzz` is some
important system file, and that it needs to be protected from
unauthorised tampering (i.e., breaches of integrity). Only
root will have permissions to write to the file.

Our program, `privileged`, will be run by non-root users, but
will be a setuid program, so that it can still amend `/etc/zzz`
when needed. In line 17, it opens the file for reading and writing,
and specifies that any write operations will append to the end
of the file.

It then performs important tasks on the `/etc/zzz` (we simulate these by
calling `sleep()`, at line 24).

Once those tasks are done, we relinquish privileges by calling
`setuid()`, close the file, and spawn a child process to
perform more unprivileged tasks (displaying to the end-user
summaries of what has been done, perhaps).

**Question:**

:   Before even running the program -- can you spot any security
    issues with the code?




Once your program is compiled, create the file `/etc/zzz` and restrict
access to it using `chown` and `chmod`:

```bash
$ sudo touch /etc/zzz
$ sudo chown root:root /etc/zzz
$ sudo chmod u=rw,g=r,o=r /etc/zzz
# or chmod 0644 /etc/zzz would have the same effect
```

What permissions does `/etc/zzz` have once you've done this?



Run the `privileged` executable using your
normal user account, and describe what you have observed. Will the file
`/etc/zzz` be modified? Explain your observation.



Now give the `privileged` program setuid capabilities.
Change the owner of the `privileged` executable to `root`, and enable
the setuid bit:

```
$ sudo chown root:root ./privileged
$ sudo chmod u+s ./privileged
```

Run the program again -- is `/etc/zzz` modified? Was that the program
designer's intent?



### 1.3. Discussion of code

When programs are run which use the `setuid` feature,
there are multiple different sorts of "user ID" at play.

- `rUID` -- the real user ID. This means the ID of the user
  who created the process.
- `rGID` -- the real group ID. This means the group ID of the user
  who created the process.
- `eUID` -- the effective user ID. For many executables,
  this will be the same as the `rUID`. But if an executable has the
  `setuid` feature enabled, then the *effective* user ID
  will be different -- it will be whoever owns the executable
  (often, `root`).
- `eGID` -- the effective group ID. This is similar to `eUID`, but
  for user groups. Programs can have a `setgid` feature enabled,
  and the effective group ID can be different from the real group ID
  if it is enabled.

In the code above, paste the following at various spots in the program
to see what the real and effective user ID are:

```
  uid_t spot1_ruid = getuid();
  uid_t spot1_euid = geteuid();
  printf("at spot1: ruid is: %d\n", spot1_ruid);
  printf("at spot1: euid is: %d\n", spot1_euid);
```

(Change `spot1` to `spot2`, `spot3` etc. in the
other locations you paste the code.)
Re-compile the program, give it appropriate permissions,
and run it again -- what do you observe? Why does this happen?

See if you can fix the issues with the program you identified
earlier. (Hint: read the SEI pages, and look up what `man 2 setuid`
says about return values from the function.) Once you've made
your changes, compile and run the program again, and confirm that
your changes prevent the vulnerability.

As an aside: it's not uncommon for a program that needs special
privileges to "split itself into two" using the `fork` system call.
The parent process retains elevated privileges for as long as it needs,
and sets up a communications channel with the child (for instance,
using a *pipe*, a *socket* or *shared memory* -- more on these later).
The parent process has very limited responsibilities, for instance,
writing to a `root`-owned file as needed, say. The child process handles
all other responsibilities (e.g. interacting with the user).




## 2. Capabilities

Traditionally on Unix-like systems, running processes can be divided
into two categories:

- *privileged* processes, which have an effective user ID of 0
  (that is, `root`)
- *unprivileged* processes -- all others.

Privileged processes bypass any permission checks the kernel would
normally apply (i.e., when checking whether the process has permission
to open or write to a file), but unprivileged processes are subject
to full permission checking.

This is a very coarse-grained, "all or nothing" division, though.
Modern OSs may take a finer-grained approach, in which the ability to
bypass particular permission checks is divided up into units called
*capabilities* (this is just Linux's term for them -- they are not
actually the same as "capabilities" in security theory). For example,
the ability to bypass file permission
checks when reading or writing a file could be one privilege; the
ability run a service on a port below 1024 might be another.

Since version 2.2 of the Linux kernel (released in 1999), Linux possesses
a capabilities system. It is documented
under [`man capabilities`][man-cap], and [this article][linux-cap-art]
provides a good introduction to why capabilities exist and how they
work.

**Question:**

:   What advantages can you see of a finer-grained permissions system
    over the traditional Unix approach? Are there any disadvantages?


[man-cap]: https://man7.org/linux/man-pages/man7/capabilities.7.html
[linux-cap-art]: https://blog.container-solutions.com/linux-capabilities-why-they-exist-and-how-they-work

## 3. Moodle exercises

On Moodle, under the section "Week 5 – sanitization" are some C
programming exercises relating to environment sanitization. If you don't
get a chance to complete these during the lab, you should do so in your
own time.


<br><br>

## 4. Credits

The code for the `privileged.c` program is adapted from
<https://web.ecs.syr.edu/~wedu/seed/Labs/Set-UID/Set-UID.pdf>
and is copyright Wenliang Du, Syracuse University.

<br><br><br>


<!-- vim: syntax=markdown tw=72 :
-->
