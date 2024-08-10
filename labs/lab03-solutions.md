---
title: |
  CITS3007 lab 3 (week 5)&nbsp;--&nbsp;Permissions and setuid programs&nbsp;--&nbsp;solutions
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



<div class="solutions">

::: block-caption

Sample solutions

:::

The commands all have their `setuid` bit set. This is needed
because:

- For `chsh`, it changes a user's login shell, and that
  information is stored in `/etc/passwd`, which is only
  writeable by root.
  (In other words -- the reason is much the same as for `passwd`.)

or,

- For `su` and `sudo`: they allow commands to be run with `root` permissions.


</div>



For each of these commands, copy the executable to your working
directory using the `cp` command, and run `ls -al` on the copy. What permissions does the copy
have? Try running the copied `sudo` and `chsh` commands (give `chsh`
the password `vagrant`, and suggest `/bin/sh` as a new shell). What
happens?

You can add the `setuid` feature to the copied programs by running the command
`chmod u+s ./SOME_PROGRAM` (inserting the name of the copied program
as appropriate). However, the programs will still be owned by you --
what happens if you try to run them?



<div class="solutions">

::: block-caption

Sample solutions

:::

If you copy one of the `setuid` programs, the copy no longer has its `setuid` bit set.
By default, the `cp` command assumes we wish to copy the file *content*, but not file properties like
whether the `setuid` bit is set.[^copy-setuid]

