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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/version.h"
#include "../include/arg.h"
#include "../include/flag.h"

#include "../include/info.h"

#define LIMIT_ARRAY_BUILD       0
#define LIMIT_ARRAY_MINOR       1

#define BUILD                   "BUILD"
#define MINOR                   "MINOR"
#define MAJOR                   "MAJOR"

#define N_TYPES                 3  // there are three types to check (BUILD, MINOR, MAJOR)

/* maybe these aren't really necessary... */
#define ERROR_NO_FILE           1
#define ERROR_FILE_NOT_EXIST    404

void arguments_parse(unsigned int nargs, char *arguments[]);

void parse_overflow(char **str, unsigned int start, unsigned int end);
void parse_limit(char **str, unsigned int start, unsigned int end);
void store_overflow(char *str);

void parse_file(FILE *file);
long increase(long type, long nver, FILE *file, int *overflowed);

void hasoverflowed(int *hasOverflowed, FILE *file);

long check_type(char *type);
long getlimit(char current);
long do_the_trick(long ver_num, FILE *file);

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
    unsigned char types_to_overlow;
    unsigned int limit[2];
} tARG_INFO;

tARG_INFO argument_info;

int main(unsigned int nargs, char *args[])
{

    /* set the argument_info struct up in such a way we can detect errors and prevent them */
    argument_info.file           = NULL;
    argument_info.flags          = 0;
    argument_info.prefix_len     = 0;
    argument_info.types_to_overlow = 0;

    /* limits for every -l argument ('b' or 'mi') */
    argument_info.limit[0] = argument_info.limit[1] = 0;

    /* parse the arguments (it probably isn't called 'parsing', but I'll use the word anyway) */
    arguments_parse((nargs - 1), &args[1]);

    if((argument_info.flags & FLAG_HELP))
    {
        info_print_help();
        return 0;
    }
    else if((argument_info.flags & FLAG_VERSION))
    {
        info_version();
        return 0;
    }

    /* Were any flags (other than quiet and limit_everything) chosen? if not we can set FLAG_BUILD as default  */
    if((argument_info.flags & 0xFFAF) == 0) argument_info.flags |= FLAG_BUILD;

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
        // FIXME: ARGs in an array and loop?
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

    // FIXME: surely there is a better way
    while(ptr && i < end)
    {
        /* store what is specified in this string piece */
        store_overflow(str[i]);

        if(where = (unsigned long) strstr(str[i], LIMIT_OVERLOW_ARG_BUILD))
        {
            where = (where - (unsigned long) str[i]) + strlen(LIMIT_OVERLOW_ARG_BUILD)+1;
            argument_info.limit[LIMIT_ARRAY_BUILD] = strtoul(&str[i][where], NULL, 10);
        }
        if(where = (unsigned long) strstr(str[i], LIMIT_OVERLOW_ARG_MINOR))
        {
            where = (where - (unsigned long) str[i]) + strlen(LIMIT_OVERLOW_ARG_MINOR)+1;
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
    if(strstr(str, LIMIT_OVERLOW_ARG_BUILD))
        argument_info.types_to_overlow |= FLAG_BUILD;
    if(strstr(str, LIMIT_OVERLOW_ARG_MINOR))
        argument_info.types_to_overlow |= FLAG_MINOR;
}

void parse_file(FILE *file)
{
    // FIXME: these char buffers are ridiculous use strlen and malloc or something
    char file_str[2048], define[512], type[512];
    int hasOverflowed = 0;
    long nver;
    size_t line_len;

    while(fgets(file_str, 512, file))
    {
        int nvars = sscanf(file_str, "%s %s %ld", define, type, &nver);

        /* if we didn't fill enough variables, we may as well cut it short RIGHT NOW! :) */
        if(nvars < 3)
            continue;

        long of_type = check_type(type);

        if(of_type && (argument_info.flags & of_type) == of_type)
        {
            nver = increase(of_type, nver, file, &hasOverflowed);
            
            line_len = strlen(file_str);
            
            /* change and write the file, to update it to the new information */
            sprintf(file_str, "%.512s %.512s %ld", define, type, nver);
            write_to_file(file, line_len, file_str);

            /* print what we did to the output of the console, unless the user
            specified us to shut up */
            if(!(argument_info.flags & FLAG_QUIET)) 
                printf("%s increased to %ld\n", type, nver);

            argument_info.flags &= ~(of_type); 
        }
    }
    
    /* check if we've already updated the parent, if necessarry, and if not 
    make sure we read the file. (because the parent is probably above it's child in the file) */
    hasoverflowed(&hasOverflowed, file); 
}

long increase(long type, long nver, FILE *file, int *overflowed)
{
    long limit = getlimit(type);

    /* check if type == type_to_overlow and that the current version needs to be limited */
    if(limit && (argument_info.types_to_overlow & type) && (nver >= limit))
    {
        ShrinkFile(file, digit_count(nver));
        nver = 0;

        // set overflow
        *(overflowed) = (argument_info.flags & (type << 1)) ? 0 : 1;

        /* indicate that we overflowed and make sure we also now increase the next type */
        argument_info.flags |= type << 1;

        // remove current type from the todo-list, since we may need to read the file again
        // and you don't want the current type to be updated a second time
        argument_info.flags &= ~(type); 
    }
    else
        nver = do_the_trick(nver, file);
    
    return nver;
}

// returns the type that is in the string (e.g. BUILD, MINOR or MAJOR)
long check_type(char *type)
{
    char *types[N_TYPES] = {BUILD, MINOR, MAJOR};

    for(int i = 0; i < N_TYPES; ++i)
        if(!strcmp(&type[argument_info.prefix_len], types[i]))
            return TYPE_FLAG << i;
    
    return 0;
}

void hasoverflowed(int *hasOverflowed, FILE *file)
{

    /* if hasOverflow was never reset to 0, then we haven't found the type we needed to overflow yet
        (parent type to increase). 

        by reading the file again with all flags off (except the ones we
        need to find the parent type) we can find and change them anyway.
        
        this probably isn't the most beautiful solution, but it works */

    if(!*(hasOverflowed))
        return;
    *(hasOverflowed) = 0;

    fseek(file, 0, SEEK_SET);
    parse_file(file);  
}

long getlimit(char current)
{
    if(current & FLAG_BUILD || (argument_info.flags & FLAG_LIMIT_EVERYTHING == FLAG_LIMIT_EVERYTHING))
        return (long) argument_info.limit[LIMIT_ARRAY_BUILD];
    if(current & FLAG_MINOR)
        return (long) argument_info.limit[LIMIT_ARRAY_MINOR];

    return 0;
}

long do_the_trick(long ver_num, FILE *file)
{
    if (digit_count(ver_num) < digit_count(ver_num + 1))
        GrowFile(file);

    return ++ver_num;
}

void write_to_file(FILE *file, long line_size, char file_line[512])
{
    fseek(file, -(line_size), SEEK_CUR);
    fputs(file_line, file);
}

int digit_count(long value)
{
    int count = (!value) ? 1 : 0;

    while(value != 0)
    {
        count++;
        value = value / 10;
    }

    return count;
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
