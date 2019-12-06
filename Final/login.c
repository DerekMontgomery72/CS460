///////*Algorithm for Login ////////////
// login.c : Upon entry, argv[0]=login, argv[1]=/dev/ttyX

/*
(1). close file descriptors 0,1 inherited from INIT.
(2). open argv[1] 3 times as in(0), out(1), err(2).
(3). settty(argv[1]); // set tty name string in PROC.tty
(4). open /etc/passwd file for READ;
*/
#include "type.h"
#include "ucode.c"

int in, out, err;
char uname[128], password[128];

int main(int argc, char *argv[])
{
  
  //getc();
  int passwrd, lineLen;
  char *ptr, buf[1024], *cp, line[1024],
        fPasswrd[64], fUname[64], fprgm[64], *cp1;
  close(0);
  close(1);
  close(2);
  out = open(argv[1], O_RDONLY);
  in = open(argv[1], O_WRONLY);
  err = open(argv[1], O_RDONLY);
  
  passwrd = open("/etc/passwd", O_RDONLY);
  printf("Files %s opened\n", argv[1]);
  read(passwrd, buf, 1024);
  printf("buf: %s", buf);
  
  while(1){

    strcpy(line,buf);
    cp1 = line;
    cp = mystrtok(&cp1, '\n');
    printf("login:");
    gets(uname);
    printf("password:");
    gets(password);

    printf("USERNAME: %s  Password: %s\n",uname, password);

    while(cp)
    {
      cp = mystrtok(&cp1, '\n');
      if(cp == 0)
      {
        break;
      }
      TryLogin(cp,uname,password);
    }

    printf("Login failed, try again\n %c", '\0');

  }
  
 return 0;
}
// PASSWORD FILE LINE
// username:password:gid:uid:fullname:HOMEDIR:program
int TryLogin(char *line, char *uname, char *pword)
{
 char name[64],password[64],program[64], fullname[128], homedir[256];
 char *cp, *cp1;
 int gid, uid;

 cp = line; cp1=line;
 cp = mystrtok(&cp1,':');
 strcpy(name,cp);
 if(strcmp(name,uname) != 0)
 {
   return 1;
 }

 cp = mystrtok(&cp1,':');
 strcpy(password,cp);
 if(strcmp(password,pword) != 0)
 {
   int i = 0;
   while(i < 4)
   {
    printf("Invalid Password: Please try again\n %c", '\0');
    printf("Password: ");
    gets(pword);
    if(strcmp(password,pword) == 0)
    {
      break;
    }
    i++;
   }
   if(i == 4)
   {
    printf("Too many attempts\n");
    return 1;
   }
 }

 //User successfully logged in
 cp = mystrtok(&cp1,':');
 gid = atoi(cp);
 
 cp = mystrtok(&cp1,':');
 uid = atoi(cp);
 chuid(uid,gid);

 cp = mystrtok(&cp1,':');
 strcpy(fullname,cp);

 cp = mystrtok(&cp1,':');
 chdir(cp);

 cp = mystrtok(&cp1,':');
 exec(cp);
 return 0;

}

     


    
