#include "type.h"
#include "ucode.c"



int main(int argc, char *argv[])
{
    int infile = 0, r, size, readSize, bytesRead, outfile = 0;
    struct stat mystat, *sp = &mystat;
    
    char file[256], path[256], cwd[256], buf[1024], file2[256];
    if(argc < 2)
    {
        printf("Please give a second file to copy to\n%c",'\0');
    }
    else
    {
        strcpy(file,argv[1]);
        strcpy(file2, argv[2]);
        if(file[0] != '/')
        {
            getcwd(cwd);
            strcat(file, "/");
            strcat(cwd,file);
            strcpy(file,cwd);
            printf("File path %s\n",file);

        }
        if(file2[0] != '/')
        {
            getcwd(cwd);
            strcat(cwd,file2);
            strcpy(file2,cwd);
            printf("File2 path %s\n",file2);
        }
        r = stat(file,sp);
        infile = open(file,O_RDONLY);
        outfile = open(file2, O_WRONLY|O_CREAT);

        if(infile == 0 || outfile == 0)
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
            write(outfile, buf, readSize);

            bytesRead += readSize;

        
        }   
        printf("\n%c",'\0');

        printf("File Read bytes Read = %d\n%c",bytesRead,'\0');
    }

}