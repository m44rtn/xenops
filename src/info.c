/* MIT License:

Copyright (c) 2019-2021 Maarten Vermeulen
Copyright (c) 2019      Mercury Thirteen (mercury0x000d)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <stdio.h>

#include "../include/info.h"
#include "../include/arg.h"
#include "../include/version.h"

void info_print_help(void)
{
    /* help screen */
    printf("Basic usage: xenops --file [file-loc] (--prefix [prefix] --major --minor --build)\n\n");
    printf("%s, %s\t\t\t\tSpecify input file\n", ARG_FILE_LOC_S, ARG_FILE_LOC_L);
    printf("%s, %s\t\t\t\tUpdate major version number\n", ARG_CHNG_MAJOR_S, ARG_CHNG_MAJOR_L);
    printf("%s, %s\t\t\t\tUpdate minor version number\n", ARG_CHNG_MINOR_S, ARG_CHNG_MINOR_L);
    printf("%s, %s\t\t\t\t(default) Update build number\n", ARG_CHNG_BUILD_S, ARG_CHNG_BUILD_L);
    printf("%s, %s [b,mi]\t\t\tApply an overflow rule (refer to manual)\n", ARG_OVERFLOW_S, ARG_OVERFLOW_L);
    printf("%s, %s [#, b=#, mi=#]\t\tLimit one or more version numbers (refer to manual)\n", ARG_LIMIT_S, ARG_LIMIT_L);
    printf("\n%s, %s [prefix]\t\t\tSpecify what prefix is in front of version type\n", ARG_PREFIX_S, ARG_PREFIX_L);

    printf("\n%s, %s\t\t\t\tNo output\n", ARG_QUIET_S, ARG_QUIET_L);
    printf("%s, %s\t\t\t\tShow version number\n", ARG_VERSION_S, ARG_VERSION_L);
}

void info_version(void)
{
    char *is_rc = XENOPS_VERSION_ISRC ? "-RC" : " ";
    printf("\nXENOPS %i Mark %i%s (build %i)\nCopyright (c) %i - MIT Licensed\n\n", XENOPS_VERSION_MAJOR, XENOPS_VERSION_MINOR, is_rc, XENOPS_VERSION_BUILD, XENOPS_RELEASE_YEAR);
}
