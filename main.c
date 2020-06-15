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
#include <unistd.h>

/* Note to self: reset minor to 0 with every major number */
#define XENOPS_VERSION_MAJOR 4
#define XENOPS_VERSION_MINOR 1
#define XENOPS_VERSION_ISRC  1  /* 1 == is Release Candidate, 0 == is not a Release Candidate */
#define XENOPS_RELEASE_YEAR  2020


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

#define ARG_MINOR                   "mi" /* these will be used to specify which numbers to overflow */
#define ARG_BUILD                   "b"

#define ARG_LIMIT_L                 "--limit"   /* limit the numbers of build/minor/major to a specific number... */
#define ARG_LIMIT_S                 "-l"        /* ...so that the version number may automatically roll-over to the its 'parent'
                                                    (refer to manual for better explanation) */

#define ARG_CHNG_MAJOR_L            "--major"   /* change major version number */
#define ARG_CHNG_MINOR_L            "--minor"   /* change minor version number */
#define ARG_CHNG_BUILD_L            "--build"   /* change build version number (default) */

#define ARG_CHNG_MAJOR_S            "-ma"   /* change major version number */
#define ARG_CHNG_MINOR_S            "-mi"   /* change minor version number */
#define ARG_CHNG_BUILD_S            "-b"   /* change build version number (default) */

#define LIMIT_ARRAY_BUILD       0
#define LIMIT_ARRAY_MINOR       1

#define FLAG_BUILD              1
#define FLAG_MINOR              1 << 1
#define FLAG_MAJOR              1 << 2

#define FLAG_HELP               1 << 3  /* this makes sure we can check if help has been called (for the error checking 'n such) */
#define FLAG_QUIET              1 << 4  /* The quiet option means no output to the shell */
#define FLAG_VERSION            1 << 5  /* Shows the version of Xenops */
#define FLAG_LIMIT_EVERYTHING   1 << 6  /* A limit has been set globally, so we need to keep track of that */

/* maybe these aren't really necessary... */
#define ERROR_NO_FILE           1
#define ERROR_FILE_NOT_EXIST    404

void print_help();
void arguments_parse(unsigned int nargs, char *arguments[]);

void parse_overflow(char **str, unsigned int start, unsigned int end);
void parse_limit(char **str, unsigned int start, unsigned int end);
void store_overflow(char *str);

void parse_file(FILE *file);

char setcurrent(char *type);
long getlimit(char current);
long do_the_trick(long ver_num);

void write_to_file(FILE *file, long line_size, char file_line[512]);
int digit_count(long value);

void GrowFile(FILE *file);
void ShrinkFile(FILE *file, unsigned int bytes);

typedef struct
{
    char* file;
    char *prefix; 

    unsigned int flags;
        
    unsigned int prefix_len;

    /* overflow and limit stuff */
    unsigned char whatToOverflow;
    unsigned int limit[2];
} tARG_INFO;

tARG_INFO argument_info;

void print_help()
{
    /* help screen */
    printf("Basic usage: xenops --file [file-loc] (--prefix [prefix] --major --minor --build)\n\n\n");
    printf("%s, %s\t\t\t\tLets me know which file to use\n", ARG_FILE_LOC_S, ARG_FILE_LOC_L);
    printf("\n%s, %s\t\t\t\tThis lets me know I should increment the major version number as well\n", ARG_CHNG_MAJOR_S, ARG_CHNG_MAJOR_L);
    printf("%s, %s\t\t\t\tThis lets me know I should increment the minor version number as well\n", ARG_CHNG_MINOR_S, ARG_CHNG_MINOR_L);
    printf("%s, %s\t\t\t\t(default) This lets me know I should increment the build version number as well\n", ARG_CHNG_BUILD_S, ARG_CHNG_BUILD_L);
    printf("%s, %s [b,mi]\t\t\tLets me know on which version number to apply an overflow rule (refer to manual)\n", ARG_OVERFLOW_S, ARG_OVERFLOW_L);
    printf("%s, %s [#, b=#, mi=#]\t\tLets me know what the limit is for the version number (refer to manual)\n", ARG_LIMIT_S, ARG_LIMIT_L);
    printf("\n%s, %s [prefix]\t\t\tThis lets me know if there's something in front of major/minor/build (for example VER_BUILD instead of BUILD)\n", ARG_PREFIX_S, ARG_PREFIX_L);

    printf("\n%s, %s\t\t\t\tXENOPS won't output any text to the shell\n", ARG_QUIET_S, ARG_QUIET_L);
    printf("%s, %s\t\t\t\tShows version number\n", ARG_VERSION_S, ARG_VERSION_L);
}

