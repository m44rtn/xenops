> oh no, we got there... the moment at which this piece of software
> actually needs an accompanying manual to explain how it works :/


# XENOPS Reference Manual
> aka the REFMAN

Welcome to the reference manual (REFMAN) of XENOPS! This markdown file will explain how XENOPS works in great detail, at least: that's the plan. First chapter will explain why XENOPS exists and what it is. After that, in chapter 2, all supported commands are explained. In chapter 3, you can find the information on where the license to this software can be found.

## 1. Introduction
XENOPS is a command line utillity that makes it easier to not care about the version number of your projects. XENOPS will care about the version number for you. Probably since it has nothing better to do.

XENOPS is compatible with the `MAJOR.MINOR.BUILD` versioning scheme. It can change any one of them as long as they're defined in a file.

## 2. Supported commands

In this chapter all available commands are listed and explained. Every command has it's own 'chapter'. 

XENOPS uses the keywords `BUILD`, `MINOR` and `MAJOR` for finding the specifc version numbers (types) in a file.

Each version number type (BUILD, MINOR and MAJOR) has its own parent, except MAJOR. This means that MINOR is the parent of BUILD and that MAJOR is the parent of MINOR.

### 2.1. File (-f, --file)
> usage: `-f [path_to_file]` OR `--file [path_to_file]`

> example: `-f example.h` OR `--file example.h`

This command is used to specify which file should be read and written to. This file should already have the
`BUILD`, `MINOR` and/or `MAJOR` types defined (depending on which XENOPS is going to change).

### 2.2. Build (-b, --build) [DEFAULT]
> usage: `-b` OR `--build`

This command is used to specify that the `BUILD` number should be increased every time XENOPS runs. In case of BUILD,
this is already the default and doesn't need to be specified unless you also want to increase other version numbers (MINOR/MAJOR). 

To succesfully change the BUILD number, your define line should meet the following criteria:

