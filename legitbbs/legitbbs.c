#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bbs.h"

//char flag[] = "myverysecretkey123456\x00";
char *flag = 0x4010000;


#define MAXLINE 12
char *readline( )
{
  char *line = malloc(MAXLINE+1);
  int i = 0;
  
  if ( line == NULL ) {
    puts("[-] malloc failed.\n");
    return NULL;
  }
  
  memset( line, 0, MAXLINE);
  
  printf("> ");
  fflush(stdout);
  
  while( 1 ) {
    wait();
    
    while ( (i < MAXLINE) && (read(line+i, 1) > 0) ) {
      
      if ( line[i] == '\n') {
  line[i] = '\x00';
  return line;
      }
      
      i += 1;
    }
  }
  return line;
}


int fake_fgets(unsigned char *buf, int length, int HANDLE){
  char tmpBuf=0;
  int rec=0;
  int ret;
  while(rec < length){
    ret=read(&tmpBuf,1);
    if(ret==0){
      wait();
      continue;
    }
    rec+=ret;
    buf[rec-1]=tmpBuf;
    if(tmpBuf==0x0a|| tmpBuf== 0x4){
      buf[rec-1]=0;
      break;
    }
  }
  return rec;
}


#define DEBUG 0
int fetch(char *search){
 
  char *begin,*end;
  char searchbuf[50];
  if (DEBUG){printf("1");fflush(stdout);}
  begin = strstr(search,"FETCH ");
  if (DEBUG){printf("2");fflush(stdout);}
  if (0 == begin){return 0;}
  if (DEBUG){printf("begin: %X\n",begin);fflush(stdout);}
  begin = strstr(begin," ");
  if (DEBUG){printf("begin: %X\n",begin);fflush(stdout);}
  ++begin;
  if (DEBUG){printf("begin: %X\n",begin);fflush(stdout);}
  end = strstr(begin, "\x0a");
  if (NULL == end){return(0);}
  if (DEBUG){
  printf("end: %X\n",end);fflush(stdout);
  printf("end-begin: %d\n",(end-begin));
}
  memcpy(searchbuf,begin,(end-begin));
  searchbuf[end-begin] = 0;
  printf("FETCHING %s\n",searchbuf);


  if (NULL != strstr(flag,searchbuf)){
  return(1);
}
  
  if (NULL != strstr(BBSDATA,searchbuf)){
  return(1);
}

  if (NULL != strstr(menu,searchbuf)){
  return(1);
}
  
  if    (NULL != strstr(BARRICADBBS,searchbuf)){
  return(1);
}
  
  if    (NULL != strstr(BBSDATA,searchbuf)){
  return(1);
}
  
  if    (NULL != strstr(BULLETINBBS,searchbuf)){
  return(1);
}
  
if    (NULL != strstr(BYEBYEBBS,searchbuf)){
  return(1);
}
 
 if    (NULL != strstr(CBBS,searchbuf)){
  return(1);
}
 
 if    (NULL != strstr(CONTENTSBBS,searchbuf)){
return(1);
}
 
 if    (NULL != strstr(DAYLIMITBBS,searchbuf)){
  return(1);
}
 
 if    (NULL != strstr(ECHOBBS,searchbuf)){
  return(1);
}
 
 if    (NULL != strstr(EDITORBBS,searchbuf)){
  return(1);
}

if    (NULL != strstr(FILEBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(FILESBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(INQUIREBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(LOCATEBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(LOGOBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(LOREHELPBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(MAILBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(MAINBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(MSGBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(NEWUSERBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(OPEDBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(QUOTESBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(REPEDITBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(ROOKIEBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(SYSOPBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(TIMEWARNBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(TOOSLOWBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(WELCOMEBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(WHYFBBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(WHYHUBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(WHYIBMBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(WHYOPEDBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(WHYPVTBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(WHYSHADBBS,searchbuf)){
return(1);
}

if    (NULL != strstr(XFERBAUDBBS,searchbuf)){
return(1);
}


  return(0);
}


int main(int argc, char *argv[]){

  
  
  /* Declarations */
  char inputbuf[100];
  char fetchterm[50];
  int nextRet;
  /*
  char banana[BUF_LEN]; //named by Owen
  char attacker_prelude[PRELUDE_LEN];
  char secret[SECRET_LEN];
  char attacker_trailer[TRAILER_LEN];
  char *p;
  */
  /* Main */

  /*
  if (argc != 2){
    printf("Requires exactly one argument\n");
    exit(1);
  }

  */
  //flag page 0x4010000
//  dbg_break(inputbuf,0);
  printf(WHYSHADBBS);
  fflush(stdout);
  printf(MIRRORSH);
  fflush(stdout);
  
  fake_fgets(inputbuf,2,stdin);
  if (121 != inputbuf[0]){
    printf(BYEBYEBBS);
    dbg_break(inputbuf,-1);
    exit(0);
  }

  //  dbg_break(inputbuf[0],inputbuf[1]);
  
  //  printf("%s\n",flag+10);
  //printf("\n");
  printf(menu);
  fflush(stdout);
  while (1){

    fflush(stdout);
    fake_fgets(inputbuf,50,stdin);
//    dbg_break(inputbuf,0x90);;
    //dbg_break(inputbuf,0x80);
    //nextRet=strstr(inputbuf,"CONTENTS");
   
    inputbuf[40] = NULL;
    if (NULL != strstr(inputbuf,"GOODBYE")){
      dbg_break(0x90,0x90);
      printf(BYEBYEBBS);
      fflush(stdout);
      exit(0);
    } else if (NULL != strstr(inputbuf,"BARRICAD")){
      printf(BARRICADBBS);
    } else if
  (NULL != strstr(inputbuf,"DATA")){
      printf(BBSDATA);
    } else if (NULL != strstr(inputbuf,"FETCH")){
      if (fetch(inputbuf)){
  printf("FOUND IT\n");
      }else{
  printf("DIDNT FIND IT\n");
      }
    } else if
  (NULL != strstr(inputbuf,"BULLETIN")){
      printf(BULLETINBBS);
    } else if
  (NULL != strstr(inputbuf,"BYEBYE")){
      printf(BYEBYEBBS);
    } else if
  (NULL != strstr(inputbuf,"C")){
      printf(CBBS);
    } else if
  (NULL != strstr(inputbuf,"CONTENTS")){
     // dbg_break(1,1);
      printf(CONTENTSBBS);
    } else if
  (NULL != strstr(inputbuf,"DAYLIMIT")){
      printf(DAYLIMITBBS);
    } else if
  (NULL != strstr(inputbuf,"ECHO")){
      printf(ECHOBBS);
    } else if
    (NULL != strstr(inputbuf,"EDITOR")){
      printf(EDITORBBS);
    } else if
  (NULL != strstr(inputbuf,"FILE")){
      printf(FILEBBS);
    } else if
  (NULL != strstr(inputbuf,"FILES")){
      printf(FILESBBS);
    } else if
  (NULL != strstr(inputbuf,"INQUIRE")){
      printf(INQUIREBBS);
    } else if
    (NULL != strstr(inputbuf,"LOCATE")){
      printf(LOCATEBBS);
    } else if
    (NULL != strstr(inputbuf,"LOGO")){
      printf(LOGOBBS);
    }
    else if (NULL != strstr(inputbuf,"LOREHELP")){
      printf(LOREHELPBBS);
    } else if
  (NULL != strstr(inputbuf,"MAIL")){
      printf(MAILBBS);
    } else if
  (NULL != strstr(inputbuf,"MAIN")){
      printf(MAINBBS);
    } else if
  (NULL != strstr(inputbuf,"MSG")){
      printf(MSGBBS);
    } else if
  (NULL != strstr(inputbuf,"NEWUSER")){
      printf(NEWUSERBBS);
    } else if
  (NULL != strstr(inputbuf,"OPED")){
      printf(OPEDBBS);
    } else if
  (NULL != strstr(inputbuf,"QUOTES")){
      printf(QUOTESBBS);
    } else if
  (NULL != strstr(inputbuf,"REPEDIT")){
      printf(REPEDITBBS);
    } else if
  (NULL != strstr(inputbuf,"ROOKIE")){
      printf(ROOKIEBBS);
    } else if
  (NULL != strstr(inputbuf,"SYSOP")){
      printf(SYSOPBBS);
    } else if
  (NULL != strstr(inputbuf,"TIMEWARN")){
      printf(TIMEWARNBBS);
    } else if
  (NULL != strstr(inputbuf,"TOOSLOW")){
      printf(TOOSLOWBBS);
    } else if
  (NULL != strstr(inputbuf,"WELCOME")){
      printf(WELCOMEBBS);
    } else if
  (NULL != strstr(inputbuf,"WHYFB")){
      printf(WHYFBBBS);
    } else if
  (NULL != strstr(inputbuf,"WHYHU")){
      printf(WHYHUBBS);
    } else if
  (NULL != strstr(inputbuf,"WHYIBM")){
      printf(WHYIBMBBS);
    } else if
  (NULL != strstr(inputbuf,"WHYOPED")){
      printf(WHYOPEDBBS);
    } else if
  (NULL != strstr(inputbuf,"WHYPVT")){
      printf(WHYPVTBBS);
    } else if
  (NULL != strstr(inputbuf,"WHYSHAD")){
      printf(WHYSHADBBS);
    } else if
  (NULL != strstr(inputbuf,"XFERBAUD")){
      printf(XFERBAUDBBS);
    } else {
//      dbg_break(0x20,-1);

      printf(menu);
      printf("\n");
    }
    
  /*
  (NULL != strstr(inputbuf,"FETCH")){
      printf(FETCH);
    } else if
  
  }
  */

      
  }
  
  exit(0);
    
 return(0);
 
}
