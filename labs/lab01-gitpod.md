---
title: |
  Using GitPod
header-includes: |
  ```{=html}
  <style>
  div#agreement img {
    border: solid 0.5pt hsl(0 0% 80% / 0.5);
  }
  </style>
  ```  
---

[gitpod]: https://gitpod.io/
[vs-code]: https://code.visualstudio.com

If you don't yet have access to a laptop on which you
can install VirtualBox and Vagrant, then for many purposes,
you can also use
a cloud-based development VM provided by [GitPod][gitpod].

(Note that if you use GitPod, you will *not* be able to alter the
[kernel parameters][kernel-params] of the running kernel. Altering the
kernel parameters is
required if you want to complete the "extension tasks" for the week 4 lab, on buffer
overflows, and for the week 8 lab, on race conditions.)

[kernel-params]: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/8/html/managing_monitoring_and_updating_the_kernel/configuring-kernel-parameters-at-runtime_managing-monitoring-and-updating-the-kernel

You should complete the following steps:

1.  You'll need to have a GitHub account for this, so if you don't have
    one already, visit <https://github.com/> and create one.

#.  Log into your GitHub account. Once that's done, paste the following URL into your browser:

    &nbsp; <https://gitpod.io/#https://github.com/cits3007/ubuntu-gitpod>
    
    You'll be prompted to use GitHub to access GitPod:
    
    `<div id="agreement" style="display: flex; justify-content: center;">`{=html}
    ![](images/gitpod-agreement.png "gitpod agreement")
    `</div>`{=html}
    
#.  Click "Continue with GitHub".
    
    GitPod will start a cloud-based virtual machine in which the standard
    CITS3007 environment is available, and the latest version of
    [Visual Studio Code][vs-code] (VS Code) editor is running:
   


    ![](images/gitpod-env.png "gitpod environment")

    </div>
    
    Initially, VS Code will start with the folder `/workspace/ubuntu-gitpod`
    open, which contains code downloaded from
    <https://github.com/cits3007/ubuntu-gitpod>. However, you can easily
    create and open new folders if desired.

Note that when using GitPod, it's up to you to ensure a copy of your code is saved in
some permanent location -- when the virtual machine shuts down, any files you created or
altered will be lost, if they haven't been saved elsewhere.


<!-- vim: syntax=markdown tw=88 smartindent :
-->
