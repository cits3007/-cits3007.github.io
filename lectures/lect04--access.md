---
title: |
  CITS3007 Secure Coding\
  Access control and setuid
author: "Unit coordinator: Arran Stewart"
include-before: |
  ```{=latex}
  %\setbeameroption{hide notes} % Only slides
  %\setbeameroption{show only notes} % Only notes
  \setbeameroption{show notes on second screen=right} % Both
  ```
---


### Highlights

- Authentication and authorization
- Access control
  - What is it, how it helps with our security goals
- Access control lists and capabilities
- `setuid` and `setgid`
- "Confused deputy" and TOCTOU vulnerabilities

### Security goals

On most systems, we have more than one user, and more than
one program.

This makes achieving our "C I A" security goals more complicated.

Two mechanisms that help us are *authentication* and *authorization*.

authentication

:   \
    Verifying who a user is ("Who is this?")


authorization

:   \
    Checking whether a particular user is permitted to perform some
    action ("What can they do?")

### Authentication

Simple (not necessarily most secure) way to implement authentication:
identify every user using a \alert{username} and a \alert{password}.

\break

(What if a user can change their username? Persistent \alert{user IDs},
usually incrementing integers, help solve this.)

### Multi-factor authentication

Stronger authentication relies on multiple methods ("factors") of
proving who you are. Traditionally, two or more of

something you have

:   \
    E.g. a smart card, or USB fob

something you know

:   \
    E.g. a password (or even better, a passphrase)

something you are

:   \
    E.g. your fingerprint, retina scan, or face

::: notes

- no security is absolute
- ways of getting around these?

:::


### Authentication as part of the OS

\small

Authentication provides only very weak guarantees on its own -- it must
be used in concert with other techniques.

::: block

#### Example

A desktop computer or laptop might ask for a username and password, but
not encrypt disk storage used, and not use a secure boot
process.\footnotemark[1] `\\[1em]`{=latex}

That would mean anyone with physical access to the computer could boot
from e.g. a USB thumb-drive, run their own OS, bypass my computer's
normal authentication system, and read (or alter) data on disk.

:::

`\footnotetext[1]{`{=latex}
Usually, a secure boot process should (a) limit what
devices the system can be booted from, (b) only allow the
computer to be booted using OSs from trusted sources, and (c) attempt
to detect possible tampering with the hardware.
`}`{=latex}

### Authentication best practices

- Passwords should not be stored as plain text -- in fact, they should not
  be stored at all.

- Operating systems (and other software) normally instead store a
  cryptographic one-way \alert{hash} of the password or passphrase. \
  (We discuss hashes further when we look at cryptography.)

- Creating a good password hash algorithm is difficult and error-prone,
  so it's best to stick to a known and reliable one.

  - e.g. The default algorithm on many recent Linux distributions is an
    algorithm called [yescrypt][yescrypt]

[yescrypt]: https://www.openwall.com/yescrypt/

::: notes

discussion - encryption vs hashing

**password hash functions** are just one sort of hash function -- different sorts have
different requirements

what hash function is used on windows?

