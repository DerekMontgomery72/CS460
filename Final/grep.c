#include "type.h"
#include "ucode.c"



int main(int argc, char *argv[])
{
    int infile = 0, r, size, readSize, bytesRead;
    struct stat mystat, *sp = &mystat;
    
    char file[256], path[256], cwd[256], buf[1025]
        ,*cp, *cp1, pattern[1024];
    if(argc == 1)
    {
        printf("Grep Bad usage pleaes give a filename and pattern\n%c",'\0');
        return;
    }
    else if(argc == 2)
    {
        printf("Grep Bade Usage please give both pattern and filename\n%c", '\0');
        return;
    }
    else
    {
        strcpy(pattern,argv[1]);
        strcpy(file, argv[2]);
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
            buf[1024] = '\0';
            cp = buf; cp1 = buf;
            while(cp != 0)
            {
                cp = mystrtok(&cp1,'\n');
                if(cp == 0)
                {
                    break;
                }
                if(mystrContains(cp,pattern) == 1)
                {
                    printf("%s\n%c",cp,'\0');
                }
                 
            }

            bytesRead += readSize;

        
        }   
        printf("\n%c",'\0');

        printf("File Read bytes Read = %d\n%c",bytesRead,'\0');
    }

}