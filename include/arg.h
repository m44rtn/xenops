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

#ifndef __ARG_H__
#define __ARG_H__


#define ARG_FILE_LOC_L              "--file"    /* location of the file containing the versioning */
#define ARG_FILE_LOC_S              "-f"    

#define ARG_HELP_L                  "--help"    /* guess what this does */
#define ARG_HELP_S                  "-h"

#define ARG_QUIET_L                 "--quiet"   /* shuts xenops up */
#define ARG_QUIET_S                 "-q"  

#define ARG_PREFIX_L                "--prefix"  /* prefix to the major/minor/build numbers in the file */
#define ARG_PREFIX_S                "-p"

#define ARG_VERSION_L		        "--version" /* the version of Xenops */
#define ARG_VERSION_S		        "-v"

#define ARG_OVERFLOW_L              "--overflow" /* enable overflow for specific numbers (for limit)... */
#define ARG_OVERFLOW_S              "-o"         /* ...when using --limit, default is all */

#define LIMIT_OVERLOW_ARG_MINOR                   "mi" /* these will be used to specify which numbers to overflow */
#define LIMIT_OVERLOW_ARG_BUILD                   "b"

#define ARG_LIMIT_L                 "--limit"   /* limit the numbers of build/minor/major to a specific number... */
#define ARG_LIMIT_S                 "-l"        /* ...so that the version number may automatically roll-over to the its 'parent'
                                                    (refer to manual for better explanation) */

#define ARG_CHNG_MAJOR_L            "--major"   /* change major version number */
#define ARG_CHNG_MINOR_L            "--minor"   /* change minor version number */
#define ARG_CHNG_BUILD_L            "--build"   /* change build version number (default) */

#define ARG_CHNG_MAJOR_S            "-ma"   /* change major version number */
#define ARG_CHNG_MINOR_S            "-mi"   /* change minor version number */
#define ARG_CHNG_BUILD_S            "-b"   /* change build version number (default) */

#endif // __ARG_H__
