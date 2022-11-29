#!/usr/bin/env bash

# Variables
# this one is just for printing logs
PROJECT_NAME="Hexagon Mudlib VM"
# path of the directory inside the virtual machine
PROJECT_PATH="/var/muds/hexagon"
DOMAIN_NAME="hexagon.dev"

# Repair "==> default: stdin: is not a tty" message
# Use this fix in the Vagrantfile
  # to fix a bug while provisioning, neverbot
  # fix found here https://github.com/mitchellh/vagrant/issues/1673#issuecomment-28288042
  # config.ssh.shell = "bash -c 'BASH_ENV=/etc/profile exec bash'"

# In order to avoid the message
# "==> default: dpkg-preconfigure: unable to re-open stdin: No such file or directory"
# use " 2> /dev/null" to redirect dpkg-preconfigure error to /dev/null

echo "------------------------------------------------------"
echo " $PROJECT_NAME: updating packages"
echo "------------------------------------------------------"

export DEBIAN_FRONTEND=noninteractive
sudo -E apt-get update 2> /dev/null

echo "------------------------------------------------------"
echo " $PROJECT_NAME: installing developer tools"
echo "------------------------------------------------------"

sudo -E apt-get -y install build-essential curl byacc 2> /dev/null
# reload bash (nvm in path, etc)
# source ~/.bashrc

echo "------------------------------------------------------"
echo " $PROJECT_NAME: checking git submodules"
echo "------------------------------------------------------"

cd $PROJECT_PATH
git submodule update --init --recursive
# git clone --recursive git://github.com/maldorne/hexagon.git

echo "------------------------------------------------------"
echo " $PROJECT_NAME: compiling DGD"
echo "------------------------------------------------------"

cd $PROJECT_PATH
cd driver/dgd/src
make clean
cd $PROJECT_PATH
./install.sh

echo "------------------------------------------------------"
echo " $PROJECT_NAME: editing config.dgd file"
echo "------------------------------------------------------"

cd $PROJECT_PATH
sed -i '/directory = "\/path_to_the_base_directory\/mud";/c\directory = "\/var\/muds\/hexagon\/mud";' driver/config.dgd

echo "------------------------------------------------------"
echo " $PROJECT_NAME: setting hostname"
echo "------------------------------------------------------"

sudo hostname $DOMAIN_NAME
echo "$DOMAIN_NAME" | sudo tee /etc/hostname
# old ubuntus
# sudo service hostname restart 
sudo hostnamectl set-hostname $DOMAIN_NAME

echo "------------------------------------------------------"
echo " $PROJECT_NAME: setting timezone to Europe/Madrid"
echo "------------------------------------------------------"

sudo cp /usr/share/zoneinfo/Europe/Madrid /etc/localtime

echo "------------------------------------------------------"
echo " $PROJECT_NAME: enabling color prompt in terminal"
echo "------------------------------------------------------"

sed -i '/#force_color_prompt=yes/c\force_color_prompt=yes' /home/vagrant/.bashrc

echo "------------------------------------------------------"
echo " $PROJECT_NAME: starting hexagon server..."
echo "------------------------------------------------------"

cd $PROJECT_PATH
./mud.sh &> /dev/null &

echo "------------------------------------------------------"
echo " $PROJECT_NAME: provisioning script ended"
echo "------------------------------------------------------"
