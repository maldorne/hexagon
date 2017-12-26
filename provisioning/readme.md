# hexagon virtual machine

## Developer how-to (using a virtual machine)

Software required: [VirtualBox](https://www.virtualbox.org/wiki/Downloads) && [Vagrant](https://www.vagrantup.com/downloads.html)

1. Download this repository to your local/host machine in your preferred directory, with:

    `git clone git@github.com:houseofmaldorne/hexagon.git`

3. Inside your preferred terminal of your current operating system, go to the `provisioning` 
    directory and install the virtual machine with:

    `vagrant up --provider virtualbox`

    This will:
    
    + Install needed packages packages
    + Initial configuration of the project
    + Start the mud so you can test it
    
4. Optional: add to you `/etc/hosts` file:

    `192.168.10.192 hexagon.dev`
    
    And the application should now be reachable from your host machine at telnet://hexagon.dev:6047
    If you want to use a different ip, it's defined in `Vagrantfile`

The app could be tested in `telnet://192.168.10.192:6047/` 

### Recreating the virtual machine

Once you have the development enviroment up and running, any change could be restored to the default init status with:

1. Destroy the virtual machine. In the virtual machine directory execute: 

    `vagrant destroy`

2. Reset the virtual machine:

    `vagrant up --provision`

    In a pair of minutes everything will be reinstalled: apt packages, etc.
