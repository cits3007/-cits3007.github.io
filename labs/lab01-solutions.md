---
title: |
  CITS3007 lab 1&nbsp;--&nbsp;Development environment&nbsp;--&nbsp;solutions
---

<div style="border: solid 2pt orange; border-radius: 5pt; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

**Laptop and SDE requirement**

When attending lab classes, you will need access to a laptop from which you can access the
[CITS3007 standard development environment][faq-dev-env] (SDE), which is based on Ubuntu 20.04,
running on version 5.4.0 of the Linux kernel on an x86-64 processor.

[faq-dev-env]: https://cits3007.github.io/faq/#cits3007-sde

The CITS3007 SDE will be used for the unit project, later in semester: all
submissions are expected to compile and run correctly in this environment (though you may
find it useful to test your code in other environments, as well).

We may also refer to the SDE in tests or quizzes -- you might be asked to write code which
will compile in this environment, for instance.

In the SDE, you are able to run commands as `root` (by using the [`sudo` command][sudo]), and to
alter the parameters of the running kernel (using the [`sysctl` command][sysctl]). Being able to run
`sudo` is important for many of the labs, so you'll want to make sure it functions correctly
for you. Being able to run the `sysctl` command is of lesser importance -- you will likely
only need to use it if you want to complete the extension tasks for lab 4 (on buffer
overflows) and lab 8 (on race conditions).

[sudo]: https://linux.die.net/man/8/sudo
[sysctl]: https://linux.die.net/man/8/sysctl

The preferred way of accessing the SDE is by running [VirtualBox][virtualbox] and [Vagrant][vagrant]
on your laptop, as outlined in this lab sheet.  (Vagrant is widely used in industry to provide sandard development
environments.)
However, if you run into difficulties, there are a few other options,
outlined below.

Note that UWA provides [financial support][fin-supp] via the "SOS IT Equipment Scheme" to students who
are unable to purchase a laptop due to financial hardship.

[fin-supp]: https://www.uwa.edu.au/students/Support-services/Financial-assistance#:~:text=SOS%20IT%20Equipment%20Scheme

</div>

## 1. Setting up the C development environment { #c-devel-env }

To access the CITS3007 standard development environment, the preferred option is:

<div style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**Use Virtualbox and Vagrant**

