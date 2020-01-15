/* MIT License:

Copyright (c) 2020 Maarten Vermeulen

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
#include <string.h>
#include <stdlib.h>

/* Note to self: reset minor to 0 with every major number */
#define XENOPS_VERSION_MAJOR 3
#define XENOPS_VERSION_MINOR 2
#define XENOPS_RELEASE_YEAR  2020

#define ARG_FILE_LOC_L            "--file"    /* location of the file containing the versioning */
#define ARG_FILE_LOC_S            "-f"    

#define ARG_HELP_L                "--help"    /* guess what this does */
#define ARG_HELP_S                "-h"

#define ARG_QUIET_L               "--quiet"   /* shuts xenops up */
#define ARG_QUIET_S               "-q"  

#define ARG_PREFIX_L              "--prefix"  /* prefix to the major/minor/build numbers in the file */
#define ARG_PREFIX_S              "-p"

#define ARG_VERSION_L		    "--version" /* the version of Xenops */
#define ARG_VERSION_S		    "-v"

#define ARG_CHNG_MAJOR_L          "--major"   /* change major version number */
#define ARG_CHNG_MINOR_L          "--minor"   /* change minor version number */
#define ARG_CHNG_BUILD_L          "--build"   /* change build version number (default) */

#define ARG_CHNG_MAJOR_S          "-ma"   /* change major version number */
#define ARG_CHNG_MINOR_S          "-mi"   /* change minor version number */
#define ARG_CHNG_BUILD_S          "-b"   /* change build version number (default) */

#define FLAG_BUILD              1
#define FLAG_MINOR              1 << 2
#define FLAG_MAJOR              1 << 3

#define FLAG_HELP               1 << 4  /* this makes sure we can check if help has been called (for the error checking 'n such) */
#define FLAG_QUIET              1 << 5  /* The quiet option means no output to the shell */
#define FLAG_VERSION            1 << 6  /* Shows the version of Xenops */

/* maybe these aren't really necessary... */
#define ERROR_NO_FILE           1
#define ERROR_FILE_NOT_EXIST    404

void help();
void arguments_parse(unsigned int nargs, char *arguments[]);
void parse_file(FILE *file);
long do_the_trick(long ver_num);
void write_to_file(FILE *file, long line_size, char file_line[512]);
int digit_count(long value);
void GrowFile(FILE *file);

typedef struct
{
    char* file;
    char *prefix; 

    unsigned int flags;
        
    unsigned int prefix_len;
} tARG_INFO;

tARG_INFO argument_info;

void help()
{
    /* help screen */
    printf("usage: xenops --file [file-loc] (--prefix [prefix] --major --minor --build)\n\n\n");
    printf("%s, %s\t\t\t\tLets me know which file to use\n", ARG_FILE_LOC_S, ARG_FILE_LOC_L);
    printf("\n%s, %s\t\t\t\tThis lets me know I should increment the major version number as well\n", ARG_CHNG_MAJOR_S, ARG_CHNG_MAJOR_L);
    printf("%s, %s\t\t\t\tThis lets me know I should increment the minor version number as well\n", ARG_CHNG_MINOR_S, ARG_CHNG_MINOR_L);
    printf("%s, %s\t\t\t\t(default) This lets me know I should increment the build version number as well\n", ARG_CHNG_BUILD_S, ARG_CHNG_BUILD_L);

    printf("\n(%s, %s) [prefix]\t\tThis lets me know if there's something in front of major/minor/build (for example VER_BUILD instead of BUILD)\n", ARG_PREFIX_S, ARG_PREFIX_L);

    printf("%s, %s\t\t\t\tXenops won't output any text to the shell\n", ARG_QUIET_S, ARG_QUIET_L);
    printf("%s, %s\t\t\t\tShow version number\n", ARG_VERSION_S, ARG_VERSION_L);
}

void version()
{
    printf("\nXenops %i Mark %i\nCopyright (c) %i - MIT Licensed\n\n", XENOPS_VERSION_MAJOR, XENOPS_VERSION_MINOR, XENOPS_RELEASE_YEAR);
}

int main(unsigned int nargs, char *args[])
{
    /* set the argument_info struct up in such a way we can detect errors */
    argument_info.file          = NULL;
    argument_info.flags         = 0;
    argument_info.prefix_len    = 0;

    /* parse the arguments (it probably isn't called 'parsing', but I'll use the word anyway) */
    arguments_parse((nargs - 1), &args[1]);

     if((argument_info.flags & FLAG_HELP))
    {
        help();
        return 0;
    }
    else if((argument_info.flags & FLAG_VERSION))
    {
        version();
        return 0;
    }

    /* 0x1F fixes a bug: it makes sure FLAG_QUIET is ignored. Otherwise it causes a problem where when you don't
    specify any other commands (ARG_BUILD etc.) with it, it wouldn't auto-search and change the BUILD and nothing would be altered. */
    if((argument_info.flags & 0x1F) == 0) argument_info.flags |= FLAG_BUILD;

    /* did we get a file? */
    if(argument_info.file == NULL)
    {   
        /* set the color to red, announce it's an error and use the default color again */
        printf("\n\033[0;31mError:\033[0m No input file specified\nUse xenops --help for help\n\n"); 
        return ERROR_NO_FILE;
    }

    /* now we can open the file */
    FILE *file = fopen(argument_info.file, "r+");

    /* did the file exist? */
    if(file == NULL)
    {   
        printf("\n\033[0;31mError:\033[0m Input file does not exist\n\n"); 
        return ERROR_FILE_NOT_EXIST;
    }

    /* now we can do things with it */
    parse_file(file);
    
    fclose(file);
    return 0;
}

