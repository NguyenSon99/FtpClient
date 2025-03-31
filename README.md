# FtpClient

# Installation Guide

## Overview
This guide provides instructions on how to install QtFtp library 

## Installation Steps
```bash
# Update package lists and upgrade existing packages
sudo apt update && sudo apt upgrade

# Install QtFtp package 
sudo apt install qt5-qmake qtbase5-dev qtftp5-dev
sudo apt install libqt5ftp5
git clone https://code.qt.io/qt/qtftp.git
cd qtftp
qmake
make
sudo make install