:   Install two open source tools ([VirtualBox][virtualbox] and
    [Vagrant][vagrant]) on your Windows or Linux laptop. See
    [VirtualBox and Vagrant](#virtualbox-and-vagrant), below.

    You will need about 15 GB of disk space and a minimum of 4 GB of RAM on your
    laptop. (Note that 4 GB of RAM is likely to result in your VM
    running very slowly -- 8 GB or more is preferable.)

</div>

However, VirtualBox is not available for M-series (Apple Silicon) mac laptops -- in that case, you
might like to try:

<div style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**Use UTM for Mac**

:   This involves installing UTM, virtualization software for the macOS operating system.
    See [Using UTM on M-series Mac laptops](lab01-utm.html).
    As explained on that page, you'll need to pick one or more Linux virtual machine images.
    If compiling code for the project or the lab 4 and 8 extension tasks, you'll need
    to make sure you are using an appropriate virtual machine.

    In general, the facilitators and unit coordinator won't be able to assist you if you run
    into problems using UTM, but you should feel free to post on the Help3007 discussion
    forum, as other students using Macs may be able to assist.

</div>

Virtualbox also may not function properly on Windows laptops where you are already using
the [Windows subsystem for Linux][wsl] (WSL).

[wsl]: https://learn.microsoft.com/en-us/windows/wsl/about

In that case, you might wish to:

<div style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**Use Ubuntu 20.04 from WSL on Windows**

:   In this case, we assume you are already familiar with how to install
    and access Ubuntu distributions using the WSL.

    In general, the facilitators and unit coordinator won't be able to assist you if you run
    into problems using the WSL, but you can find general instructions on using it
    [here][using-wsl].

    If you use the WSL, you *will* be able to run commands as root using `sudo`, but may
    not be able to alter kernel parameters using `sysctl`.

</div>

[using-wsl]: https://learn.microsoft.com/en-us/windows/wsl/install

If none of the above options work for you, a remaining option is:

<div style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**Access the SDE using GitPod**

:   [GitPod][gitpod]
    provides web-based access to development environments
    hosted in the cloud, and provides a quota of free hours each month (which most students
    will probably not exceed).

    We provide instructions on how to use GitPod [here](lab01-gitpod.html).

    If using GitPod, you *will* be able to run commands as root using `sudo`, but won't
    be able to alter kernel parameters using `sysctl`.

</div>    

[virtualbox]: https://www.virtualbox.org
[vagrant]: https://www.vagrantup.com
[gitpod]: https://gitpod.io/




### VirtualBox and Vagrant { #virtualbox-and-vagrant }

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**What are VirtualBox and Vagrant?**

**VirtualBox** is a type of [virtualization software][virtualization] which allows you to
run other operating systems on your computer – even operating systems designed for
completely different hardware (such as mobile phones, for instance).  It allows precise
control of what OS [kernel][kernel] is run.
(Other technologies for running Linux do exist -- for instance, Docker and Windows
Subsystem for Linux, or WSL -- but they do not allow the precise control over kernel version
that VirtualBox does.)

**Vagrant** is a tool for managing virtual environments. It can manage environments created
using VirtualBox, Docker, VMWare, WSL, and many more.  It handles tasks such as

- fetching virtual machine (VM) [images][vm-image] from the web and installing them in a
  standard location;
- checking whether any updates have been issued for an image, and prompting the user to
  download them, if so;
- allowing a user to easily manage multiple versions of the same basic VM image; and
- abstracting over the details of particular virtualization technologies.

It's widely used in industry to provide developers with a Standard Development Environment
(SDE) for their team or organization, so it's hoped that experience with Vagrant will prove
useful beyond your university studies.

Since the purpose of Vagrant is to manage VMs on a computer, vagrant commands need to be run
from the *host* computer (for instance, your laptop), not from within a VM.

[virtualization]: https://en.wikipedia.org/w/index.php?title=Hardware_virtualization
[kernel]: https://en.wikipedia.org/wiki/Kernel_(operating_system)
[vm-image]: https://en.wikipedia.org/w/index.php?title=Disk_image

</div>



To install VirtualBox and Vagrant:

1.  Visit <https://www.virtualbox.org/wiki/Downloads>, download
    the appropriate VirtualBox package for your platform (Windows, MacOS or Linux),
    and install it.

    Note, if you are using a Mac: VirtualBox is only available for Macs using **x86-64
    processors**. If you are using a more recent M-series (Apple Silicon) Mac, then it uses an ARM64
    processor, and you should be using UTM, as described previously.
    If you don't know which yours is: you can go to the 'Apple' at the
    top left of your screen and select 'About this Mac', and it will show whether your
    cpu is Apple silicon (ARM, M1 or M2) or x86/Intel.

    - On Windows, the installer is an `.exe` file you can simply run
      by double-clicking.
    - On MacOS, the installer is a `.dmg` file -- double click on it
      for instructions.
    - On Ubuntu distributions a `.deb` file is provided -- we assume you already know how to install
      one of these. If you are using a non-Ubuntu distribution, consult
      the VirtualBox documentation for instructions on how to install.

#.  Visit <https://www.vagrantup.com/downloads>, download the
    appropriate Vagrant package for your platform, and install it.

    - On Windows, the installer is an `.msi` file you can run by
      double-clicking.
    - On MacOS, you can either download a `.dmg` file, or
      (if you have [Homebrew](https://brew.sh) installed) there are
      instructions for adding Vagrant with a `brew install` command.
    - On Ubuntu, it's recommended you install Vagrant using the APT package
      manager (see "Package manager for Linux" on the Vagrant page).

    <!--
    macos old intel: may hit network issue, just google and amend
    vagrantfile
    -->

#.  Check that Vagrant has been successfully installed.

    On Windows, open a "Command Prompt" window. On MacOS or Linux, open
    a terminal window.

    Then type `vagrant --version`; Vagrant should display

    ```
    Vagrant 2.3.7
    ```

    (Or something similar – any version from 2.2.0 onwards should be
    fine.)

#.  Test a small virtual machine (VM) image.

    On your *host machine* (i.e. your laptop), create a new directory
    (called e.g. `vagrant-test`), open a terminal window and `cd` into
    the directory. (The `cd` command works on Linux, Mac and Windows.)

    Type `vagrant init generic/alpine316`, then `vagrant up
    --provider=virtualbox`.  Vagrant should take about a minute to
    download a virtual machine containing [Alpine Linux][alpine] and
    configure it for use.

    If you type `vagrant ssh`, Vagrant will connect to the new virtual
    machine using [`ssh`][ssh] and present you with a shell prompt:

    ```
    $ vagrant ssh
    alpine316:~$
    ```

    Note that the [command-line prompt][prompt] has now **changed**
    from the usual terminal prompt you see. The prompt `alpine316:~$`
    indicates that you're running commands in an Alpine Linux VM. To be able
    to run commands from your host machine again, you need to exit
    from your SSH session to the VM. (Or alternatively, of course, you
    can simply open a new terminal window.)

    You can exit your current SSH session by typing `exit` or
    hitting `ctrl-D`.

    (It's not strictly necessary, but it's good
    practice to also *halt* VMs when you're not using them --
    you can do so by issuing the command `vagrant halt` after you've
    exited the SSH session.)

    <div style="border: solid 2pt orange; border-radius: 5pt; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

    **Troubleshooting on Linux**

    Note that on Linux, if you happen to already have `libvirt` installed:
    `libvirt` and VirtualBox can't both run at the same time. Ensure
    that `libvirt` isn't running by typing `sudo systemctl stop
    libvirtd`.

    </div>

    [alpine]: https://www.alpinelinux.org
    [ssh]: https://www.ssh.com/academy/ssh
    [prompt]: https://en.wikipedia.org/wiki/Command-line_interface#Command_prompt

#.  Download a VM containing the standard CITS3007 development
    environment. Note that this will likely take **around 5--10
    minutes**, so once you've started the process, move onto the next
    exercise.

    On your *host machine* (i.e. your laptop), create a new directory
    (called e.g. `cits3007-test`), open a terminal window and `cd` into
    the directory.

    Type `vagrant init arranstewart/cits3007-ubuntu2004`, then `vagrant
    up --provider=virtualbox`.  Vagrant should take around 5--10 minutes
    to download a virtual machine and configure it for use. The output
    from Vagrant should look something like the following:

    ```
    Bringing machine 'default' up with 'virtualbox' provider...
    ==> default: Importing base box 'arranstewart/cits3007-ubuntu2004'...
    ==> default: Matching MAC address for NAT networking...
    ==> default: Checking if box 'arranstewart/cits3007-ubuntu2004' version '0.1.2' is up to date...
    ==> default: Setting the name of the VM: xxx-cits3007-test_1658816964468_16898
    ==> default: Clearing any previously set forwarded ports...
    ==> default: Fixed port collision for 22 => 2222. Now on port 2200.
    ==> default: Clearing any previously set network interfaces...
    ==> default: Preparing network interfaces based on configuration...
        default: Adapter 1: nat
    ==> default: Forwarding ports...
        default: 22 (guest) => 2200 (host) (adapter 1)
    ==> default: Booting VM...
    ==> default: Waiting for machine to boot. This may take a few minutes...
        default: SSH address: 127.0.0.1:2200
        default: SSH username: vagrant
        default: SSH auth method: private key
        default: Warning: Connection reset. Retrying...
        default: Warning: Remote connection disconnect. Retrying...
        default: Warning: Connection reset. Retrying...
        default:
        default: Vagrant insecure key detected. Vagrant will automatically replace
        default: this with a newly generated keypair for better security.
        default:
        default: Inserting generated public key within guest...
        default: Removing insecure key from the guest if it's present...
        default: Key inserted! Disconnecting and reconnecting using new SSH key...
    ==> default: Machine booted and ready!
    ==> default: Checking for guest additions in VM...
    ==> default: Setting hostname...
    ```

    Once it's done, you can type `vagrant ssh` to get shell access to
    the new VM:

    ```
    $ vagrant ssh
    vagrant@cits3007-ubuntu2004:~$
    ```

    Note that again, your terminal prompt has changed -- it sould look
    like this:

    ```
    vagrant@cits3007-ubuntu2004:~$
    ```

    (usually in bright green). The prompt indicates the name of the
    current user ("`vagrant`")[^vagrant-user], and the hostname for the
    machine you are connected to ("`cits3007-ubuntu2004`").

    If you want to issue commands on your host machine again (that is,
    on your laptop), you'll need to either

    - exit your current SSH session, either by running the command `exit`
      or by typing `ctrl-D`
    - open a new terminal window, and issue your commands from there.

    In particular, trying to run Vagrant commands from within your VM
    will *not* work, since Vagrant has not been installed within the VM.

#.  Optional: install VS Code and the "Remote-SSH" extension.

    The standard CITS3007 development environment VM comes with the
    `vim` editor already installed, and for some tasks we do,
    it will be necessary to use `vim` or another terminal-based
    editor.

    However, if you already have VS Code installed and are familiar
    with it, we provide instructions on how to configure it
    for use with a Vagrant box [here](lab01-vs-code.html).

[^vagrant-user]: Conventionally, all Vagrant VMs have a user account
  named "vagrant" on them, with the password for the account also
  set to "vagrant". This makes it very easy to log into them.
  Normally, using a well-known, easy-to-guess, hard-coded user ID and
  password would be a [*poor* security practice][default-creds].\
  &nbsp;
  But in this case: the VM is only being used for development purposes,
  rather than deployed in production; it should only be possible to
  connect to the VM from your local computer; and the VM is unlikely to
  contain any confidential data of yours. This being so, the use of
  default credentials is usually an acceptable risk.

[default-creds]: https://en.wikipedia.org/wiki/Default_Credential_vulnerability

#### Vagrant cheat sheet

You can find a useful "cheat sheet" of commands for managing Vagrant VMs
here:
<https://cheatography.com/davbfr/cheat-sheets/vagrant-cheat-sheet/>.




## 2. Concept review questions

If possible, it's recommended you work on this section in pairs or small
groups and compare answers.

### 2.1. Definitions

For each of the following concepts, review the definition given in the
lecture slides, and explain it in your own words. If you can, give an
example, based on your own experience or from recent news reports:

a. threat
#. vulnerability
#. confidentiality
#. integrity
#. availability



<div class="solutions">

**Sample solutions**

Definitions for all these terms are in the week 1 lecture slides: your
explanations should be in your own words.

***Examples*** of each could be:

a.  threat: Any source of danger or loss will do. A threat to
    *information security* specifically might be: Accidental [or
    intentional] deletion of records from a server.
#.  vulnerability: A vulnerability rendering (for example) your house
    susceptible to physical security might be: there are no locks
    on your doors and windows. (This renders you susceptible
    to theft.)

    A vulnerability of a computer system might be: lack of
    a firewall. (This allows connections to potentially be made
    from any remote system to any [open port][open-port] on the computer system.)
#.  confidentiality: An example of confidentiality in the physical world
    might be: I might have a filing cabinet containing (say)
    financial records which I don't wish to be publicly available.

    An information security example could be much the same (financial
    records), except that the records are held on a computer system
    (e.g. a bank's servers).
#.  integrity: An example of a (compromise of) information
    integrity could be the example given above -- deletion of records
    from a server.

    A *physical security* example is similar: suppose business records
    are kept in hard-copy format (as was not uncommon up until the
    mid to late twentieth century). If those hard copies are
    destroyed (or fraudulently altered), there's been a compromise of
    *integrity*.
#.  availability: An example of a (compromise of) availability
    is when (for instance) a web server goes down.

    The server could become unavailable due to (for instance) a natural
    disaster such as flood or power outage, but also due to
    a malicious attacker.


[open-port]: https://en.wikipedia.org/wiki/Open_port
[fed-atlanta]: https://www.atlantafed.org/-/media/documents/banking/consumer-payments/research-data-reports/2020/02/13/us-consumers-use-of-personal-checks-evidence-from-a-diary-survey/rdr2001.pdf


</div>







### 2.2. Concepts

a.  How would you describe the relationship between threats,
    vulnerabilities, and attacks? Give examples to illustrate the
    relationship.

    

    <div class="solutions">

    **An example answer:**

    A *threat* can be thought of as a source of danger (e.g.
    a power outage could pose a threat to the integrity of computer records stored
    on a server). However, not all systems will necessarily be
    vulnerable (that is, susceptible) to a particular threat.
    For example, systems with proper backups might not be vulnerable
    to a loss of integrity due to a power outage.
    If the power outage were due to natural causes or accident, then
    this would not be an *attack*, but rather an "incident".
    However, if the power outage was deliberately brought about with the
    intent of causing data loss, then this would be an *attack*.
    (An example of a known cyber attack on a power grid is the
    "[Ukraine power grid hack][ukraine-power-hack]" of December 2015,
    in which attackers were able to remotely compromise the computer
    systems of three power distribution companies in Ukraine, and
    temporarily disrupted the power supply to consumers.)

    [ukraine-power-hack]: https://en.wikipedia.org/wiki/Ukraine_power_grid_hack

    </div>
    



#.  Is it possible for a *threat* and a *vulnerability* to both exist,
    without there being an attack? Explain why or why not.

    

    <div class="solutions">

    **An example answer:**

    Yes, it is possible.

    A threat to (for instance) data integrity might exist (an intern
    with access to a server), and a vulnerability might exist (files
    on the server do not have adequate protections and/or are not
    properly backed up), and the threat might even eventuate (the intern
    accidentally deletes all files). But if the act wasn't carried out
    *intentionally*, this is not considered an "attack".

    </div>
    

#.  Consider an ATM, which requires users to provide a bank card and a
    PIN in order to perform transactions. Give examples of
    *confidentiality*, *integrity* and *availability* requirements
    associated with the system. How important do you think each
    requirement is -- are they all equally important, or does the
    importance differ?

    

    <div class="solutions">

    **An example answer:**

    An example of a confidentiality requirement might be: when a bank
    card and a PIN are provided, the ATM doesn't let a customer view
    the transactions of *other* customers.

    For integrity: when a bank card and a PIN are provided, the ATM
    doesn't let a customer make transactions for other customers
    (nor does it let records of transactions be deleted).

    For availability: the provider of the ATM system might have a
    requirement along the lines of
    "the ATM shall not be unavailable due to hardware or software
    system error more than 0.1 percent of the time the system
    is operational". (This would allow, for instance, the system to be
    unavailable for no more than about 9 hours each year.)\
    If the *bank*, however, suffered a local power outage, that would not
    be a failure of the requirement, so the provider of the ATM system
    would not be at fault. (The banks customers might be unhappy,
    though: perhaps the bank should ensure backup generators
    are available.)

    All of these requirements are fairly important, but
    most customers would probably rate the "integrity" requirement
    as being most important, then the confidentiality requirement,
    and finally the availability requirement.

    </div>
    

## 3. AusCERT security bulletins

It's recommended you complete this exercise in pairs or small groups.
Security Bulletins are sometimes not easy to understand, at first
glance, but by sharing ideas and comparing your understanding with
other students, you should be able to complete the questions in this
section.

[AusCERT][auscert] is a non-profit organisation that provides advice on cybersecurity
threats and vulnerabilities. Some of its information is publicly
available, and some is provided only to members.

Visit its website at <http://www.auscert.org.au/>, and from the
"Resources" tab, select "Security Bulletins". (The direct link is
<https://auscert.org.au/bulletins/>.)

Take a look at several, and try searching for
`ASB-2022.0077` and `ESB-2022.3655`. All bulletins have the following
features:

- They have an *ID number*. Bulletins with an ID number
  beginning with "ESB" are bulletins from external sources, being
  redistributed by AusCERT; those beginning with "ASB" are published by
  AusCERT itself. The ID number contains 4 digits indicating what year the
  bulletin was published, followed by the bulletin number within that
  year.
- They specify the product affected by some vulnerability, the operating
  system affected, and what the *resolution* (i.e. action to be taken,
  in order to mitigate or prevent the vulnerability) is.
- They refer to one or more *CVE* identifiers. The CVE ("Common Vulnerabilities and
  Exposures") identification scheme is provided by the US-based MITRE
  organisation. For instance, Auscert's bulletin `ASB-2022.0129`
  refers to `CVE-2022-30168`, the details of which can be found at
  <https://www.cve.org/CVERecord?id=CVE-2022-30168>.
- They give a CVSS (Common Vulnerability Scoring System) score. This is
  a way of quantifying the severity of a vulnerability using a number
  from 0 to 10, where 0 is the least severe and 10 the most.
  Read more about these severity ratings at
  <https://nvd.nist.gov/vuln-metrics/cvss>.

Now, find the `ESB-2022.1671` bulletin and read through it, and refer to
the NIST page on severity ratings at <https://nvd.nist.gov/vuln-metrics/cvss>.
Then answer the following questions:

a.  The bulletin is for a patch which fixes a *vulnerability*.
    What is the vulnerability (give a CVE identifier and a description)?
    In what product does it occur?

    

    <div class="solutions">

    **example answer:**

    ASB-2022.0129 mentions the vulnerability CVE-2022-1271, which
    occurs in the open source `gzip` software package.


    The `zgrep` program is intended to allow a user to search a possible
    compressed file for a regular expression (run `man zgrep` for
    details). The AusCERT bulletin says that if `zgrep` can be forced
    to run on a file with a specially crafted filename, then this
    could result in execution of arbitrary code or overwrite of
    arbitrary files.

    (Details of what these "specially crafted" filenames might
    be are harder to track down.
    But following links from the AusCERT bulletin
    to Debian's own security advisory, at
    <https://www.debian.org/security/2022/dsa-5122>, then
    Debian's security tracker
    (<https://security-tracker.debian.org/tracker/source-package/gzip>),
    and then to
    <https://security-tracker.debian.org/tracker/CVE-2022-1271>
    give more details. It appears if a filename contains newlines --
    which is allowed, on Unix-like systems -- then that could
    allow this vulnerability to be exploited.)

    </div>
    

#.  How severe is the vulnerability? If a vulnerability is severe, does
    that necessarily mean it is easy to exploit, and/or has a high
    probability of being exploited?


    

    <div class="solutions">

    **example answer:**

    AusCERT has published a CVSS rating of 7.1 for this
    vulnerability, indicating a "high" severity.

    This means that the consequences could be severe if the vulnerability
    *is* exploited; it doesn't mean that vulnerability is easy to
    exploit, nor that there's a high probability of this happening.
    (In fact, reading the linked documents in this case suggests
    there's a fairly low probability of this vulnerability being
    exploited.)

    AusCERT in this case has used a CVSS published by Red Hat,
    and you may notice that the NVD contains a *different* CVSS rating.
    You can read [more about CVSS scores and how they are
    calculated][cvss-scores] [here][cvss-scores].

    [cvss-scores]: /labs/lab01-cvss.html

    </div>
    


Locate the CVE information page on <https://www.cve.org> for
this vulnerability, and follow the links there to find out more
about it. Google for the definitions of any terms you do not know --
[Wikipedia](https://en.wikipedia.org/) has good summaries of most terms.
Answer the following questions:

c.  How could an attacker exploit this vulnerability? Describe
    a scenario in which this could occur.

    

    <div class="solutions">

    **example answer:**

    From the linked documents, it appears an attacker would have to
    force the `zgrep` command to be run with a specially-crafted
    filename containing newlines.

    This is a fairly unlikely occurrence, but if (for instance)
    the attacker could convince a user to run an attacker-provided
    script which did this, then the vulnerability could be exploited.

    </div>
    


#.  Of the "C I A" security goals -- which ones could be compromised
    if this vulnerability were exploited?

    

    <div class="solutions">

    **example answer:**

    The attacker is able to overwrite arbitrary files,
    so they could compromise *integrity*.

    But it seems that in some cases, they can execute arbitrary code,
    as well -- in which case, all three C I A goals could be
    compromised.

    </div>
    





[auscert]: http://www.auscert.org.au/

## 4. C development

<div style="border: solid 2pt orange; border-radius: 5pt; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

**C language familiarity**

Ideally, you should be doing this unit in your 3rd year, and have
familiarity with the C programming language.

You need to either

- have access to a **good C programming textbook** -- see the
  CITS3007 ["Resources"
  page](https://cits3007.github.io/resources/#c-programming) for one
  recommendation -- or
- be comfortable searching through and interpreting the C language pages at
  **<https://en.cppreference.com/>**.

Note that YouTube tutorials will **not** be sufficient as a C reference
for this unit -- you **will** need a textbook (or to make use of the cppreference.com
site). The lab facilitators will not be able to assist you if you are
using video tutorials instead of a C language reference.

If you are still in the process of purchasing a textbook, the
following sites may provide a helpful refresher:

- Carl Burch, [*C for Python programmers*][c-for-py]
  - Slides based on [*C for Python programmers*][c-for-py-slides]
- Steven Simpson, [*Learning C from Java*][c-from-java]
- Daniel Weller and Sharat Chikkerur, [Practical Programming in C][prac-c] (lecture slides)

[c-for-py]: http://www.cburch.com/books/cpy/index.html
[c-for-py-slides]: https://web.cs.hacettepe.edu.tr/~bbm101/fall16/lectures/w12-c-for-python-programmers.pdf
[c-from-java]: https://ssjools.hopto.org/java2c/alldiffs
[prac-c]: https://ocw.mit.edu/courses/6-087-practical-programming-in-c-january-iap-2010/pages/lecture-notes/

</div>


### 4.1. C questions

a.  What is the difference between the C literal values `3`, `'3'`,
    `"3"` and `3.0`? What is the type of each?

    

    <div class="solutions">

    **Answer**

    The literal value `3` is an integer -- it has type `int` (see
    [here](https://en.cppreference.com/w/c/language/integer_constant#The_type_of_the_integer_constant)).

    The literal value `'3'` is a character constant -- it also has type
    `int` (see
    [here](https://en.cppreference.com/w/c/language/character_constant)).

    The literal value `"3"` is a string literal; it has type `char[1]`
    (see
    [here](https://en.cppreference.com/w/c/language/string_literal)).

    The literal value `3.0` is a floating constant; it has type `double`
    (you could force a type of `float` by writing `3.0f`. See
    [here](https://en.cppreference.com/w/c/language/floating_constant)).

    </div>
    





#.  Consider the following C code:

    ```C
    const char * str = "test";
    char c = str[0];

    if (c >= 65 && c <= 90) {
      printf("high\n");
    } else if (c >= 97 && c <=122) {
      printf("low\n");
    } else {
      printf("other\n");
    }
    ```

    If this code is executed, what will be printed to the terminal?


    

    <div class="solutions">

    **Answer**

    Assuming the code is part of some function which is executed,
    then the text "`low`" will be printed to the terminal.
    This is because the first element of the string `str`
    is the char `'t'`, which can also be interpreted as the integral
    value 116. Since 116 is between 97 and 122 (inclusive),
    the string `"low\n"` will get printed.

    </div>
    

#.  What is the output of the question (b) code if we change the string
    `"test"` to `"TEST"`?


    

    <div class="solutions">

    **Answer**

    The first element of `str` in this case is `'T'`,
    which can also be interpreted as the integral
    value 84. Since 84 is between 65 and 90 (inclusive),
    the string `"high\n"` will get printed.

    </div>
    

#.  What is the output of the question (b) code if we change the string
    `"test"` to `"???"`?

   

    <div class="solutions">

    **Answer**

    The first element of `str` in this case is `'?'`,
    which can also be interpreted as the integral
    value 63. The first two "`if`" conditions in the code evaluate to
    false, so the last `else` branch is executed, and the text
    "`other`" is printed.

    </div>
    

### Building and modifying C programs

1.  Download the `lab-01-code.zip` file from the "Labs" section of the
    CITS3007 [Resources page](https://cits3007.github.io/resources/#labs).

    The best way to do so is from *within* your CITS3007 development environment
    (i.e. from within the virtual machine).

    You can do so by running

    ```bash
    $ wget https://cits3007.github.io/labs/lab-01-code.zip
    ```

2.  Unzip the zip file into a directory of your choosing, using the `unzip`
    command. (See `man unzip` if you need details of how to use the `unzip` command.)

3.  Build the `test_leap` program by typing `make`, then try running it
    with various different command-line arguments:

    ```
    $ ./test_leap 1901
    $ ./test_leap 1900
    $ ./test_leap 2000
    $ ./test_leap -1
    $ ./test_leap -0
    $ ./test_leap 9223372036854775807
    $ ./test_leap 9223372036854775808
    $ ./test_leap foo
    $ ./test_leap foo bar
    ```

    What results do you get? Take a look at the code in `test_leap.c`,
    read the documentation for the `strtol` function (by running
    `man strtol`),
    and explain what is happening in each case.

   

    <div class="solutions">

    A copy of the code with line numbers printed is included below.

    ```{ .C .numberLines}

    #include <stdlib.h>
    #include <stdio.h>
    #include <errno.h>

    /* return 0 (false) or 1 (true), depending on whether
     * `year` is a leap year or not.
     */
    int is_leap(long year) {

      if (year % 4 != 0) {
        return 0;
      }

      if (year % 100 == 0) {
        return 1;
      }

      return 0;
    }

    int main(int argc, char **argv) {
      argc--;
      argv++;

      if (argc != 1) {
        fprintf(stderr, "Error: expected 1 command-line argument (a YEAR), but got %d\n", argc);
        exit(1);
      }

      char *end;

      // clear errno so we can check whether strtol fails
      errno = 0;
      long year = strtol(argv[0], &end, 10);
      int res_errno = errno;

      if (end == argv[0]) {
        fprintf(stderr, "Error: couldn't interpret '%s' as a number\n", argv[0]);
        exit(1);
      } else if (res_errno == ERANGE) {
        fprintf(stderr, "Error: '%s' is outside the range of numbers we can handle\n", argv[0]);
        exit(1);
      } else {
        if (is_leap(year)) {
          printf("%ld is a leap year\n", year);
        } else {
          printf("%ld is not a leap year\n", year);
        }
      }

    }
    ```

    **Answer**

    The results observed should be as follows:

    - 1901: Because 1901 is not divisible by 4, line 12 (the first `return 0;`) of
      `is_leap` gets executed, and `"1901 is not a leap year"` is
      printed.
    - 1900: Because 1900 is divisible by both 4 and 100, line 19 (the
      final `return 0;`) of `is_leap` gets executed, and `"1900 is a leap
      year"` is printed. (This is incorrect, though -- 1900 is not in
      fact a leap year.)
    - 2000: The same path through the code is executed as for 1900,
      and `"2000 is a leap year"` is printed. (And this is correct,
      2000 *is* a leap year.)
    - -1: The same path through the code is executed as for 1901,
      and `"-1 is a leap year"` is printed. (This is correct, if we use
      the "[proleptic Gregorian calendar][proleptic-cal]"; that is, we
      imagine the Gregorian calendar is extended backwards before the
      year in which it was actually introduced, namely 1582 for most of
      Europe, and 1752 in Britain and its colonies.[^britain-gregorian])
    - -0: The `strtol` function interprets `-0` the same as `0`,
      and the same path through the code is executed as for 1900,
      and `"0 is a leap year"` is printed. (Which is correct, if we use
      the proleptic Gregorian calendar.)
    - 9223372036854775807: This number is not divisible by 4, so
      the same path through the code is executed as for 1901,
      and `"9223372036854775807 is not a leap year"` is printed.
    - 9223372036854775808: This number exceeds the range of what can be
      stored in a C `long int`. The behaviour of `strtol` in this case
      (run `man strtol` to read the documentation for the function)
      is that the global variable `errno` is set to the constant
      `ERANGE`; the condition on line 41 (`res_errno == ERANGE`)
      evaluates to true,
      and `"9223372036854775808 is outside the range of numbers we
      can handle"` is printed.
    - foo: `strtol` cannot interpret this as a number at all; it sets
      its parameter `&end` to be the same as `argv[0]`. (`end` is
      set to store the address of the "first invalid character" in
      `argv[0]`. Since there are *no* valid characters in `argv[0]`,
      it therefore ends up pointing to the start of that string.
      As a result, the condition on line 38 (`end == argv[0]`) evaluates
      to true,
      and `Error: couldn't interpret 'foo' as a number` is printed.
    - foo bar: Two arguments are being supplied on the command line,
      so the condition in line 26 (`argc != 1`) evaluates to true,
      and
      `Error: expected 1 command-line argument (a YEAR), but got 2`
      is printed.

    [proleptic-cal]: https://en.wikipedia.org/wiki/Proleptic_Gregorian_calendar

    [^britain-gregorian]: In Britain in 1752, the British calendar
      differed from the Gregorian calendar by 11 days.
      During the changeover, those 11 days "disappeared" --
      the day after 2 September 1752 was 14 September 1752.


    </div>
    


5.  The code in `test_leap.c` contains an error. The correct algorithm
    for determining whether a year is a leap year is outlined at
    <https://en.wikipedia.org/wiki/Leap_year#Algorithm>, but the code in
    `test_leap.c` incorrectly reports that (for instance) 1900 is a leap
    year, when it is not. (Can you spot any other errors? Feel free to
    post in the Help3007 forum if you can.)

    Fix the code in `test_leap.c` and test your changes by trying the
    values from step 3 again.

    Compare your changes with another student's – are there any
    differences between how you fixed the program?

    

    <div class="solutions">

    **Answer**

    An improved version of `is_leap` could be implemented
    as follows:

    ```{.C .numberLines}
    int is_leap(long year) {

      if (year % 4 != 0) {
        return 0;
      }

      if (year % 100 == 0) {
        return year % 400 == 0;
      }

      return 1;
    }
    ```

    </div>
    


## 5. Moodle signup and/or login

For upcoming quizzes and tests, we'll be using the CSSE Moodle server,
available at <https://quiz.jinhong.org>.

There is currently one Moodle quiz available -- it is not assessed, but you
should attempt it, as the material covered is examinable later.
You can use that quiz to revise or refresh your knowledge of C.

Visit <https://quiz.jinhong.org> and sign up
with your UWA email address, then attempt the quiz
in your own time, if you don't finish it in the lab.


<!-- vim: syntax=markdown tw=92 smartindent :
-->
