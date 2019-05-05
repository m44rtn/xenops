/* MIT License:

Copyright (c) 2019 Maarten Vermeulen

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

#define ARG_FILE_LOC            "--file"    /* location of the file containing the versioning */
#define ARG_FILE_TYPE           "--ftype"   /* specifies the file type, this changes where we will search for major/minor/build, default is txt */

#define ARG_HELP                "--help"    /* guess what this does */

#define ARG_PREFIX              "--prefix"  /* prefix to the major/minor/build numbers in the file */

#define ARG_CHNG_MAJOR          "--major"   /* change major version number */
#define ARG_CHNG_MINOR          "--minor"   /* change minor version number */
#define ARG_CHNG_BUILD          "--build"   /* change build version number (default) */



#define FLAG_BUILD              1
#define FLAG_MINOR              1 << 2
#define FLAG_MAJOR              1 << 3

#define FLAG_HELP               1 << 4  /* this makes sure we can check if help has been called (for the error checking 'n such) */

/* maybe these aren't really necessary... */
#define ERROR_NO_FILE           1
#define ERROR_FILE_NOT_EXIST    404

void help();
void arguments_parse(unsigned int nargs, char *arguments[]);
void parse_file(FILE *file);
int do_the_trick(int ver_num);


typedef struct
{
    char* file;
    char *prefix; 

    unsigned int flags;
    unsigned int offset;
    
    size_t prefix_len;
} tARG_INFO;

typedef struct 
{
    char str[512];
    int line_size;
} tDATA;


tARG_INFO argument_info;

void help()
{
    printf("usage: xenops --file [file-loc] (--prefix [prefix] --major --minor --build)\n\n\n");
    printf("%s\t\t\t\tlet's me know which file to use\n", ARG_FILE_LOC);
    printf("\n%s\t\t\t\tthis let's me know I should increment the major version number as well\n", ARG_CHNG_MAJOR);
    printf("%s\t\t\t\tthis let's me know I should increment the minor version number as well\n", ARG_CHNG_MAJOR);
    printf("%s\t\t\t\t(default) this let's me know I should increment the build version number as well\n", ARG_CHNG_MAJOR);

    printf("\n%s [prefix]\t\tthis let's me know if there's something in front of major/minor/build (for example VER_BUILD instead of BUILD)\n", ARG_PREFIX);
    
}

int main(unsigned int nargs, char* args[])
{
    /* set the argument_info struct up in such a way we can detect errors */
    argument_info.file          = NULL;
    argument_info.flags         = FLAG_BUILD;
    argument_info.offset        = 0;           /* this is where do_the_trick() starts searching for major/minor/build */
    argument_info.prefix_len    = 0;

    /* parse the arguments (it probably isn't called 'parsing', but I'll use the word anyway*/
    arguments_parse((nargs - 1), &args[1]);

    if((argument_info.flags & FLAG_HELP))
    {
        help();
        return 0;
    }

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
        printf("\n\033[0;31mError:\033[0m File does not exist\n\n"); 
        return ERROR_FILE_NOT_EXIST;
    }

    /* now we can do things with it */
    parse_file(file);
    
    fclose(file);
    return 0;
}

void arguments_parse(unsigned int nargs, char *arguments[])
{
    unsigned int i = 0;
    for(i; i < nargs; i++)
    {
        /* strcmp() returns 0 if the strings are identical */

       if(!strcmp(arguments[i], ARG_HELP)) 
            argument_info.flags |= FLAG_HELP;
    

        /* if the arguments contain a file location, store it */
        else if(!strcmp(arguments[i], ARG_FILE_LOC))
            argument_info.file = arguments[i + 1];
        
        /* store the prefix */
        else if(!strcmp(arguments[i], ARG_PREFIX))
        {
            argument_info.prefix = arguments[i + 1];
            argument_info.prefix_len = strlen(argument_info.prefix);
        }
            
        
        /* look what needs to be changed within the file */
        else if(!strcmp(arguments[i], ARG_CHNG_BUILD))
            argument_info.flags |= FLAG_BUILD;
        else if(!strcmp(arguments[i], ARG_CHNG_MINOR))
            argument_info.flags |= FLAG_MINOR;
        else if(!strcmp(arguments[i], ARG_CHNG_MAJOR))
            argument_info.flags |= FLAG_MAJOR; 
           
    }
}

void parse_file(FILE *file)
{
    tDATA data[3];
    int *file_ptr = (int *) file;

    fseek(file, 0, SEEK_END);

    long file_size = ftell(file);

    fseek(file, 0, SEEK_SET);
    /*fseek(file, argument_info.offset, SEEK_CUR);*/

    char file_str[512];
    char *original_str_ptr;
    
    const int define_length = argument_info.prefix_len + 5;

    char define[7], type[define_length];
    int nver;

    char *str1, *str2, *str3;

    /* There is probably a better way to do this */
    int i = 0;

    while(fgets(file_str, 512, file) != 0)
    {
        
        size_t line_len = strlen(file_str);

        int ignore = sscanf(file_str, "%s %s %d", define, type, &nver);

        if(strcmp(define, "#define"))
            continue;
        
        if(!strcmp(&type[argument_info.prefix_len], "BUILD") && (argument_info.flags & FLAG_BUILD))
        {
            nver = do_the_trick(nver);
            strcpy(data[i].str, file_str);
            ignore = sprintf(data[i].str, "%s %s %d\n", define, type, nver);
            data[i].line_size = line_len;
            fseek(file, -(data[i].line_size), SEEK_CUR);
            fputs(data[i].str, file);
            i++; 
        }
        if(!strcmp(&type[argument_info.prefix_len], "MINOR") && (argument_info.flags & FLAG_MINOR))
        {
            nver = do_the_trick(nver);
            strcpy(data[i].str, file_str);
            ignore = sprintf(data[i].str, "%s %s %d\n", define, type, nver);
            data[i].line_size = line_len;
            fseek(file, -(data[i].line_size), SEEK_CUR);
            fputs(data[i].str, file);
            i++;
        }
        if(!strcmp(&type[argument_info.prefix_len], "MAJOR") && (argument_info.flags & FLAG_MAJOR))
        {
            nver = do_the_trick(nver);
            strcpy(data[i].str, file_str);
            ignore = sprintf(data[i].str, "%s %s %d\n", define, type, nver);
            data[i].line_size = line_len;
            fseek(file, -(data[i].line_size), SEEK_CUR);
            fputs(data[i].str, file);
            
        }

        
    }

    for(i; i > 0; i--)
    {
        /* let it store the stuff? */
        
    }
    
    
    /*fseek(file, 0, SEEK_SET);
    fwrite(original_str_ptr, file_size, 1, file);*/
}

int do_the_trick(int ver_num)
{
    ver_num++;
    printf("hi! %d\n", ver_num);
    return ver_num;
}