void version()
{
    char *is_rc = XENOPS_VERSION_ISRC ? "-RC" : " ";
    printf("\nXENOPS %i Mark %i%s\nCopyright (c) %i - MIT Licensed\n\n", XENOPS_VERSION_MAJOR, XENOPS_VERSION_MINOR, is_rc, XENOPS_RELEASE_YEAR);
}

int main(unsigned int nargs, char *args[])
{

    /* TODO:
        feature backlog:
            - Upon rollover or change of parent, automatically reset the other number to zero [done]
            - Automatic rollover using: [done]
                a) overflow, which enables overflow for specified numbers
                b) limit, which rolls over the specified numbers of [overflow] at the specified limit
        fixes:
            - better help [done]
            - shrinkfile shrinks one byte too many? [done]
            - clean this shit up [no]
        misc:
            - update xenops version
            - make a manual explaining xenops (oh god, we got there) --> maybe in html
            - test xenops to pieces [done]
    */

    /* set the argument_info struct up in such a way we can detect errors and prevent them */
    argument_info.file           = NULL;
    argument_info.flags          = 0;
    argument_info.prefix_len     = 0;
    argument_info.whatToOverflow = 0;

    /* limits for every -l argument ('b' or 'mi') */
    argument_info.limit[0] = argument_info.limit[1] = 0;

    /* parse the arguments (it probably isn't called 'parsing', but I'll use the word anyway) */
    arguments_parse((nargs - 1), &args[1]);

    if((argument_info.flags & FLAG_HELP))
    {
        print_help();
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
    char *ptr;

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

        /* overflow and limit */
        else if(!strcmp(arguments[i], ARG_OVERFLOW_L) || !strcmp(arguments[i], ARG_OVERFLOW_S))
            parse_overflow(arguments, i+1, nargs);
        else if(!strcmp(arguments[i], ARG_LIMIT_L) || !strcmp(arguments[i], ARG_LIMIT_S))
            parse_limit(arguments, i+1, nargs);
        
    }
}

/* stores on which things overflow is enabled (MAJOR/MINOR/BUILD) */
void parse_overflow(char **str, unsigned int start, unsigned int end) 
{
    unsigned int i = start;
    char *ptr = (char *) 1;

    while(ptr && i < end)
    {
        /* store what is specified in this string piece */
        store_overflow(str[i]);

        /* is there a comma? if not this is the last time we loop... */
        ptr = strchr(str[i], ',');
        
        ++i;
    }
}

void parse_limit(char **str, unsigned int start, unsigned int end)
{
    unsigned int i = start;
    char *ptr = (char *) 1;
    unsigned int where;

    while(ptr && i < end)
    {
        /* store what is specified in this string piece */
        store_overflow(str[i]);

        if(where = (unsigned long) strstr(str[i], ARG_BUILD))
        {
            where = (where - (unsigned long) str[i]) + strlen(ARG_BUILD)+1;
            argument_info.limit[LIMIT_ARRAY_BUILD] = strtoul(&str[i][where], NULL, 10);
        }
        if(where = (unsigned long) strstr(str[i], ARG_MINOR))
        {
            where = (where - (unsigned long) str[i]) + strlen(ARG_MINOR)+1;
            argument_info.limit[LIMIT_ARRAY_MINOR] = strtoul(&str[i][where], NULL, 10);
        }

        /* is there a comma? if not this is the last time we loop... */
        ptr = strchr(str[i], ',');
        
        ++i;
    }

    /* no specific thing was selected so it was set globally, so make sure we honor that */
    if(!argument_info.limit[LIMIT_ARRAY_BUILD] && !argument_info.limit[LIMIT_ARRAY_MINOR])
    {
        argument_info.limit[LIMIT_ARRAY_BUILD] = strtoul(str[start], NULL, 10);
        argument_info.flags |= FLAG_LIMIT_EVERYTHING;
    }    
}

void store_overflow(char *str)
{
    if(strstr(str, ARG_BUILD))
        argument_info.whatToOverflow |= FLAG_BUILD;
    if(strstr(str, ARG_MINOR))
        argument_info.whatToOverflow |= FLAG_MINOR;
}

