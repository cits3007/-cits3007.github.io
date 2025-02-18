---
title: |
  Visual Studio Code configuration
---

The standard CITS3007 development environment VM comes with the
`vim` editor already installed, but many people find it more
comfortable developing and debugging code using a graphical
editor.

[Visual Studio Code][vs-code] (VS Code) works well on Windows, Mac
and Linux computers, and allows you to seamlessly write and debug
code on a Vagrant VM.

The following steps should be done on your *host* machine (i.e.
your laptop).
You will need to have a CITS3007 development environment VM already running
(e.g. through invoking the `vagrant up` command).

1.  Install VS Code by downloading it from
    <https://code.visualstudio.com>.

#.  Start VS Code (on Linux or MacOS, the command to do so from the
    terminal is just `code`), and from the menu select 'File' /
    'Preferences' / 'Extensions'. In the search box at the top left of
    the window, type `remote ssh` and hit enter. The top hit should be
    the "Remote - SSH" extension; select it, then click the "install"
    button.

    `<div style="display: flex; justify-content: center;">`{=html}
    ![](images/remote-ssh-extension.png "installing the remote-ssh extension")
    `</div>`{=html}

    In the lower left corner of the window you should see a button with
    facing angle bracket symbols ("><") – click on it:

    `<div style="display: flex; justify-content: center;">`{=html}
    ![](images/new-ssh-config.png "new remote-ssh connection")
    `</div>`{=html}

#.  VS Code will then offer several options -- select "Open SSH
    configuration file".

    ![](images/new-ssh-config2.png "new remote-ssh connection")

    Select the first configuration file VS Code suggests and open it.

#.  Now, in the terminal window from which you started the Vagrant VM
    (that is -- on your *host* machine or laptop),
    type `vagrant ssh-config`.

    <div style="border: solid 2pt orange; border-radius: 5pt; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

    ::: block-caption

    Troubleshooting

    :::

    Note that vagrant commands need to be run from the host machine, not
    from within a VM. If your terminal prompt looks like this:

    ```
    vagrant@cits3007-ubuntu2004:~$
    ```

    then your terminal is currently connect to a VM. You need to
    either exit or switch to a new terminal in order to issue vagrant
    commands.

    </div>

    Vagrant will output configuration settings which can be used with
    `ssh` – something like the following:

    ```
    Host default
      HostName 127.0.0.1
      User vagrant
      Port 2200
      UserKnownHostsFile /dev/null
      StrictHostKeyChecking no
      PasswordAuthentication no
      IdentityFile /home/smithj/cits3007-test/.vagrant/machines/default/virtualbox/private_key
      IdentitiesOnly yes
      LogLevel FATAL
    ```

    Paste this `ssh` configuration into the file which is open in VS
    Code, and save it.  You may want to change the first line from `Host
    default` to something more informative, like `Host cits3007-test`.

    Pasting this configuration informs VS Code about the new VM and how
    to connect to it.

#.  If you again click on the "remote window" button
    in the lower left corner of VS Code, one of the options is "Connect
    to Host", and if you select *that*, one of the options should be the
    new VM you specified (e.g. `cits3007-test`).

    VS Code will open a new window (and spend a few seconds setting up
    some software on the VM): in this window, you can use VS Code as
    normal, but all files you can access will be on the virtual machine.

    [vs-code]: https://code.visualstudio.com


<!-- vim: syntax=markdown tw=72 smartindent :
-->