void arguments_parse(unsigned int nargs, char* arguments[])
{
    unsigned int i = 0;
    for(i; i < nargs; i++)
    {
        /* if one of the arguments is the help command then set the 
            help flag */
       if(!strcmp(arguments[i], ARG_HELP_L) || !strcmp(arguments[i], ARG_HELP_S)) 
            argument_info.flags = FLAG_HELP;
       if(!strcmp(arguments[i], ARG_VERSION_L) || !strcmp(arguments[i], ARG_VERSION_S))
            argument_info.flags = FLAG_VERSION;
    
        /* if the arguments contain a file location, store it */
        else if(!strcmp(arguments[i], ARG_FILE_LOC_L) || !strcmp(arguments[i], ARG_FILE_LOC_S))
            argument_info.file = arguments[i + 1];
        
        /* store the prefix */
        else if(!strcmp(arguments[i], ARG_PREFIX_L) || !strcmp(arguments[i], ARG_PREFIX_S))
            argument_info.prefix_len = strlen(arguments[i + 1]);            
        
        /* look what needs to be changed within the file */
        else if(!strcmp(arguments[i], ARG_CHNG_BUILD_L) || !strcmp(arguments[i], ARG_CHNG_BUILD_S))
            argument_info.flags |= FLAG_BUILD;
        else if(!strcmp(arguments[i], ARG_CHNG_MINOR_L) || !strcmp(arguments[i], ARG_CHNG_MINOR_S))
            argument_info.flags |= FLAG_MINOR;
        else if(!strcmp(arguments[i], ARG_CHNG_MAJOR_L) || !strcmp(arguments[i], ARG_CHNG_MAJOR_S))
            argument_info.flags |= FLAG_MAJOR;     

        else if(!strcmp(arguments[i], ARG_QUIET_L) || !strcmp(arguments[i], ARG_QUIET_S))
            argument_info.flags |= FLAG_QUIET;       
    }
}

void parse_file(FILE *file)
{
    char file_str[512], define[512], type[512];
    int ignore;
    long nver;
    size_t line_len;

    /* There is probably a better way to do this */
    while(fgets(file_str, 512, file))
    {
        line_len = strlen(file_str);
        ignore = sscanf(file_str, "%s %s %ld", define, type, &nver);

        /* if we didn't fill enough variables, we may as well cut it short RIGHT NOW! :) */
        if (ignore < 3)
            continue;

        /* check if the current line is either the BUILD/MINOR/MAJOR and if the flag for it is set*/
        long isBuild = !strcmp(&type[argument_info.prefix_len], "BUILD") && (argument_info.flags & FLAG_BUILD);
        long isMinor = !strcmp(&type[argument_info.prefix_len], "MINOR") && (argument_info.flags & FLAG_MINOR);
        long isMajor = !strcmp(&type[argument_info.prefix_len], "MAJOR") && (argument_info.flags & FLAG_MAJOR);

        /* if it is, change it */
        if(isBuild || isMinor || isMajor)
        {

            /* If incrementing the value will cause it to gain a digit in length, we need to grow the file to avoid overwriting data. */
            if (digit_count(nver) != digit_count(nver + 1))
               GrowFile(file);

            nver = do_the_trick(nver);
            ignore = sprintf(file_str, "%s %s %ld", define, type, nver);
            write_to_file(file, line_len, file_str); 

            if(!(argument_info.flags & FLAG_QUIET)) printf("Changed %s to %ld\n", type, nver);
        }
    }
}

long do_the_trick(long ver_num)
{
    ver_num++;    
    return ver_num;
}

void write_to_file(FILE *file, long line_size, char file_line[512])
{
    fseek(file, -(line_size), SEEK_CUR);
    fputs(file_line, file);
}

int digit_count(long value)
{
    int retval = 1;
    
    while(!((value /= 10) < 10))
        retval++;

    return retval;  
}

void GrowFile(FILE *file)
{
    /* inserts a byte into the file specified at the location specified */

    int ignore;
    long initialFilePosition, readFilePosition, stopFilePosition, writeFilePosition;
    unsigned char currentByte;

    initialFilePosition = ftell(file);
    stopFilePosition = initialFilePosition - 3;

    fseek(file, 0, SEEK_END);

    readFilePosition = ftell(file);
    writeFilePosition = readFilePosition;

    readFilePosition--;

    /* a loop to expand the rest of the file down by one byte */
    while (stopFilePosition < readFilePosition)
    {
        fseek(file, readFilePosition, SEEK_SET);
        currentByte = fgetc(file);

        fseek(file, writeFilePosition, SEEK_SET);
        ignore = fputc(currentByte, file);
        
        readFilePosition--;
        writeFilePosition--;
    }

    /* reset the original file position before we leave */
    fseek(file, initialFilePosition, SEEK_SET);
}
