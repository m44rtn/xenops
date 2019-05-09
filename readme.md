# Xenops

Xenops is a tool that can help with incrementing the version number in a file for your project. It assumes the file uses the C standard for defining values `#define NAME <value>` but it doesn't matter if it's actually a C file or not (a txt file for example, would work too).

This tool is written for the Linux terminal.


## What it can change
Xenops can change the BUILD/MINOR/MAJOR numbers in a file. It can update all the numbers in one go. You can use
`xenops --build --minor --major` to update all of the numbers

## Usage
Xenops can be used like this: `xenops --file [file-loc] (--prefix [prefix] --major --minor --build)`. To run xenops an input file is required, you can specify one by running `xenops --file <location of file> [other commands]`. The `--build` command is always the default if nothing else is specified. In case you specified either MAJOR (`--major`) or MINOR (`--minor`), and want xenops to change the BUILD too you do have to specify it.

Xenops searches for the keywords `BUILD`, `MINOR` or `MAJOR` depending on the input command. Xenops also supports a prefix to the keywords. An example of this would be HI_ in `#define HI_BUILD 0`. If you use a prefix, xenops will fail at finding the keywords if you don't specify it. To specify a prefix run `xenops --prefix <prefix> [other commands]`, xenops will then ignore these prefixes.

In the file you need to use the keywords, otherwise xenops fails. 
For an example, take a look at example.h in the repo.

## How to install and run xenops
You can use the makefile to build the source. Just use 
`make build`.
You can now use xenops in it's current directory using
`./xenops [commands]`

Additionally, if you'd like to be able to use it anywhere, you can put it in a folder and edit home/username/.bashrc
to include the folder. For example, say you have xenops in a folder `home/username/custom_bin`
and you'd like to be able to run it anywhere you can add the following line to the .bashrc file in your home directory:
`PATH=$PATH:/home/username/custom_bin`. After doing this, you can run xenops from any directory using
`xenops [commands]`.

## Feedback 'n such
Feedback and contributions are very welcome.

