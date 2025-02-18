---
title:  CITS3007 project marking rubric
---

## Part A rubric

Regarding marks allocation for the writing/research portion of the
project: the same rubric is used as for "long answer" questions in the
mid-semester test. (See
[here](https://secure.csse.uwa.edu.au/run/help3007?p=np&opt=B104).)

## Part B rubric

Regarding marks allocation for the coding portion of the project: the
key thing being assessed is, Can you implement the spec correctly, while
adhering to good secure coding practices? (On top of general
expectations about code clarity and quality, already mentioned in the
spec.)

### General marks breakdown

- **Implementation of the spec**. Code implements the spec: 40 marks.
- **Secure coding best practices**. Code follows secure coding best practices: 10 marks.
- **Code clarity and style**. Code is clear, easy to read, and follows general
  coding best practices (a list is contained in the spec, under "Items
  to submit" / "C files"): 20 marks.

### Possible ways to get very low marks

**Incomplete projects**.

:   Marks for implementation of the spec (40 marks),
    secure coding best practices (10 marks) and code style and clarity
    (10 marks) are predicated on the project actually being *attempted*.
    If the code is mostly or wholly incomplete (e.g. empty function
    definitions, or function definitions containing complete nonsense/no
    serious attempt), then you won't be able to get more than X% of the
    marks for those components for an X%-complete project.

**Projects that don't compile**

:   There is ample opportunity to
    compile, test and run your code with the exact same compiler (`gcc`)
    and the exact same flags (minimum of `-std=c11 -pedantic -Wall
    -Wextra`) on the exact same platform (the CITS3007 standard
    development environment) we will use, so there is no good reason to
    submit non-compiling code.  Projects that don't compile will receive
    at most 5 marks for implementation (out of 40).

    The marks for secure coding best practices (10 marks) and code style
    and clarity (10 marks) are predicated on your actually submitting
    compilable code, so at most 1 mark each will be awarded for those if
    code doesn't compile.

**Projects that segfault/fail to pass minimum tests**

:   Uploaded on the website is
    a basic "submission checker" (here:
    <https://cits3007.github.io/assessment/#project>) which can be
    downloaded and run within a CITS3007 development environment.  It's
    a setuid program, and **NOT EVEN REMOTELY SECURE**, so don't run it
    outside a development VM.[^security_alert] You'll need to run `sudo
    tar xf submission-checker.txz` within a VM to unpack it.

    [^security_alert]: It's fairly unlikely to do anything terribly
      dreadful to a system, or to be targetted by attackers. Nevertheless,
      it operates at a privileged level, and assumes it's being run in a
      virtual environment where it can do little damage or expose
      confidential information, so it would be best not run it outside a
      VM.

    Given an `adjust_score.c` file, the submission-checker will:

    - unpack the contents of a filesystem which can be used as an isolated
      environment
    - mount that filesystem and copy in your code
    - compile and link it against a sample `main.c`
    - run a minimal test (incrementing the score of a user who doesn't yet have
      a record in a valid scores file).

    At a bare minimum, your code should be able to result in a valid "scores" file
    with the correct contents, as checked for by this program. (Actually, it
    should do more: to keep the implementation small and simple, this
    submission-checker doesn't instrument your code with the Google
    AddressSanitizer, UBSanitizer and MemorySanitizer sanitizers, which we
    will do when testing your code.)

    Code that can't do so will be awarded at most 10 marks out of 40 for implementation,
    at most 3 marks for secure coding best practices, and at most
    5 marks for code clarity and style.

    (NB: You can also use the submission checker to tell you, in case
    you're unsure, if you're about to
    submit something that looks like a report file [PDF or Markdown],
    `answers.txt` file and `adjust_score.c` file. Invoke it as
    `./submission-checker /path/to/report_file /path/to/answers_file
    /path/to/c_file`.)

### Ways to get high marks

Assuming your code doesn't fall into one of these 3 unfortunate
categories, marks are awarded as follows:

- **Implementation of the spec** (40 marks).

  Note that all programs will be instrumented with Google sanitizers (AddressSan, MemorySan
  and UBSan), and marks will only be awarded if tests pass *with those sanitizers enabled*.

  Program produces correct results when run on scores files containing typical data: 20 marks.

  Program produces correct results when run on scores files containing unusual and/or edge-case
  data: 15 marks.

  Program is robust (does not segfault or crash) even in the face of malformed
  or otherwise incorrect scores files and other unusual circumstances: 5
  marks. The program may terminate execution by
  calling functions intended to exit the program, but should print an error message
  before doing so.

- **Secure coding best practices** (10 marks).

  There'll be specific things we're looking for here.

  Note that in an ideal project, we'd aim to not just detect errors and halt
  execution (as outlined above), but also ensure any errors are propagated up
  to the part of the program best equipped to diagnose/handle them. But for
  this project we won't require that. (My code doesn't do it.)

- **Code clarity and style** (20 marks). When writing code that aims to be
  secure, it's *especially* important that your code is easily auditable and
  readable, and that (where relevant) you've justified particular approaches to
  security that you've taken. So ensuring your code is easy to read, well-formatted,
  and follows general coding best practices (there's a list in the spec) is
  worth quite a bit (around 30%, compared to the 10% that is often typical for
  CSSE projects).

  If your code contains known bugs / small portions are incomplete,
  document those using the Doxygen `\bug` command in a documentation
  block for the relevant function or the file as a whole. It won't mean
  you get marks for them, but it will count towards good code clarity
  and style: documenting known bugs is good style.

  It's not necessary for every function to include a documentation
  block, but major functions should; and even the most minor functions
  should at least have a couple of lines of comment above them
  explaining what they do.

  Excessive *inline* comments (comments in the body of a function) are
  poor coding style, as they make your code harder to read -- they will
  result in a low mark for code clarity and style.

  If your code emits excessive warnings on compilation, that's often
  (though not always) a sign that you've done something that's poor
  style/not best practice. Good C textbooks should contain advice on
  reducing warnings.[^good-textbooks]


[^good-textbooks]: In the event of a dispute about what constitutes a
  "good C textbook": any text that contradicts basic advice given
  in Seacord, *Effective C: An Introduction to Professional C
  Programming* (2020) is not a good textbook.

## General project advice

### Making use of the lab facilitation staff

The lab teaching stuff will usually be happy to give general advice on
overcoming difficulties with C
programming. If you're running into problems, then taking your code
along to a lab to discuss is definitely a good idea. But bear in mind
that they are unlikely to

- clarify the project spec for you (you're expected to interpret the
  spec yourself, or post in the forum if the spec is
  unclear/inconsistent/incomplete)
- tell you exactly what secure programming practices to
  adopt (it's up to you to work out what's relevant, based on what we've
  covered in class), or
- teach you the basics of C (that's assumed prior knowledge).

Assistance with the project is also a lower priority than covering any
required exercise material.



<!-- vim: syntax=markdown tw=72
-->
