---
title: |
  CITS3007 lab 3 (week 5)&nbsp;--&nbsp;Permissions and setuid programs
---

It's recommended you complete this lab in pairs, if possible, and
discuss your results with your partner.


## 1. `setuid` and Unix permissions

This section and the following ones introduce the Unix
access control system. This includes:

- traditional Unix file access mechanisms;
- `setuid` permissions; and
- Linux capabilities.

(You may want to refer back to this lab worksheet in future labs, when we look at how
`setuid` programs can be exploited.)

The `setuid` ("set user identity") facility allows normal, non-root users to run a program
*as if* it were being run by another user. This allows users to do things like change their
password. We know that hashes of passwords are stored in the `/etc/shadow` file,[^hash-and-salt] which is
only readable and writeable by `root`. (Run `ls -al /etc/shadow` to
see the file's permissions -- what are they?)

[^hash-and-salt]: To be more precise, what's stored in `/etc/shadow` is
  a [hashed and salted][shadow-file] version of the password.

[shadow-file]: https://www.cyberciti.biz/faq/understanding-etcshadow-file/


A non-root user can run the command `passwd` to change their password -- so how does this
work, if that user cannot read or write the `/etc/shadow` file?

The answer is that `passwd` is a `setuid` program, and when a `setuid` program is run, it
assumes the privileges of the *owner of the executable file*, rather than (as is usually the
case) the privileges of the user executing the command.
Specifically, since the `/usr/bin/passwd` executable is owned by `root`, that means that
when a normal user runs that executable, it will run with the root user's permissions.

![](images/setuid-passwd.svg){ width=100% }

Let's take a look at the permissions of the `passwd` executable by running the following:

```
$ ls -al `which passwd`
```

You should see something like this:

```
-rwsr-xr-x 1 root root 68208 Mar 14 16:26 /usr/bin/passwd
```

The "`s`" in the first 3 characters of the permissions indicates
that this program has the `setuid` feature enabled.



An example of a *non*-`setuid` program, on the other hand, is `less` (and most other
programs you commonly run). We'll confirm that if we try to use `less` to read a file that
only `root` can read, it won't work. Try running the following commands:

```
$ ls -al /etc/sudoers
$ less /etc/sudoers
```

The output of the `ls` command should look something like this:

```
-r--r----- 1 root root 798 Mar 16 15:22 /etc/sudoers
```

Like the `/etc/shadow` file, the `/etc/sudoers` file contains critical operating system
information, and thus can't be read or written to by most users.
(It is a configuration file used by the `sudo` command, which can be used to
temporarily run *any* command as `root`.)
In fact, as the file permissions show, only the `root` user can read the file, so when you try to
read it with `less`, you get a permission error.[^writing-to-sudoers]

[^writing-to-sudoers]: If you look at the listing of `/etc/sudoers` carefully,
  you may notice not even **`root`** has write permissions to
  write to the `/etc/sudoers` file! So you might ask: how can can the file ever be modified?
  The answer is that on Unix systems, whenever the access control system asks
  "Can user X perform action Y?" -- if the user is `root`, the answer is always "yes", no matter
  what the action is. \
  &nbsp; &nbsp; In other words, `root` can basically do anything, no matter what the file permissions
  say. Try this: create a root-owned file by running `sudo touch myfile`, then `chmod 000
  myfile`, then `ls -al myfile`. You should see that no user can read or write `myfile`. \
  &nbsp; &nbsp;  But then run the command `echo hello | sudo tee -a myfile`. (We use `tee -a` to *append* text to
  the file.) \
  &nbsp; &nbsp; If you inspect the contents of `myfile` (e.g. by using `sudo less`), you'll
  see we *did* successfully append text to it, even though `root` didn't have write
  permissions to the file, and we were able to read from it, even though `root` didn't have
  write permissions. So in that case -- why ever set specific permissions for `root`-owned
  files at all? The answer is that we could just leave those permissions "blank" if we
  wanted -- it's just convention to set them up as we do.





<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 2rem;">

::: block-caption

How to read the output of the `ls` command

:::


![](images/unix_permissions.svg){ width=100% }

The `man ls` documentation unfortunately doesn't fully explain the
listing format used by `ls` -- see the documentation of the
[GNU coreutils package][coreutils] for a fuller explanation, or [this guide][privs] for a
short tutorial.

In brief: the first group of 10 characters (starting with "`-r`")
indicates who can access the file, and how they can access it. Every
file on a Unix-like system has associated with it a set of *flags*
(binary options), and the last 9 characters in that group show what they
are. The meaning of the characters is:

[coreutils]: https://www.gnu.org/software/coreutils/manual/html_node/What-information-is-listed.html

- The first character isn't a flag -- it indicates the type of file.
  A directory is shown as "`d`", and a symbolic link as "l". Other file
  types are shown using other characters, as outlined in the GNU
  coreutils documentation.

- The remaining 9 characters should be read in groups of three.
  The first group indicates permissions available to the file owner,
  the second, permissions available to the file group, and the third,
  permissions available to everyone else.

  If someone has read, write and execute permissions, the group of three
  will be the characters "`rwx`"'; if they only have a subset of those
  permissions, some of the characters will be replaced by a hyphen.

  Thus, a file where the user has read, write and execute permissions,
  but everyone else only has read permissions, will look like this:
  `-rwxr--r--`.

  For some programs, the "`x`" in the first group may be replaced by
  "`s`", as we shall see -- this tells the operating system
  that it has the "`setuid`" feature enabled, and when run, its
  effective permissions are those of the *owner* of the file (rather
  than of the user who started the process).


</div>




[privs]: https://devconnected.com/linux-file-permissions-complete-guide/

Programs that use features
like `setuid` are essential on Unix systems, but also can be very dangerous: if they are
not carefully written, their elevated privileges can have unexpected
consequences. For instance, consider a normal program being run by a non-root user. The
program might contain a vulnerability which allows that user's confidential data to be read
by a third party.
This obviously isn't ideal, but is at least restricted to the user who invoked the program.
If a `setuid` program contains a similar vulnerability, however, then it could potentially
allow *all* users' confidential data to be read, because when run the program will have
`root`'s permissions.

### 1.1. Some `setuid` programs

Consider the following commands:

- `"chsh"`
- `"su"`
- `"sudo"`

What do they do? (Read the `man` pages for the commands for details.) How can you find out
where the executable files for those commands are located? (Hint: check out `man which`.)
List the permissions for the executables using `ls -al`. What do they
have in common? Can you explain, in each case, why those permissions would be needed?



For each of these commands, copy the executable to your working
directory using the `cp` command, and run `ls -al` on the copy. What permissions does the copy
have? Try running the copied `sudo` and `chsh` commands (give `chsh`
the password `vagrant`, and suggest `/bin/sh` as a new shell). What
happens?

You can add the `setuid` feature to the copied programs by running the command
`chmod u+s ./SOME_PROGRAM` (inserting the name of the copied program
as appropriate). However, the programs will still be owned by you --
what happens if you try to run them?




### 1.2 Principle of least privilege


When a program needs to have the `setuid` feature enabled, it is best practice to use the
lowest possible level of elevated privilege, and to use the elevated privilege level for as
short a time as possible.
Once the privileges have been used for whatever purpose they were needed for, the program
should relinquish them -- it does so by calling the `setuid()` function (or one of a number
of closely related functions[^setuid-syscalls]).
You can read about the `setuid()` function at `man 2 setuid` -- but its use is complicated,
and we will elaborate more on it in the next section.

[^setuid-syscalls]: For more information about `setuid`-related functions,
  you should read Chen et al, ["Setuid demystified"][setuid-demyst] (PDF; 11th USENIX
  Security Symposium, 2002), which is part of the recommended readings for week 3. \
  &nbsp; &nbsp; Chen et al point out that Linux has a whole gamut of `setuid`-related
  functions -- `setuid()`, `seteuid()`, `setreuid()`, `setresuid()`, and more -- and it's
  not obvious which one should be used. The authors point out that *one* of those functions
  has much clearer semantics and documentation than the others -- read the paper to find out
  which.

[setuid-demyst]: https://people.eecs.berkeley.edu/~daw/papers/setuid-usenix02.pdf


Relinquishing privileges is especially important for long-running processes.
For instance, a web server might require higher permissions than normal early in the life of
the process (e.g. to read configuration files, or listen for connections on port 80, which
normally only `root` can do)
but should **drop** those permissions once they are no longer needed -- otherwise the
potential exists for them to be exploited.

Using as few privileges as are needed, for as short a time as is needed,
is known as the "Principle of Least Privilege": read
more about it in the *Unix Secure Programming HOWTO*
on [minimizing privileges][secure-howto].

[secure-howto]: https://dwheeler.com/secure-programs/3.012/Secure-Programs-HOWTO/minimize-privileges.html

[**Question:**]{ #hr-db-amend }

:   Suppose we are creating a set of programs which manipulate a human resources database.
    The database file is located at `/var/hr/hr.db` and is owned by root.  One of our
    programs is called `hr_db_amend`, and is a setuid program also owned by root.

    Is this in line with good security practice, as explained in the *Unix Secure
    Programming HOWTO*? If not, what should we do instead?



### 1.3 Effective versus real user ID

Unix systems keep track of two facts about a running process -- the process's "real user ID"
(that is, the user ID of the user who created the process), and its "effective user ID"
(which represents the permissions the process is currently acting with, and might be
different to the real user ID).

For most programs, the two are exactly the same.
For `setuid` programs, however, the operating system sets the real user ID (abbreviated
"rUID") as per usual, but sets the effective user ID (abbreviated "eUID") to the owner of
the executable file being run. (You might think of the "real" user as being the user who
actually invoked the command, and the "effective" user as being the user whose privileges
are currently being used.)

The Linux C functions [`getuid` and `geteuid`][getuid-man] are used to
determine at runtime what the rUID and eUID are. This is how a program
like `su` can tell whether it is being run as a setuid, root-owned
program -- it's eUID should be 0.
(Linux also has something called a ["saved user ID"][saved-uid], used for the
situation where a program *mostly* needs to run as root, but temporarily
needs to run some actions as a non-privileged user. However, we will
leave discussion of it until a later lab.)

[getuid-man]: https://manpages.ubuntu.com/manpages/focal/man2/getuid.2.html
[saved-uid]: https://en.wikipedia.org/wiki/User_identifier#Saved_user_ID

**Exercise**

:   Compile the following program:

    ```{ .C .numberLines }
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <unistd.h>

    int main() {
      uid_t ruid = getuid();
      uid_t euid = geteuid();
      printf("real uid is: %d\n", ruid);
      printf("effective uid is: %d\n", euid);

      return EXIT_SUCCESS;
    }
    ```

    Compare the results it gives when run with the results it gives after you change its
    owner to root and make it a setuid program, and make sure you understand why you see the
    results you do.



### 1.4 Relinquishing privileges

So, we know that we should use elevated privileges for as short a time as possible, and then
relinquish them.  How do we do that?

The Linux C function [`seteuid`][man-seteuid][^or-demyst] lets us change our eUID; to drop privileges,
we just change our eUID so that it's the same as our rUID.

[man-seteuid]: https://manpages.ubuntu.com/manpages/focal/man2/seteuid.2.html

[^or-demyst]: Or one of the other `setuid`-related functions,
  discussed in Chen et al, ["Setuid demystified"][setuid-demyst] (PDF; 11th USENIX
  Security Symposium, 2002).

However, there are some pitfalls to be aware of when relinquishing privileges: read the
Software Engineering Institute's [web page on relinquishing permissions][permission-relinq]
for an explanation of some of the issues.

[permission-relinq]: https://wiki.sei.cmu.edu/confluence/display/c/POS37-C.+Ensure+that+privilege+relinquishment+is+successful

One of the most common pitfalls is *not checking the return value from `setuid`-related
functions that can fail*. Not checking the return values of functions that can fail is bad
practice in *any* C program, but is even more severe for `setuid` programs -- because if
we're not checking return values, then we don't know what privileges our program is
currently operating with, and could easily do something both dangerous and unintended.


**Question:**

:   Can the function `getuid()` fail? What about `geteuid()`? What about `setresuid()`?



**Question:**

:   In our `hr_db_amend` program described [above](#hr-db-amend), we initially run with
    elevated privileges.

    Most users on the system cannot read or write the `/var/hr/hr.db` file, but because we
    are running as the `hr` user, we can.

    Our `hr_db_amend` program contains the following code, executed shortly after the
    program has started running:


    ```{ .c .numberLines}
      // open DB file for reading and writing
      fd = open("/var/hr/hr.db", O_RDWR);
      if (fd == -1) {
         printf("Cannot open /var/hr/hr.db\n");
         exit(EXIT_FAILURE);
      }

      // now that we have a file descriptor,
      // privileges are no longer needed - relinquish them

      setuid(getuid());  // getuid() returns the real uid
    ```

    Does this code contain potential security flaws?  If so, how could it be changed to
    remove them?




We will experiment further with `setuid` programs, and code for using and relinquishing
privileges, in future labs.

## 2. Moodle exercises

On [Moodle](https://quiz.jinhong.org), under the section "Week 4 – string handling", is a
set of exercises on using C's string handling functions to write a safe "path-construction"
function.

Complete the exercises, using your development environment to help test your code. If you
aren't able to complete the exercises during your scheduled lab time, you should work
through them in your own time.





## 3. Challenge question

(Challenge questions in the lab worksheets are aimed at students
who already have a good knowledge of C and operating systems --
they are not compulsory to complete.)

On Linux, is it possible to write a setuid program in Python? Why
or why not? Is it advisable?





<!-- vim: syntax=markdown tw=92 :
-->
