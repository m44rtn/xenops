# Xenops

## How to install and run xenops
You can use the makefile to build the source. Just use 
`make build`
You can now use xenops in it's current directory using
`./xenops [commands]`

Additionally if you'd like to use it everywhere, you can put it in a folder and edit home/username/.bashrc
to include the folder. For example, say you have xenops in a folder `home/username/custom_bin`
and you'd like to run it anywhere you can add the following line to the .bashrc file in your home directory:
`PATH=$PATH:/home/username/custom_bin`. After doing this, you can run xenops from any directory using
`xenops [commands]`

## TODO
* write documentation and readme

* refactor and comment