[^copy-setuid]: The logic of the `cp` command (at least, the GNU implementation of the
  command which we are using -- see [here][gnu-cp-src] for the source code) is not always
  very intuitive.\
  &nbsp; &nbsp; As noted above, by default, `cp` doesn't copy file metadata at all. If you add the
  `--preserve` flag, it'll *usually* copy the file metadata (including the `setuid` bit) --
  but if the source file is owned by a different user from you, it won't copy the `setuid`
  bit. (It *is* possible to persuade the `cp` command to do so, though it doesn't seem to be
  well-documented -- as a challenge you might like to find out how.) \
  <!-- answer: you need to add `preserve=mode` as a flag. This is the case *even* if you
  have already added `preserve=all`. Why? who knows.
     The "GNU coreutils decoded" program attempts to investigate the structure of coreutils
  programs somewhat (https://www.maizure.org/projects/decoded-gnu-coreutils/index.html),
  but the coding style is pretty impenetrable.
     See comments on the coreutils source at <https://news.ycombinator.com/item?id=30127084>
  -->
  &nbsp; &nbsp; As a result, for purposes like backing up a filesystem, we often instead
  use programs with better-documented and more intuitive behaviour, like [tar][tar] and
  [rsync][rsync].


[gnu-cp-src]: https://github.com/coreutils/coreutils/blob/c07a7d999e86e17ad5d4ea09a3fc8e8b1ac259f7/src/cp.c
[tar]: https://man7.org/linux/man-pages/man1/tar.1.html
[rsync]: https://linux.die.net/man/1/rsync

Even if you *do* set the `setuid` bit on the files, using `chmod`, though --
due to the way they have been coded, none of the programs will function properly without
**both** (a) having the `setuid` bit set, and (b) being owned by `root`.

This is a safety precaution built into the programs: if the programs detect they are being
used in a way that isn't intended, they abort execution.

</div>




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



<div class="solutions">

::: block-caption

Sample solution

:::

This is not in line with best practice.

Our `hr_db_amend` program will run as `root`, when it could be given far fewer privileges,
which would lessen the impact if we happen to make a mistake when coding it.

A better approach would be to create a user called `hr`, and have that user ID own the
database file `/var/hr/hr.db` and the `hr_db_amend` executable.

We could still make our program setuid; but now it would run as user `hr`, giving it access
to the HR database, but not to important system configuration files like `/etc/passwd` and
`/etc/shadow`.

An example of a Unix program which creates specialized user accounts, just for the purpose of
reading or writing files owned by those accounts, is [crontab][crontab], part of the
["cron"][cron] software package.

[docker]: https://manpages.org/docker
[cron]: https://en.wikipedia.org/wiki/Cron
[crontab]: https://linux.die.net/man/1/crontab
[containers]: https://www.redhat.com/en/topics/containers#overview

Crontab allows users to create recurring "tasks" which will be run periodically on a Unix
system. The files defining these tasks are stored in the directory
`/var/spool/cron/crontabs`. The Crontab program needs to meet the following requirements:

- Individual non-root users should be able to create configuration files in the
  `/var/spool/cron/crontabs` directory; BUT
- Individual users should also have the integrity and confidentiality of their files protected --
  which means we can't simply make `/var/spool/cron/crontabs` world-readable and writable.

The solution is to make the `/var/spool/cron/crontabs` directory group-owned by a group
called `crontab`, and for the `/usr/bin/crontab` program to be a `setgid` program and be also
group-owned by `crontab`.[^cron-setgid-innovation] (`setgid` is like `setuid`, but it means a program runs with
different *group* permissions, instead of different *user* permissions.)

By listing the details of `/var/spool/cron/crontabs` and `/usr/bin/crontab`, you should be
able to confirm that this is the case.

[^cron-setgid-innovation]: The version of cron being used in the CITS3007 SDE is called
  ["Vixie cron"][vixie], and is widely used on Unix systems.
  The use of `setgid` isn't part of the original "Vixie cron" source code, however -- it
  was introduced as an improvement to the original Vixie cron in the
  [Debian distribution of Linux in 2003][debian-cron-setgid].

[debian-cron-setgid]: https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=18333

<!--

patch source is here:
<https://sources.debian.org/src/cron/3.0pl1-162/debian/patches/features/Make-crontab-SGID-crontab.patch/>

notes on setgid in vixie cron

The request for the setgid feature is here:
<https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=18333>.

In that report, Topi Miettinen <Topi.Miettinen@medialab.sonera.net> points out that
security could be improved by using one of the approach outlined in lectures --
splitting cron into critical parts which need to be setuid or setgid, and other
parts which could communicate with them via a socket:

> With 2.1 series kernels, even setgid can be dropped: crontab connects to
> cron daemon using a unix socket, daemon can check client access rights with
> getsockopt(SO_PEERCRED).

However, this suggestion did not end up being implemented.

-->

<!--

vulnerabilities in vixie cron:

Vixie cron is also subject to a vulnerability caused by failing to check the result
of setuid() - exactly the problem we warn students about.

See <https://nvd.nist.gov/vuln/detail/CVE-2006-2607>


-->

Because of the security vulnerabilities often introduced by `setuid` and `setgid` programs,
a blog post by Konstantin Ivanov [here][remove-setuid] suggests removing (or partly
disabling) many of the less commonly used `setuid` and `setgid` programs --
they introduce potential vulnerabilities for little advantage.

[remove-setuid]: http://www.linux-admins.net/2010/09/finding-and-eliminating-setuidsetgid.html


</div>



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


<div class="solutions">

::: block-caption

Sample solutions

:::

The `getuid()` and `geteuid()` functions cannot fail, but `setresuid()` can.

This is documented in the manual pages for those functions.


</div>


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



<div class="solutions">

::: block-caption

Sample solution

:::

It does contain security flaws. The `setuid` function (or `seteuid`, a variant of it found
on many Unix-like operating systems) can fail, and if it fails, then we *haven't
successfully relinquished privileges*.
In case of a failure, we must immediately abort execution -- otherwise, we'll be performing
actions which where intended to be performed by a non-privileged user, but will be
performing them as root. This is highly prone to being exploited by attackers.

We should check the return value from `setuid`, and if it is non-zero, we should abort.

(In fact, we should [*always* check the return value from any
function call that can fail][check-ret]. All our subsequent code has presumably been written
on the assumption that the function call succeeded; if it didn't, then our assumption is
wrong, and we have no idea what the actual current state of the system is.)

[check-ret]: https://wiki.sei.cmu.edu/confluence/display/c/EXP12-C.+Do+not+ignore+values+returned+by+functions


</div>




We will experiment further with `setuid` programs, and code for using and relinquishing
privileges, in future labs.

## 2. Moodle exercises

On [Moodle](https://quiz.jinhong.org), under the section "Week 4 – string handling", is a
set of exercises on using C's string handling functions to write a safe "path-construction"
function.

Complete the exercises, using your development environment to help test your code. If you
aren't able to complete the exercises during your scheduled lab time, you should work
through them in your own time.



<div class="solutions">

::: block-caption

Sample solution

:::

The following is a sample solution for the `would_wrap_around()`
function:

``` {.C .numberLines}
/** Return true when the sum of dirlen, filelen, extlen and 3
 * would exceed the maximum possible value of a size_t; otherwise,
 * return false.
 */
bool would_wrap_around(size_t dirlen, size_t filelen, size_t extlen) {
  size_t res = dirlen;
  res += filelen;
  if (res < dirlen || res < filelen)
    return true;
  size_t old_res = res;
  res += extlen;
  if (res < old_res || res < extlen)
    return true;
  old_res = res;
  res += 3;
  if (res < old_res || res < extlen)
    return true;
  return false;
}
```

Comments on the code:

- This can be simplified somewhat. In line 8, for instance, we
  check to see if the result of `dirlen + filelen` is less than
  `dirlen` or less than `filelen`. But actually, it suffices to see
  if the result is less than `dirlen` alone. (Can you see why?)



The following is a sample solution for the `make_pathname()`
function:

``` {.C .numberLines}
char* make_pathname(const char *dir, const char *fname, const char *ext) {
  size_t dirlen = strlen(dir);
  size_t filelen = strlen(fname);
  size_t extlen = strlen(ext);

  if (would_wrap_around(dirlen, filelen, extlen)
    return NULL;

  // include terminating NUL
  size_t pathbuf_size = dirlen + filelen + extlen + 3;

  char *path = malloc (pathbuf_size);
  if (!path)
      return NULL;

  memcpy (path, dir, dirlen);
  path[dirlen] = '/';

  memcpy (path + dirlen + 1, fname, filelen);
  path[dirlen + filelen + 1] = '.';

  memcpy(path + dirlen + filelen + 2, ext, extlen + 1);

  return path;
}
```

Some comments on the code:

- When working on a C project, it can be helpful if your development team has a convention
  for distinguishing *lengths of strings* (which don't include in their count the
  terminating `NUL`) from *sizes of buffers* (which always *should* include in their count
  any required `NUL` characters).

  This can help make code reviews easier, and ensure mistakes aren't made by using one sort
  of length where the other would be appropriate. (Even better would be to configure your
  team's static analysis programs to distinguish the two, and enforce rules about how each
  is used.)

  The code above uses names like `dirlen`, `filelen` etc for string lengths, but
  `pathbuf_size` for a buffer.

- When you submit C code as part of an assessment, your code will be assessed not only for
  correctness, but for style and clarity.

  This means making appropriate use of C library functions.  The following are examples of
  solutions which *don't* make appropriate use of C library functions:

  a.  Solutions which don't make use of C library functions at all --
      for instance, using a `for` loop instead of `memcpy` to copy
      characters from the arguments to `path`:

      ```C
        for(size_t i = 0; i < dirlen; i++){
                path[i] = dir[i];
        }
      ```

      Writing a `for` loop makes the solution needlessly complex, harder to read than
      necessary, and more prone to error.  If a known number of bytes need to be copied from
      one location in memory to another, then `memcpy` is the standard C idiom for doing so.

  b.  Solutions which use `strcat` to build up the `path` instead of `memcpy`.

      For instance:

      ```C
      strcpy(path, dir);
      strcat(path, "/");
      strcat(path, fname);
      strcat(path, ".");
      strcat(path, ext);
      ```

      Although less problematic than the code in point (a), this is needlessly inefficient.
      Each call to `strcat` will result in `path` being iterated over again.

  c.  Solutions which use `snprintf` to write into `path` -- these are creating additional
      points of failure in the function.  They are discussed further below.


**Calling `make_pathname`**

We could write a `main` routine which allows us to
invoke `make_pathname` from the command-line, as follows:

```{ .c .numberLines }
int main(int argc, char **argv) {
  // skip program name
  argc--;
  argv++;

  if (argc != 3) {
    fprintf(stderr, "Expected 3 args\n");
    exit(EXIT_FAILURE);
  }

  // See note: poor validation!
  char* dir = argv[0];
  char* fname = argv[1];
  char* ext = argv[2];

  char* path = make_pathname(dir, fname, ext);

  if (path != NULL) {
    printf("path is: '%s'\n", path);
    free(path);
  }
  else
    printf("couldn't allocate enough memory\n");

  return EXIT_SUCCESS;
}
```

Some comments on the code:

- In lines 12--14, our program is taking input from a potentially untrusted
  user via the elements of the argv array.

  We can be sure that those elements *are* valid null-terminated strings --
  they aren't `NULL` pointers, nor do they run off past the end of the
  memory segment -- because the language standard assures us so.

  But we *can't* be sure they satisfy the
  other preconditions of `make_pathname` (e.g. the file and extension not
  containing slashes or dots).

  So that will mean that when we call `make_pathname`, we could be violating
  its preconditions, and the returned string might not be what we
  expect and might not be safe to use.

  Furthermore, if we pass that string on to other functions, we can render
  ourselves vulnerable to a *path traversal* attack (see
  <https://owasp.org/www-community/attacks/Path_Traversal>).

  For instance, suppose the `dir` argument is known to be a
  (system-controlled, trusted) path to a directory where web pages are
  stored ("`/path/to/webdir`", say), and the `fname` and `ext` arguments
  are taken from an (untrusted) browser request and specify an HTML file
  to serve up.

  If we haven't validated the contents of the `fname` and `ext`
  arguments, an attacker could set them to `"."` and `"/"`,
  respectively.  Then the result of `make_pathname` would be
  "`/path/to/webdir/../`", which represents the *parent* directory of
  `webdir`.

  We have thus potentially given the attacker the ability to read files
  from outside the directory they should have access to, which could
  include configuration files and other sensitive data.

  We will look in more detail at input validation and path traversal
  attacks in future labs.

An alternative implementation of `make_pathname`:

```{.c .numberLines }
char* make_pathname(const char* dir, const char* fname, const char* ext) {
  if (would_wrap_around(dirlen, filelen, extlen)
    return NULL;

  size_t totalLength = strlen(dir) + strlen(fname) + strlen(ext) + 3;
  // +3 for '/', '.', and '\0'

  char* result = malloc(totalLength);
  if (result == NULL)
    return NULL;

  int written = snprintf(result, totalLength, "%s/%s.%s", dir, fname, ext);
  if (written != totalLength - 1) {
    free(result);
    return NULL; // error occurred
  }

  return result;
}
```

Comments on this implementation:

- This implementation uses `snprintf` -- many C programmers would regard
  `memcpy` and array-element assignment as the simpler solution, though,
  because when called correctly, `memcpy` *can't* fail.
  (`memcpy` is basically just a `for` loop which copies from destination
  to source.)

  `snprintf` on the other hand is a complicated function. There's no
  obvious reason why it *should* fail, if our logic is correct; but
  nevertheless, we can only be certain that it worked if the number of
  bytes written was exactly equal to what we expected (`totalLength -
  1`). So we've added in an additional possibility for failure that wasn't
  in the original specification -- the spec said we would *only* return
  `NULL` if it wasn't possible to allocate enough memory, but now we're
  opening up the possibility for other causes of error.

  (Technically, if we're failing for some reason other than "can't
  allocate enough memory", we should probably `abort()` rather than
  return `NULL`, because we can't fulfil the promise we made in the
  function specification.)



</div>





## 3. Challenge question

(Challenge questions in the lab worksheets are aimed at students
who already have a good knowledge of C and operating systems --
they are not compulsory to complete.)

On Linux, is it possible to write a setuid program in Python? Why
or why not? Is it advisable?




<div class="solutions">

::: block-caption

Sample solution

:::

In current versions of Linux (since at least kernel version 3.0), it's
not (straightforwardly) possible for a Python program to be setuid.
You can try it by writing the following Python program, and making it
owned by root, executable, and with its setuid bit enabled:


```{ .python .numberLines }
#!/usr/bin/env python3

import os

euid = os.geteuid()
print("euid is:", euid)
```

When run, this script will simply print out your normal user ID, not 0.

Why is this? One reason is that the file is a script, and isn't being
*executed* in the same way as a binary executable. When asked to execute
a program, the kernel inspects the start of a file to determine
what *sort* of program it is being asked to run.[^search-binary]

[^search-binary]: This is done by the function
  [`search_binary_handler`][search-binary-handler-src] in the kernel.
  You can find more discussion of the process [here][howprog],
  [here][shebangs], [here][execve-gitbook] and [here][modprobe-path].

[search-binary-handler-src]: https://github.com/torvalds/linux/blob/a785fd28d31f76d50004712b6e0b409d5a8239d8/fs/exec.c#L1716
[howprog]: https://lwn.net/Articles/630727/
[execve-gitbook]: https://0xax.gitbooks.io/linux-insides/content/SysCall/linux-syscall-4.html#execve-system-call
[modprobe-path]: https://github.com/smallkirby/kernelpwn/blob/master/technique/modprobe_path.md#determine-binary-format
[shebangs]: https://deardevices.com/2018/03/04/linux-shebang-insights/

The first 4 bytes of a binary executable on Linux will be `"\0x7fELF"`,
indicating that it's an [ELF-format executable][wiki-elf], but scripts
will start with the characters `"#!"`, indicating that they are intended
to be supplied to an *interpreter* to be run -- in the case of Python scripts,
the interpreter will end up being `/usr/bin/python3`.

[wiki-elf]: https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

So for Python scripts, the machine-code instructions which are loaded into
memory and executed by the processor don't come from the script
(they couldn't -- the script contains plain text, not machine
code),
but from the Python interpreter binary executable at `/usr/bin/python3`.
That file doesn't have
its setuid bit enabled, so it doesn't run as root. (The script file
is just a data file which the Python interpreter opens, reads, and
interprets as Python instructions.)

You could deliberately *set* the Python interpreter to be a setuid
program, if you wanted; but that would be very unwise, as now all Python
programs would run as root.

Is there any other way one could run a Python script as a setuid
program? Yes, there is: you could create a custom interpreter
designed to run Python scripts that have their setuid bit enabled.
Your interpreter would need to be owned by root and be a setuid
program itself, and scripts intended to be run by it would start
with `#!/path/to/my/custom-interpreter`.

The custom interpreter would need to

a. inspect the script file it is being asked to run, and check whether
   it has the setuid bit enabled.
b. if not, then privileges should be dropped; otherwise, use `seteuid`
   to assume the privileges of the owner of the script.
c. use the `execve` system call (see `man execve`) to start the normal
   interpreter (e.g. `/usr/bin/python3`, for Python scripts) running,
   with the script supplied to it as an argument.

The custom interpreter would also need to be carefully coded to avoid
the possibility of
[injection attacks][injection-attacks] and a wide range of other
problems. Examples of custom interpreters which aim to do this
can be found [here][selliott] and [here][setuid-on-shell].
(Note: the safety of the code in these custom interpreters
has not been vetted – use of it is at your own risk.)

[injection-attacks]: https://owasp.org/www-community/Injection_Theory

[selliott]: https://selliott.org/python/
[setuid-on-shell]: http://web.archive.org/web/20080703190734/https://www.tuxation.com/setuid-on-shell-scripts.html#primarycontent

Would a custom interpreter like this be a good idea? It's doubtful.
Programs like the Python interpreter are very large and complex, were
not designed
with running at elevated privileges in mind, and contain innumerable
places in their code where security vulnerabilities could lurk.

Programs written in C have the disadvantage of not being
memory-safe;[^mem-safe-system-prog]
but (if crafted carefully) they have the advantage that they can be kept extremely
small and simple,
with a very limited number of locations which need to be reviewed for
security issues.

[^mem-safe-system-prog]: A number of systems programming languages
  have been developed which have better memory safety than C -- for instance,
  [Cyclone][cyclone], [ATS][ats], and [Rust][rust]. As yet, however, none
  of them have completely supplanted the role of C and
  C<span style="font-family: monospace">++</span> in systems
  programming.\
  (If you have an interest in memory-safe systems programming
  languages, then this [blog post][cyclone-influence] by Jonathan Goodwin
  examining the influence of Cyclone on later languages,
  including Rust, may be of interest.)

[cyclone]: http://trevorjim.com/unfrozen-cyclone/
[ats]: https://www.cs.bu.edu/~hwxi/atslangweb/
[rust]: https://www.rust-lang.org
[cyclone-influence]: https://pling.jondgoodwin.com/post/cyclone/

</div>





<!-- vim: syntax=markdown tw=92 :
-->