- At the start of the line there should be something like `#define` or `%define` (it could really be anything, for example `1`, `//` or `ILikeCake`, as long as it's followed by a space)

- Something containing the word BUILD (if there is a prefix to the word BUILD, refer to *chapter 2.7* of the manual), nothing can be after the word BUILD except a space

- A number in decimal format (e.g. `0`, `1` or `11112233`)

> Example: `#define BUILD 0` or `ILikeCake HAHA_BUILD 0`

> NOT: `#define BUILD_HAHA 0xFF` or `BUILD 0`

### 2.3. Minor (-mi, --minor)
> usage: `-mi` OR `--minor`

This command is used to specify that the `MINOR` number should be increased every time XENOPS runs. If you use this command and you also want to increase the BUILD number you have to use the BUILD command also.

To succesfully change the MINOR number, your define line should meet the following criteria:

- At the start of the line there should be something like `#define` or `%define` (it could really be anything, for example `1`, `//` or `ILikeCake`, as long as it's followed by a space)

- Something containing the word MINOR (if there is a prefix to the word the word MINOR, refer to *chapter 2.7.* of the manual), nothing can be after the word MINOR except a space

- A number in decimal format (e.g. `0`, `1` or `11112233`)

> Example: `#define MINOR 0` or `ILikeCake HAHA_MINOR 0`
> NOT: `#define MINOR_HAHA 0xFF` or `MINOR 0`

### 2.4. Major (-ma, --major)
> usage: `-ma` OR `--major`

This command is used to specify that the `MAJOR` number should be increased every time XENOPS runs. If you use this command and you also want to increase the BUILD number you have to use the BUILD command also.

To succesfully change the MAJOR number, your define line should meet the following criteria:

- At the start of the line there should be something like `#define` or `%define` (it could really be anything, for example `1`, `//` or `ILikeCake`, as long as it's followed by a space)

- Something containing the word MAJOR (if something is before the word MAJOR, refer to *chapter 2.7* of the manual), nothing can be after the word MAJOR except a space

- A number in decimal format (e.g. `0`, `1` or `11112233`)

> Example: `#define MAJOR 0` or `ILikeCake HAHA_MAJOR 0`
> NOT: `#define MAJOR_HAHA 0xFF` or `MAJOR 0`

### 2.5. Overflow (-o, --overflow)
> usage `-o [b, mi]` OR `--overflow [b, mi]`

> example `-o b` OR `-o mi` OR `--overflow mi, b`

This command is used to indicate that you want to set an overflow rule on a specific version number. OVERFLOW is used together with LIMIT (refer to *chapter 2.6* of the manual).

An overflow rule is used to set a maximum limit to a specific version number. After the limit is reached, XENOPS will roll the number over to its parent in the version.

You can set an overflow rule on BUILD (`b`) or MINOR (`mi`). Note that you can't specify MAJOR, because nothing exists after
MAJOR (so a rollover to the next parent is not possible).

Say, you have the limit for BUILD set to 90 and you run XENOPS this:

```C
#define BUILD 90
#define MINOR 0
```

Will become this:

```C
#define BUILD 0
#define MINOR 1
```

Note: the parent should already be defined when using this command.

The rollover will happen to its direct parent. This means that if a limit is set on BUILD, XENOPS will rollover to MINOR. You cannot, at the time of writing, rollover from BUILD to MAJOR.

### 2.6. Limit (-l, --limit)
> usage `-l [#, b=#, mi=#]` OR `--limit [#, b=#, mi=#]`

> example `-l 20` OR `-l mi=3` OR `--limit mi=2, b=1024`

This command is used to indicate what a maximum limit is for a version number. LIMIT is used together with OVERFLOW.

Using LIMIT is pretty simple, you can either:

- Specify a limit by specifying which type it refers to (BUILD (`b`) or MINOR (`mi`)) and put `=[limit]` after it (e.g. `b=10` to set the limit for BUILD to 10); OR

- Just setting the limit without specifying the type, which sets the limit globally for both BUILD and MINOR (e.g. `-l 20` to set the limit for MINOR and BUILD to 20)

The limit is set to the number it may be at maximum. Meaning that XENOPS will propagate at `> [limit]`. If you set the limit to 20 for BUILD, XENOPS will count up to 20. Only when it's about to become 21 it will rollover.

Example:

(XENOPS is run) --> result:

```C
#define BUILD 19
#define MINOR 0
```

(XENOPS is run again) --> result:

```C
#define BUILD 20
#define MINOR 0
```

(XENOPS is run again again) --> result:

```C
#define BUILD 0
#define MINOR 1
```

### 2.7. Prefix (-p, --prefix)
> usage `-p [prefix]` OR `--prefix [prefix]`

> example `-p HAHA_` OR `--prefix HAHA_`

This command is used to specify what the prefix is of the type XENOPS needs to change. For example if, in the file, you have specified:

`#define HAHA_BUILD 0`

You will need to specify the prefix to XENOPS, otherwise it can't find the BUILD keyword. To do this, simply pass `-p HAHA_` to XENOPS. It will then be able to find and increase `HAHA_BUILD` in the file.

### 2.8. Quiet (-q, --quiet)
> usage `-q` OR `--quiet`

This command is used to have XENOPS shut up. If specified, XENOPS will not print any output. This can be handy in a 'no news == good news` kind of makefile.

### 2.9. Version (-v, --version)
> usage `-v` OR `--version`

This command will output the version of XENOPS. If specified, XENOPS will ignore all other commands and no changes will be made to any file.

Version numbers will typically look something like this:

```
XENOPS 4 Mark I
Copyright (c) 2020 - MIT Licensed
```

The `Mark` refers to which revision of the XENOPS version you have (in this case the version is `4`), where `Mark I` is actually the first release ever of that version.

In some cases, after the `Mark #`, you may find the characters `RC`. This refers to 'Release Candidate' and is basically a glorified test version.

### 2.10. Help (-h, --help)
> usage `-h` OR `--help`

This command will output a short description of all available commands in XENOPS. If specified, XENOPS will ignore all other commands and no changes will be made to any file.

## 3. License
You can find the (original) license in the source code of XENOPS, in a file called 'LICENSE'. If that file is nowhere to be found, refer to [this link](https://github.com/m44rtn/xenops/blob/master/LICENSE).