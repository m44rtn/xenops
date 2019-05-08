# Xenops

Xenops is a tool that can help with incrementing the version number in a file for your project. It assumes the file uses the C standard for defining values `#define NAME <value>` but it doesn't matter if it's actually a C file or not (a txt file for example, would work too).


## What it can change
Xenops can change the BUILD/MINOR/MAJOR numbers in a file. It cannot do both at the same time yet, so you have to run both commands seperately like this:

`xenops --build`

`xenops --major`

Instead of:

`xenops --build --major`

*EDIT:* since commit [2244fca2](https://github.com/m44rtn/xenops/commit/2244fca2402adebbf88ab526624df471e0220ea7) you can do both at the same time by running `xenops --build --major [other commands]`

## Usage
Xenops can be used like this: `xenops --file [file-loc] (--prefix [prefix] --major --minor --build)`. To run xenops an input file is required. You can do specify one by running `xenops --file <location to file> [other commands]`. If you don't specify `--major`, `--minor` or `--build`, the latter one (`--build`) is always the default if nothing is specified. In case you specified either MAJOR or MINOR, and want xenops to change the BUILD too you have to specify it.

Xenops searches for the keywords `BUILD`, `MINOR` or `MAJOR` depending on the input command. Xenops also supports a prefix to the keywords. An example of this would be HI_ in `#define HI_BUILD 0`. If you use a prefix, xenops will fail at finding the keywords if you don't specify it. To specify a prefix run `xenops --prefix <prefix> [other commands]`, xenops will then ignore these prefixes.

In the file you need to use the keywords, otherwise xenops fails. 
For an example, take a look at example.h in the repo.

## How to install and run xenops
You can use the makefile to build the source. Just use 
`make build`.
You can now use xenops in it's current directory using
`./xenops [commands]`

Additionally if you'd like to be able to use it anywhere, you can put it in a folder and edit home/username/.bashrc
to include the folder. For example, say you have xenops in a folder `home/username/custom_bin`
and you'd like to be able to run it anywhere you can add the following line to the .bashrc file in your home directory:
`PATH=$PATH:/home/username/custom_bin`. After doing this, you can run xenops from any directory using
`xenops [commands]`.

## Feedback 'n such
Feedback and contributions are very welcome.

