---
title: |
  Using UTM on M-series Mac laptops
header-includes: |
  ```{=html}
  <style>
  div#agreement img {
    border: solid 0.5pt hsl(0 0% 80% / 0.5);
  }
  </style>
  ```  
---

## 1. Installing UTM

Visit <https://mac.getutm.app/>, then click "Download".  (Downloading from the UTM website
is more reliable than following the "Mac App Store" link.)

Once downloaded, locate and select the .dmg file you have downloaded.
A dialog box should appear. Click and drag the icon to your Applications folder.

## 2. Selecting a virtual machine

You'll need to download one or more virtual machine images, and then create virtual
machines from them that you'll use as a development environment. (You can download
more than one, and you can delete any downloaded image if you like and try again.)

There are many options for downloading a virtual machine image -- we outline three
possibilities.

<div style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

::: block-caption

Use an ARM64 image

:::

:   The ["gallery" of pre-built UTM virtual machines][gallery] doesn't include any for Ubuntu 20.04,
    but it *does* include one for Ubuntu 22.04, which is very similar.
    You can visit [this
    link](https://mac.getutm.app/gallery/ubuntu-20-04) to download the Ubuntu 22.04 image.
    Clicking the "Open in UTM" link should download the virtual machine image
    and open it using UTM.

    This image uses an [ARM64 processor][arm64], so it is *not* the same as the CITS3007
    standard development environment (SDE) -- see [here](#differences) for some of the
    differences. For most purposes, the virtual
    machine you've downloaded should behave quite similarly to the CITS3007 SDE, but:

    - when you get to lab 4 (in week 5), which covers buffer overflows: if you want to attempt the
      extension tasks for the lab, they *won't* work on an ARM64 virtual machine. They use
      [shellcode][shellcode] that only works on x86-64 processors.
    - when working on the project (due in week 11), you'll need to allow for differences
      between platforms. The biggest difference
      is that the `char` type on an x86-64 platform is *signed*, but on ARM64 platforms
      it is *unsigned*. You may want to make use of GCC's `-fsigned-char` option
      to mimic the x86-64 behaviour (see [here](#differences)). It's also recommended
      that you test your code using a x86-64 virtual machine before submitting.

</div>

[gallery]: https://mac.getutm.app/gallery/

<div style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

::: block-caption

Use an old x86-64 image

:::

:   The ["gallery" of pre-built UTM virtual machines][gallery] includes an image
    for a very old (2013) version of Ubuntu 14.04, which you should be able to use in order to
    test code on an x86-64 platform. However, x86-64 images run *much* more slowly
    than ARM64 images do -- your Mac has to emulate a very different sort of CPU.

    You can visit [this
    link](https://mac.getutm.app/gallery/ubuntu-14-04) to download the Ubuntu 14.04 image.
    Clicking the "Open in UTM" link should download the virtual machine image
    and open it using UTM.

</div>

Another possibility, although it has not been very thoroughly tested, is to use
UTM with a CITS3007 x86-64 [Qcow](https://en.wikipedia.org/wiki/Qcow) image file.

<div style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

::: block-caption

Use the CITS3007 x86-64 Qcow image file

:::

:   A Qcow image file -- a format related to the one UTM uses -- is available for the
    x86-64 CITS3007 standard development environment (SDE).
    However, UTM doesn't provide very clear documentation on how such files can be used.
    Nevertheless, if you wish to try it, and are familiar with the Linux command line,
    some general instructions are:

    - Download the Vagrant `.box` file from [here][cits3007-libvirt-vm] to your machine (the saved file
      should be called `vagrant.box`).
    - Extract the file `box.img` from the `vagrant.box` file using `tar` (`vagrant.box` is a standard
      gzipped tar file).
    - Rename `box.img` to `cits3007.qcow2`.
    - [Import the qcow2][utm-import] file into UTM.

    To log in, use the username and password "vagrant".

</div>

A former CITS3007 student has also provided some tips on working with UTM
[here][ben-v-utm] (PDF).

[ben-v-utm]: /labs/Installing_cits3007_VM_via_UTM_for_mac_computers_with_ARM_based_architecture.pdf 

In the following section of this lab sheet, we discuss some ways that an ARM64
platform can differ from the x86-64 platform.

## 3. Notes on the ARM64 platform

### 3.1 What are some differences between ARM64 vs x86-64 platforms? { #differences }

The standard [CITS3007 development environment][faq-dev-env] runs on the x86-64
architecture, and any code you submit for the CITS3007 project will be compiled and tested
using GCC on the x86-64 architecture.

[M-series][m-series] Mac laptops, however, use the [ARM64][arm64] architecture, which
differs from x86-64 in several ways (note that the following list is **not**
comprehensive!):

- The architectures have a different [instruction set][isa] -- programs compiled
  for one will not run on the other, and [shellcode][shellcode] written for one
  will not work on the other.
- The `char` type can be either signed or unsigned -- on the x86-64 platform, a `char`
  is usually signed, but when targeting the ARM64 architecture, C compilers (including GCC)
  usually default to it being unsigned (for [performance reasons][arm64-dobbs] that are now
  largely historical). \
  &nbsp;&nbsp;&nbsp;  It's possible to force GCC to make the `char` type signed or unsigned by using the
  [`-funsigned-char` and `-fsigned-char`][gcc-char] options.
- [Data structure padding and alignment][align] can [differ][arm64-dobbs] between the two. GCC provides
  ["attributes"][gcc-att] which allow you to force variables or structs to use padding
  and alignment rules you specify. (Doing this can result in generated code being slightly slower,
  but you may not care about the difference.)

You can find comments on some of these differences in the ["Miscellaneous C porting
issues"][arm64-porting] section of the developer documentation for the ARMv7-A series of
processors.

[faq-dev-env]: https://cits3007.arranstewart.io/faq/#cits3007-sde
[m-series]: https://en.wikipedia.org/wiki/Apple_silicon#M_series
[arm64]: https://en.wikipedia.org/wiki/ARM64
[isa]: https://en.wikipedia.org/wiki/Instruction_set_architecture
[shellcode]: https://en.wikipedia.org/wiki/Shellcode
[arm64-dobbs]: https://www.drdobbs.com/architecture-and-design/portability-the-arm-processor/184405435 
[gcc-char]: https://gcc.gnu.org/onlinedocs/gcc-4.8.0/gcc/C-Dialect-Options.html
[align]: https://en.wikipedia.org/wiki/Data_structure_alignment
[gcc-att]: https://gcc.gnu.org/onlinedocs/gcc-4.4.7/gcc/Variable-Attributes.html 
[arm64-porting]: https://developer.arm.com/documentation/den0013/d/Porting/Miscellaneous-C-porting-issues

### 3.2 What platform will submitted projects be tested on?

Projects will be tested on the x86-64 platform, using GCC as a compiler, and the
[CITS3007 standard development environment][faq-dev-env].

### 3.3 How can I ensure my code runs correctly on the x86-64 platform? { #project-requirements }

It's up to you to make sure that you thoroughly test your code and ensure it
runs correctly on the x86-64 platform. Some suggestions as to how you might
do so include:

- Compiling and testing your code using a pre-built [UTM virtual machine][utm-vms] which
  emulates the x86-64 processor. (An example is the pre-built [Ubuntu 14.04][utm-ubu]
  virtual machine.) Note that emulated virtual machines will run quite slowly on a Mac
  ARM64 computer.
- Compiling and testing your code using a virtual machine on [GitPod][gitpod]. (Note that
  on GitPod, you won't be able to [alter the kernel parameters][kernel-params], which is
  required if you want to complete the "extension tasks" for the week 6 lab, on buffer
  overflows, and for the week 8 lab, on race conditions.)
- Referring to published portability guidelines -- one set of such guidelines is provided
  by [Imperial College, London][imp-c-port], and the [GNU project][gnu-c-port] also
  has some suggestions.
- Compiling and testing on an ARM64 platform with a range of different static analysers,
  a range of different compilers (e.g. both GCC and Clang) -- with all appropriate
  warnings enabled -- with a range of different
  [sanitizers][san] enabled, and at a range of different [optimization levels][gcc-opt].
  While this won't ensure your code works correctly on an x86-64 platform, it is likely
  to result in fewer problems once you do try to run it on an x86-64 system.

[uniapps-linux]: https://uniapps.uwa.edu.au/app/763ccd6e-c650-11ee-ac2b-6045bd28c3f3
[utm-vms]: https://mac.getutm.app/gallery/ 
[utm-ubu]: https://mac.getutm.app/gallery/ubuntu-14-04
[cits3007-libvirt-vm]: https://app.vagrantup.com/arranstewart/boxes/cits3007-ubuntu2004/versions/0.1.4/providers/libvirt/unknown/vagrant.box 
[utm-import]: https://man.ilayk.com/gist/utm/
[gitpod]: https://cits3007.arranstewart.io/labs/lab01-gitpod.html
[kernel-params]: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/8/html/managing_monitoring_and_updating_the_kernel/configuring-kernel-parameters-at-runtime_managing-monitoring-and-updating-the-kernel
[imp-c-port]: https://www.doc.ic.ac.uk/lab/cplus/cstyle.html#N10553
[gnu-c-port]: https://www.gnu.org/prep/standards/standards.html#CPU-Portability
[san]: https://github.com/google/sanitizers
[gcc-opt]: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html

<!-- vim: syntax=markdown tw=90 smartindent :
-->
