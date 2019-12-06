#include "type.h"
#include "ucode.c"



int main(int argc, char *argv[])
{
    int infile = 0, r, size, readSize, bytesRead;
    struct stat mystat, *sp = &mystat;
    
    char file[256], path[256], cwd[256], buf[1024];
    if(argc == 1)
    {
        while(strcmp(buf,"exit") != 0 && strcmp(buf,"quit") != 0)
        {
            gets(buf);
            printf("%s%c\n",buf, '\0');
        }
    }
    else
    {
        strcpy(file,argv[1]);
        if(file[0] != '/')
        {
            getcwd(cwd);
            strcat(file, "/");
            strcat(cwd,file);
            strcpy(file,cwd);
            printf("File path %s\n",file);

        }
        r = stat(file,sp);
        infile = open(file,O_RDONLY);
        if(infile == 0)
        {
            printf("File not found\n%c",'\0');
            return;
        }
        size = sp->st_size;
        bytesRead = 0;
        
        while(bytesRead < size)
        {
            if((size - bytesRead) >= 1024)
            {
                readSize = 1024;
            }
            else
            {
                readSize = size - bytesRead;
            }
            read(infile,buf,readSize);
            printf("%s%c",buf,'\0');
            bytesRead += readSize;

        
        }   
        printf("\n%c",'\0');

        printf("File Read bytes Read = %d\n%c",bytesRead,'\0');
    }

}