# Hexagon virtual machine

What is this? If you know what a virtual machine is and how it works, just go to the info below. If not, a virtual machine is like a computer inside your computer. There is no need to install a lot of different developer tools, checking versions, and having in mind a lot of things that you don't know about if your computer is not used to develop software.

The solution is to have a virtual machine inside your computer, with whatever operating system the original developer (me) used, that will be configured automatically, and you will connect from your *real* computer to this *fake* one, the same as if you were connecting to a remote mud in some internet server.

In the end, if you don't like what you see, or just get bored of these things, you just remove the virtual machine and *et voilà* everything will be gone and your own computer (called *the host*) won't have changed, being the best option for non-developers.

So, how to do this?

## Developer how-to (using a virtual machine)

### Initial steps

Software required to be installed: just these two things that allow installing and configuring virtual machines, the same for any operating system: [VirtualBox](https://www.virtualbox.org/wiki/Downloads) & [Vagrant](https://www.vagrantup.com/downloads.html).

And having access to some kind of command-line terminal.

  * In Windows, execute (`Windows Key + r`) the command `cmd`. 
  * In Mac OS, open the Terminal App (or use `Cmd + spacebar` and type `Terminal`).
  * In any kind of Unix/Linux, you know what we are talking about (and maybe you should just use the basic [installation guide](/docs/install/readme.md), not the virtual machine tutorial).

### Installation

1. Download this repository to your local/host machine in your preferred directory, with:

   `git clone https://github.com/houseofmaldorne/hexagon.git` or, if you don't have git installed (why would you?), just download it from github as a zip file from [the project page](https://github.com/houseofmaldorne/hexagon) with the big green button that says *Code -> Download Zip*, and unzip it. 

2. Go to the directory where you placed the contents of that zip file, using the terminal and the command `cd`:

   - In Windows it would be something like `cd C:\Users\myuser\Downloads\something`. You can use the `dir` command to see the contents of your current directory. Try, you won't break anything.
   - In Mac OS it would be something like `cd /Users/myusername/Downloads/something`. You can use the `ls` command to see the contents of your current directory.

3. Go to the `provisioning` directory and install the virtual machine with:
    
   ```sh
   cd provisioning
   vagrant up --provider virtualbox
   ```

   This will:

   + Install needed packages
   + Set initial configuration of the project
   + Start the mud so you can test it

The app could be tested in [`telnet://192.168.10.192:6047/`](telnet://192.168.10.192:6047/)
If you want to use a different ip, it's defined in `Vagrantfile`

### Recreating the virtual machine (something went wrong?)

Once you have the development enviroment up and running, any change could be restored to the default init status with:

1. Destroy the virtual machine. In the virtual machine directory execute: 

   `vagrant destroy`

2. Reset the virtual machine:

   `vagrant up --provision`

   In a pair of minutes everything will be reinstalled: packages, configuration, etc.
