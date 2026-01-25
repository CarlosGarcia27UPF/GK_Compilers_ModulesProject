 





#ifndef PP_H  
#define	PP_H

 
 
 
 
 

#define ON              1
#define OFF             0

#define PRINTDESTFILE       "out.txt"     
#define PRINTERRORFILE      "errmsg.txt"    

#define N               2   
#define PRINTWARNING    ON       
#define PRINTERROR      ON      

 
 
 
 
 
 
 
#define ERROR1( num, message)({printf("\n%d ERROR \n", num);printf message;})
#define FERROR1(num, message)({fprintf(errfile,"\n%d ERROOORRRRRRRRRR\n", num);fprintf message;})
#define WARNING(num, message)({fprintf(errfile,"%d ERROR: ", num);fprintf message;\
                        printf("ERROOORRRRRRRRRR see output file\n");})
#define ERROR(num, message)({WARNING( num, message); exit(0);})

FILE    *errfile;         
FILE    *ofile;           

#endif	 