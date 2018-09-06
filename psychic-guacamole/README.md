# Tasty guacamole gets things done!

Here's a list of commands to get you up and running, along with some challenges to make you learn the ropes.

If you're new to Unix and/or Git, you may find the following basic commands useful to navigate around.
First, clone (download) this repository to your computer using `git clone https://github.com/MITMotorsports/MY18`. Then switch to the new-members branch using `git checkout new-members`. Plug in the development board you were given to your computer. Finally, `cd` to the respective exercise directory and run `make -j 10` to compile your code and `make flash` to upload it to the microcontroller to run!

This [FAQ](https://github.com/MITEVT/opel_EE/wiki/FAQ-about-LPC11CXX-Microcontrollers) is also good place to read if you're confused or stuck.


## Unix Cheatsheet

`.` - current directory

`..` - reference to directory one level up

`cd [directory]` - change current directory

`ls [folder]` - list files

`mkdir [directory name]` - make directory

`grep "search term" [directory to search] -rn` - search for a specific term in all the files in a directory (recursively)


## Git Setup and Guide

Read up on the [process wiki](https://github.com/mitmotorsports/process/wiki) to learn how to use Git and GitHub more in depth, although you might not need to today.

You will **not** need to worry about SSH keys yet, as you will not need to push code to GitHub today. Just clone this repository with HTTPS.

Very useful Git Cheat Sheet [here](https://services.github.com/on-demand/downloads/github-git-cheat-sheet.pdf).


## Three Exercises:
1. Blink Exercise

  Make an LED blink on and off by writing a delay function. Debug LEDs are super important!

2. UART Echo Exercise

  If you run `make com` after you `make flash`, a serial I/O connection over UART/USB is set up with the LPC. After you run `make com`, you can't see any of the characters you type, because the chip isn't sending you any characters. Program the LPC to echo input you type back to your terminal input.

3. CAN Exercise

  Make two LPC microcontrollers talk to each other over CAN, a differential pair signaling system. For a brief introduction to CAN, a good start the [Wikipedia article](https://en.wikipedia.org/wiki/CAN_bus). Ask questions as necessary.
