

/************** t.c file **************************/
int prints(char *s)
{
  // int c = 0;
  while(*s != '\0'){
    putc(*s);
    s++;
  }
  return 0;
  
  // call putc(c) to print string s;
}

int gets(char s[ ])
{
  int x = 0;
  while(x < 63){
   
   s[x]= getc();
   if(s[x] == '\r'){
     s[x] = '\0';
     prints("\n\r");
     return 0;
   }
    putc(s[x]);
    x++;
    
  }
  s[x] = '\0';
  prints("\n\r");
  return 0;
  // call getc() to input a string into s[ ]
}

main()
{
   char name[64];
   while(1){
     prints("What's your name? ");
     gets(name);
     if (name[0]==0)
        break;
     prints("Welcome "); prints(name); prints("\n\r");
   }
   prints("return to assembly and hang\n\r");
}
