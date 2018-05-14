# TC2008 Final Project

This repository contains the final project for the TC 2008 - Operating Systems subject in jan-aug 2018 in ITESM Cuernavaca, Mexico. 

# Required software
This project was developed using the following pieces of software>.
*  [Windows Ubuntu Subsystem](https://www.microsoft.com/es-mx/store/p/ubuntu/9nblggh4msv6), to develop in. This should include the nano text editor already built-in. Without this subsystem, the project should be able to run properly in any Unix based distribution.
* gcc ``sudo apt install gcc``
* git ``sudo apt install git``

# Getting started
Once you have your subsystem up and running with gcc and git in it, clone this repository:

``git clone https://github.com/epicLevi/-TC2008-Final-Project.git``

Use gcc to compile both Executive.c and Clients.c with a proper name:

``gcc Executive.c -o Executive``

``gcc Clients.c -o Clients``

Troubleshoot if necessary. However, no compile errors should exists at this time of writing. Now you can execute the programs in a terminal for each running:

``./Executive``

``./Clients``

In that given order, or in a single terminal by using:

``./Executive & ./Clients``
