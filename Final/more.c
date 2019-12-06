#include "type.h"
#include "ucode.c"



int main(int argc, char *argv[])
{
    int infile = 0, r, size, readSize, bytesRead;
    struct stat mystat, *sp = &mystat;
    
    char file[256], path[256], cwd[256], buf[1025]
        ,*cp, *cp1, input[64];
    if(argc == 1)
    {
        printf("More Bade usage pleaes give a filename\n%c",'\0');
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
        
        read(infile,buf,1024);
        printf("%s%c",buf);
        bytesRead = 1024;
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
            buf[1024] = '\0';
            cp = buf; cp1 = buf;
            while(cp != 0)
            {
                cp = mystrtok(&cp1,'\n');
                if(cp == 0)
                {
                    break;
                }
                printf("%s%c",cp,'\0');
                gets(input);
                if(strcmp(input,"quit") == 0 || strcmp(input,"exit") == 0)
                {
                    printf("Done in More \n%c",'\0');
                    return 0;
                }
            }

            bytesRead += readSize;

        
        }   
        printf("\n%c",'\0');

        printf("File Read bytes Read = %d\n%c",bytesRead,'\0');
    }

}