- MD4, apparently (<https://learn.microsoft.com/en-us/windows-server/security/kerberos/passwords-technical-overview>)

on linux?

- many choices
- see e.g.
  <https://unix.stackexchange.com/questions/430141/how-to-find-the-hashing-algorithm-used-to-hash-passwords>
- yescript is based on  `scrypt` (pron "ess crypt") <https://en.wikipedia.org/wiki/Scrypt>
  - yescrypt - bare mention of it on https://en.wikipedia.org/wiki/Yescrypt
- originally designed for Tarsnap, a "backup system for the truly paranoid"

:::


### Authorization

Once a user is authenticated, we need to decide what they are permitted to do
(i.e. perform authorization).

- Authorization is enforced by an \alert{access control system}
- The access control system assumes
  a user has already been authenticated in some way

::: block

#### Definition: access control system

- A collection of methods and components that determines who has access
  to particular system resources, and the type of access they have.
- Ensures all actions on resources are within the security policy
- Supports our goals of achieving confidentiality and integrity

:::

### Access control systems

Why do we cover this?

- So you know what's available when implementing software
  - OS-provided access control systems often have many features we can leverage
- Because multi-user software typically must implement its
  *own* access control system, and it's useful to know the basics
  - e.g. Multi-user software like a bulletin board, ride-sharing app, database, etc will
    need to model users, resources and rights

### Terminology

\alert{principal} (or \alert{subject})

:   \
    Representation of a user or a group of users

\alert{resource} (or \alert{object})

:   \
    Something we want to protect, that a principal can access or operate on in some way --
    e.g. a file, a running process, the database of users.\
    (On Unix systems, many resources are represented as files.)

\alert{permission} (or \alert{right})

:   \
    Some action that can be performed on a resource. \
    E.g. for a file -- **read**ing, **writ**ing and **execut**ing
    the file might be distinct permissions.


::: notes

What are some other permissions?

examples might be:

- Use the {camera,printer,tape drive}
  or other hardware
- Access the network
- Suspend, hibernate or reboot the system
- Connect to and debug a running process
- Exceed quotas (e.g. disk quota for a user, number of open files)
- On a phone: make or receive calls

Technically, we deal not with users but abstractions of human
users. A *principal*. See saltzer and schroeder.

- Saltzer, Jerome H., and Michael D. Schroeder. "The
  protection of information in computer systems." Proceedings of the IEEE
  63.9 (1975): 1278-1308.

  <https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=1451869>

:::

### Access control system

Examples of questions we might expect an access control system to answer:

- Can Alice read the file "`/home/Bob/my-private-journal.txt`"?
- Can Bob open a TCP socket, listening on port 80?
- Can Carol write to row 15 of the database table "`USER-SALARIES`"?

::: notes

database table - this would likely be handled be the DBMSs own access control
system, rather than the OSs.

:::

### Access control system design

- Principle to bear in mind: \alert{Principle of Least Privilege}
  - Programs, users and systems should be given enough privileges to perform their tasks,
    and no more
- *Efficiency*:
  - We can have many file accesses occuring every second, so our system needs to be able to
    make decisions quickly
- *Expressiveness*:
  - We may want to express complex, high-level policies about who can do what

::: notes

which of these are at odds?

ans:

the more expressive and complicated, typically, the less efficient

:::

### Matrix model

- Imagine that we have a matrix listing all principals (as rows) and all resources in the
  system (as columns)

`\begin{center}`{=latex}
![](images/lect04matrix.pdf){ width=8cm }
`\end{center}`{=latex}

### Matrix model

`\begin{center}`{=latex}
![](images/lect04matrix.pdf){ width=6cm }
`\end{center}`{=latex}

\vspace{-2ex}

- At the intersection, we list the **permissions** or rights for that principal and that
  resource\
  (here, **{r,w,x}** = {read,write,execute})
- Called an [\alert{access control matrix}](https://en.wikipedia.org/wiki/Access_control_matrix)
  or \alert{access matrix}

### Matrix model

`\begin{center}`{=latex}
![](images/lect04matrix.pdf){ width=6cm }
`\end{center}`{=latex}

\vspace{-2ex}

Terminology you might also see:

\alert{access control entry}

:   \
    A triplet of (principal,resource,permission list) -- i.e. one cell from the matrix

\alert{access control list}

:   \
    All the access control entries for one resource -- i.e., a column from the matrix


::: notes

just a simple example, showing r/w/x for files. other rights are possible, e.g.

- could be suspend, stop, start, for processes
- listen on a port

Lampson - an originator of the matrix model.

- Lampson, Butler W. (1971). "Protection". Proceedings of the 5th
  Princeton Conference on Information Sciences and Systems. p. 437. \
  <https://dl.acm.org/doi/pdf/10.1145/775265.775268>

note that the matrix model isn't a *complete* specification ...

- It's an abstraction of rights at one point in time.
- It doesn't tell us how rights can change over time.
- e.g. one user may have permissions to change the matrix itself -- to
  give rights to another user

:::

### DAC vs MAC -- who decides?

Who decides what rights subjects have for particular objects?

One answer:

- Individual users can control access to e.g. files that they own \
  $\Rightarrow$ we have a \alert{Discretionary Access Control}
  (DAC) system
- Some system mechanism controls access, and individual users can't
  alter it \
  $\Rightarrow$ we have a \alert{Mandatory Access Control}
  (MAC) system

There are other sorts as well -- e.g. Role Based Access Control (RBAC)
which we don't go into in this unit.

### DAC vs MAC -- who decides?

Discretionary Access Control (DAC) system:

- Owners of objects set the permissions
- Most common approach
- Poses difficulties for e.g. protecting audit logs from sysadmins

Mandatory Access Control (MAC) system:

- Enforced by the OS
- May be appropriate for e.g. Dept of Defence systems
- Implies that superusers/system administrators don't have ultimate
  control
- Used e.g. to ensure not even sysadmins can tamper with the OS kernel,
- To be effective, needs hardware support: else we can e.g.
  boot from a thumbdrive and take control

::: notes

useful refs:

- Bishop 2nd ed chap 4 - access control
- Anderson 2020 sec 6.2 on DACs and MACs

see pros and cons at

<https://www.ekransystem.com/en/blog/mac-vs-dac>

:::

### DACs

For DACs, there's usually one sort of right called "ownership",
which grants the ability to add or remove rights

- e.g. granting others the right to read files in your home directory

### DAC complications

- Suppose we're sysadmin: do we really trust users to
  get all permissions right?
- What if a user wants to download and run programs they found online --
  should they be able to?
- What if some users should be considered more trustworthy than others?

### Mixed systems

\small

Many OSs will actually implement aspects of both MACs and DACs.

Example: Windows provides a kind of support for some MAC-style,
system-specified permissions.

::: block

#### Downloaded files

- Resources have an "integrity-level" label
- Default file label = "medium", but web browser and downloaded files are "low"
- "Low"-labelled files may not alter higher-labelled files.
- $\Rightarrow$ To run some program you downloaded, if it
  will change files on filesystem, user must explicitly upgrade it from "low" to "medium".

:::

::: notes

windows also makes it harder for even administrators to interfere with the event log,
but I have less info about that

source:

- Anderson 2020 sec 6.2

:::

### Question

What is some (non-OS) software you have used recently which would have
need of an access control system?

- Who are the principals? How are they grouped?
- What are the resources?
- How is authentication done? \
  (Password? Fingerprint? Something else?)
- How would password hashes be stored?
- How would you implement it?

::: notes

authentication

- can you authenicate over the phone? Many orgs let you
  - e.g. bank might require you to verify name, perhaps a password/pass number,
    and details like approximate current balance of account

- many web apps delegate a lot of the tasks of authentication (and some part of
  authorization) to 3rd-party providers

  e.g. Okta, Azure, Google Cloud all have offerings in this area

- Or, you might implement it yourself -- storage of ACLs often done with an RDBMS (fast)

best practices: OWASP suggestions:

- <https://owasp.org/www-pdf-archive/ASDC12-Access_Control_Designs_and_Pitfalls.pdf>

:::

### Third-party providers

- Implementing an access control system can be a complex task
- Often we may leverage libraries and services provided by third parties
  - e.g. [Okta](https://www.okta.com/),
    [Azure](https://learn.microsoft.com/en-us/azure/app-service/overview-authentication-authorization),
    [Google Cloud](https://cloud.google.com/docs/authentication)
- This can solve some problems (How to securely store password hashes?) but raises
  others (How much can we trust the provider?)
  - Using the libraries is often complex, and many developers rely on copy-and-pasting code
- Doesn't alleviate us of the responsibility of making sensible design choices

::: notes

- requires care in eliciting requirements, analysis, design
- still need to ensure customer has thought carefully about what roles
  will be, what resources will be
- need to do threat analysis - covered in later lectures
- another question raised by 3rd party services/code: we often need keys to
  access their API, how will those be stored.

example:

cshelp system *partly* outsources -- authentication is done by accessing
the university's Active Directory system via LDAP

:::

### Deputies

- Suppose a user wants to change their password -- stored
  in e.g. `/sys/PASSWORDS`
- We can't give every user read and write permissions
  to that file
- We might give particular *programs* the ability to take
  actions on behalf of particular users at a higher level of privilege
  than the user has.

  - e.g. We might specify that the `passwd` program, when it runs, runs
    as `root` and can read and alter this `sys/PASSWORDS` file.
  - Or we might run a program as a server, started by a privileged user --
    and other users just send *requests*.

### Confused deputies

But if we do this carelessly, it leads to *confused deputies*.

Example: Alice sends a request to a server process
started by Bob -- "give me the contents of `file1`, nicely formatted"

`\begin{center}`{=latex}
![](lect04-images/deputy.svg){ width=80% }
`\end{center}`{=latex}


It's *Bob's* permissions that are used to check whether a file
can be read; so Alice could ask for the contents of
`/home/bob/my_secret_file.txt`,
which she shouldn't be allowed to have access to.

(See "[The Confused Deputy (or why capabilities might have been
invented)][conf-dep]")

[conf-dep]: https://css.csail.mit.edu/6.858/2015/readings/confused-deputy.html

::: notes

see original confused deputy paper

:::

### Confused deputies

An example that has actually occurred:

- A webserver -- each student is allowed to
  create a `public_html` dir from which files for a website are
  served (e.g. `/home/student1/public_html`).
- The webserver process has privileged rights (e.g. because it
  needs to serve on port 80, which only `root` can do)
- A student creates a `public_html` in their directory, but it's
  a *symbolic link* (symlink) to `/etc/passwd` (or some other file
  only root should have access to).
- Because the webserver is running as root, it *does* have access
  to files like `/etc/passwd`; so it serves it up as a webpage.

Solutions to confused deputies: coming up later

### Implementing an access control matrix

Suppose we want to implement an access control system -- how should we
store the information about rights?

"As a very big 2D array" is not a good idea -- many cells would be
empty (i.e. sparse array) or duplicated, array would be large

- e.g. Suppose Alice owns a file. Do we really want to store a list of
  all users at e.g. UWA who *don't* have permission to read/write it?
- We might want to have "default" permissions for things
  - it's then wasteful to list them explicitly for every user/subject
  - only explicitly list people who've specifically been
    granted *more* or *fewer* rights.

### Implementing an access control matrix

Some options:

(a) Store by "column" (resource)
    - Each object is associated with a list of users, and what rights
      they have
(b) Store by "row" (user)
    - Each subject is associated with a list of objects, and what rights
      the user has for it

### ACL vs capabilities

::: block

####

(a) By "column" (resource)
    - Each object is associated with a list of users, and what rights
      they have
(b) By "row" (user)
    - Each subject is associated with a list of objects, and what rights
      the user has for it

:::

- Option (a) leads to the idea of an \alert{access control list} (ACL)
- Option (b) leads to the idea of a \alert{capability system}
  (though there's more to such a system than just this)
  - Rights to do things are held by subjects, and can be
    passed around to other subjects

\hrule

\footnotesize

NB: Don't confuse a "capability system" with `man 7 capabilities`

- a Linux approach to making superuser permissions finer-grained
- not actually a "capability system"

::: notes

On capability systems:

- See e.g. Henry Levy, *Capability-based Computer Systems*
  <http://www.cs.washington.edu/homes/levy/capabook/>

:::

### ACLs vs capabilities

ACL

- Store rights as e.g. file metadata
- Straightforward to implement
- Easy to e.g. revoke one user's rights to a file
- Difficult to determine all rights possessed by one user
- Difficult to e.g. revoke a user's right on *all* files
- Example: All popular OSs (Linux, Mac, Windows)

Capabilities

- Easy to determine all rights possessed by one user
- Easy to add and remove users, and to delegate rights
- Difficult to e.g. change rights of all users to one file
- Example: Various distributed OSs (e.g. [Amoeba][amoeba], experimental
  system developed in 90s)

[amoeba]: https://www.cs.vu.nl/pub/amoeba/amoeba.html

::: notes

- The [Amoeba][amoeba] distributed OS
  is an example of a powerful and flexible capability system.

  Created early 90s -- one creator
  Andrew S. Tanenbaum of [Minix][minix] fame, Minix
  was the chief inspiration for Linux

[minix]: https://en.wikipedia.org/wiki/Minix

:::

### ACLs vs capabilities

- In practice, ACLs don't list *every* user -- doesn't scale well
- Also, we said access control needs to be efficient -- in many systems,
  permissions are only checked when a file is *opened*, not
  each time the file content is accessed
- Many OSs combine aspects of ACLs and capability systems

### Capability systems

We don't look at them in detail, but as noted, many OSs
use aspects of capabilities.

Typically very powerful and flexible.

Particular subjects might have the ability to *copy* capabilities
so they can be given to others -- or perhaps only to *transfer*
capabilities (i.e., the original subject no longer possesses them)


### Unix approach

The Unix approach to *subjects* (principals):

- Users have a *user ID*, and one or more *groups*.
- The `root` user (with user ID 0) is the superuser
  - The first process starts as `root`, spawns others
- Every user has a primary group (stored in `/etc/passwd`), and can be a member of others
  (stored in `/etc/group`).
- A user `nobody` normally exists that owns no files, and
  can be used as a default user for unprivileged operations
- Processes execute with the permissions ("effective user ID") of the
  user that started them
- When determining rights to files, we use a coarse-grained approach
  and divide all principals into
  - the user/owner
  - the group owner
  - everyone else

### Unix approach

The Unix approach to *objects* (principals):

- "Everything is a file"
- represent as many things as possible as *files*; then,
  we can use filesystem permissions to implement our
  access control system

`\begin{center}`{=latex}
![](lect04-images/everything_is_a_file.png)
`\end{center}`{=latex}


### Unix approach

- Classify file rights as "read", "write" and "execute"
  - There are other rights needed to execute particular system calls
    (e.g. to kill a process)
  - The OS kernel will check whether a subject (a process)
    has rights to make particular system calls
  - For the `root` user, the answer is always "yes"\
    (but see `man 7 capabilities`)
- Classify subjects as "user", "group", "everyone else"
- Processes have an *actual* user ID and group ID (based on the user
  that started them)
  - But can also have an *effective* user ID and group ID -- differs
    for `setuid` and `setgid` programs
  - Check file permissions only on open
  - Effectively, file descriptors are a kind of "capability", and can
    be passed around to e.g. to subprocesses, and even unrelated
    processes

::: notes

- you can pass file descriptors over a Unix socket to an
  unrelated process; the
  OS takes care of translating it into an appropriate
  file descriptor on the receiving end.

  See <https://stackoverflow.com/questions/62139881/how-does-passing-file-descriptors-between-processes-work>

:::

### Unix approach

- Doesn't easily allow for flexible rules
  - e.g. "Allow Alice's file `F` to be read by every user
    except Carol and Dan"
- The system calls for managing `setuid` and `setgid`
  programs are easy to get wrong
- Because `root` can do anything -- difficult to create a
  sysadmin-untamperable audit trail/log
  - need to either store off-system/offsite, or modify the
    DAC approach

### Further reading: extensions of the Unix approach

- In this unit, we only consider the "traditional" Unix access control model
- However, it quite possible to extend the Unix approach if desired.

::: block

#### Example: SELinux (Security-Enhanced Linux)

- A collection of kernel features and user-land tools that allow fine-grained mandatory
  access (MAC) and role-based access (RBAC) control systems to be implemented
- The Red Hat Enterprise Linux [User's and Administrator's Guide][rhel-selinux]
  provides a good introduction to SELinux concepts

[rhel-selinux]: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html-single/selinux_users_and_administrators_guide/index#part_I-SELinux

:::

### Further reading: extensions of the Unix approach

::: block

#### Example: POSIX ACLs

- Many Unix file systems allow files to have ["extended attributes"][xatt] which allow more
  flexible policies to be implemented on top of the traditional Unix model
- An example is the [POSIX ACL system][posix-acl], based on draft POSIX standard 1003.1e
- Given a file, it effectively allows us to give additional permissions to particular users,
  without having to alter the owner or group of the file.

:::

[xatt]: https://en.wikipedia.org/wiki/Extended_file_attributes
[posix-acl]: https://www.usenix.org/legacy/publications/library/proceedings/usenix03/tech/freenix03/full_papers/gruenbacher/gruenbacher_html/index.html

::: notes

- tutorials on using POSIX ACLs on Linux can be found at
  <https://tylersguides.com/guides/linux-acl-permissions-tutorial/> and
  <https://www.redhat.com/sysadmin/linux-access-control-lists>

:::


### Solutions to confused deputies

Confused deputies arise when a process with high privileges
is "fooled" into letting a less-privileged principal
do something they shouldn't.

One solution:

- Split the program into two interacting processes that
  communicate.

### Solutions to confused deputies -- client/server

e.g. Suppose a compiler needs to allow users to compile input files
and write to output files, and **also** should write billing/audit
information to `/sys/billing`.

- Split the compiler into two:
  - Compiler part runs as user, will only read or write files the user
    has access to
  - A separate process runs as (e.g. `root`), is communicated with
    by compiler process, writes to `/sys/billing` \
- Better practice would be to create a dedicated `billing` user, not
  to use `root`
- Principle of Least Privilege: give principals only the rights
  they need to perform their job

::: notes

Example of where the use of client/server was considered, but ultimately rejected: cron
(Vixie implementation) on Debian-based systems. (Discussed further in lab 3, setuid.)

For cron:

- Individual non-root users need to create configuration files in
  `/var/spool/cron/crontabs`; BUT
- Individual users shouldn't be able to alter others' files (else we could make the dir
  world-readable and writable).

See bug report <https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=18333>:

- Topi Miettinen <Topi.Miettinen@medialab.sonera.net> suggests splitting cron into critical
  parts which need to be setuid, vs other non-critical parts, and communicate via a socket.

Not implemented however -- instead, Debian creates a "crontab" group and making crontab
setgid.

:::


### Solutions to confused deputies -- `setuid`

Client/server approach is not always appropriate (e.g. not especially fast)

`setuid` approach:

- Make the compiler a `setuid` program, which starts off running as
  `root`
- Open `/sys/billing`
- Immediately drop all root privileges \
  (again: Principle of Least Privilege)
- Now do the job of e.g. reading input files, compiling
  and writing to output files

::: notes

issues here:

- running as `root` not ideal if we can get away with less

:::



### Solutions to confused deputies -- `setuid`

Downsides:

- Relies on programmer to get things right
- On Unix systems, easy to get wrong
- Easy to create [TOCTOU][toctou] bugs -- "time of check vs time of use"

::: code

####

```python
if not actual_user_can_access("file1"):
  sys.exit(1)
fp = open("file1", "w")
fp.write("some data")
```

:::


[toctou]: https://en.wikipedia.org/wiki/Time-of-check_to_time-of-use

See:

- Bishop, "[How to Write a Setuid
  Program](http://nob.cs.ucdavis.edu/~bishop/secprog/1987-sproglogin.pdf)"
  (PDF)
- [Checklist for Security of Setuid
  Programs](http://www.cis.syr.edu/~wedu/Teaching/cis785/Papers/setuid.pdf)
  (PDF)

::: notes

TOCTOU:

- we check, should the *actual* user have access? (not effective user,
  which is root)
- if so, we open and write to file
- but file could've changed in between the two, attacker interposes file


:::


<!-- vim: tw=92
-->
