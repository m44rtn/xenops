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

#ifndef __FLAG_H__
#define __FLAG_H__

#define TYPE_FLAG               1
#define FLAG_BUILD              TYPE_FLAG
#define FLAG_MINOR              TYPE_FLAG << 1
#define FLAG_MAJOR              TYPE_FLAG << 2

#define FLAG_HELP               1 << 3  /* this makes sure we can check if help has been called (for the error checking 'n such) */
#define FLAG_QUIET              1 << 4  /* The quiet option means no output to the shell */
#define FLAG_VERSION            1 << 5  /* Shows the version of Xenops */
#define FLAG_LIMIT_EVERYTHING   1 << 6  /* A limit has been set globally, so we need to keep track of that */

#endif // __FLAG_H__
