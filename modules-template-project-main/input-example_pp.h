 





#ifndef PP_H  

 
 
 
 
 




 
 
 
 
 
 
 
#define ERROR1( num, message)({printf("\n%d ERROR \n", num);printf message;})
#define FERROR1(num, message)({fprintf(errfile,"\n%d ERROOORRRRRRRRRR\n", num);fprintf message;})
#define WARNING(num, message)({fprintf(errfile,"%d ERROR: ", num);fprintf message;\
                        printf("ERROOORRRRRRRRRR see output file\n");})
#define ERROR(num, message)({WARNING( num, message); exit(0);})

FILE    *errfile;         
FILE    *ofile;           