void parse_file(FILE *file)
{
    char file_str[512], define[512], type[512];
    char current, hasOverflowed = 0, flowTo = 0;
    int ignore;
    long nver, limit;
    size_t line_len;
    
    /* it may be nice to clean this function up sometime */

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
            current = setcurrent(&type[argument_info.prefix_len]);
            limit = getlimit(current);

            /* okay, listen up: this checks if the following are true (in this order):
                    - limit > 0 (if so, a limit has been set, otherwise we can safely ignore this whole thing)
                    - is the current thing we are checking set by the overflow tag (if not, we shouldn't care about the limit)
                    - is the current version number the same as the set limit
                
                if all is true, set the version number to 0 (that's a zero)
            */      
            if(limit && (current == (argument_info.whatToOverflow & current)) 
                && (nver >= limit)) 
                {
                    if(digit_count(nver) > 1)
                        ShrinkFile(file, digit_count(nver));

                    nver = 0;
                    
                    /* indicate that we overflowed and make sure we also now increase the next version thing */
                    argument_info.flags |= current << 1;
                    hasOverflowed++;
                    flowTo |= current << 1;
                }
            else
            {
                if (digit_count(nver) < digit_count(nver + 1))
                    GrowFile(file);
            
                
                nver = do_the_trick(nver);
            }

            /* check if we had to overflow to this one */
            if(hasOverflowed && ((flowTo & current) == current))
                hasOverflowed = 0;
            
            
            ignore = sprintf(file_str, "%s %s %ld", define, type, nver);
            write_to_file(file, line_len, file_str); 

            if(!(argument_info.flags & FLAG_QUIET)) printf("%s increased to %ld\n", type, nver);
        }
    }

    /* if hasOverflow was never reset to 0, then we haven't found it yet 
        (someone is weird enough to put everything in a weird order)... so we have to
        try again, but disable the things we have already done.
        
        I know this isn't the best solution */
    if(hasOverflowed)
    {
        argument_info.flags = (argument_info.flags & FLAG_QUIET) | flowTo;
        fseek(file, 0, SEEK_SET);
        parse_file(file);
    }
}

/* returns what we're currently working on */
char setcurrent(char *type)
{
    if(!strcmp(&type[argument_info.prefix_len], "BUILD"))
        return FLAG_BUILD;
    if(!strcmp(&type[argument_info.prefix_len], "MINOR"))
        return FLAG_MINOR;
    if(!strcmp(&type[argument_info.prefix_len], "MAJOR"))
        return FLAG_MAJOR;
}

long getlimit(char current)
{
    if(current & FLAG_BUILD || (argument_info.flags & FLAG_LIMIT_EVERYTHING == FLAG_LIMIT_EVERYTHING))
        return (long) argument_info.limit[LIMIT_ARRAY_BUILD];
    if(current & FLAG_MINOR)
        return (long) argument_info.limit[LIMIT_ARRAY_MINOR];

    return 0;
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
    int returnValue;


    if (value >= 0 && value <= 9)
    {
        returnValue = 1;
    }

    if (value >= 10 && value <= 99)
    {
        returnValue = 2;
    }

    if (value >= 100 && value <= 999)
    {
        returnValue = 3;
    }

    if (value >= 1000 && value <= 9999)
    {
        returnValue = 4;
    }

    if (value >= 10000 && value <= 99999)
    {
        returnValue = 5;
    }

    if (value >= 100000 && value <= 999999)
    {
        returnValue = 6;
    }

    if (value >= 1000000 && value <= 9999999)
    {
        returnValue = 7;
    }

    return returnValue;
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

void ShrinkFile(FILE *file, unsigned int bytes)
{
    int ignore;
    long initialFilePosition, readFilePosition, stopFilePosition, writeFilePosition;
    unsigned char currentByte;

    /* get the current position so that we can reset after doing what we're going to do */
    initialFilePosition = ftell(file);

    readFilePosition = initialFilePosition - 1;
    writeFilePosition = initialFilePosition - bytes;

    fseek(file, 0, SEEK_END);
    stopFilePosition = ftell(file);

    /* copy all bytes to their new spot */
    while (readFilePosition < stopFilePosition)
    {
        fseek(file, readFilePosition, SEEK_SET);
        currentByte = fgetc(file);

        fseek(file, writeFilePosition, SEEK_SET);
        ignore = fputc(currentByte, file);
        
        readFilePosition++;
        writeFilePosition++;
    }
    
    /* here's where we actually make the file smaller */
    ignore = ftruncate(fileno(file), stopFilePosition-bytes);
    
    /* reset the position to our original position so that XENOPS can continue as if nothing happened */
    fseek(file, initialFilePosition, SEEK_SET);
}
