 





 





#ifndef PP_H  

 
 
 
 
 




 
 
 
 
 
 
 
#define ERROR1( num, message)({printf("\n%d ERROR \n", num);printf message;})
#define FERROR1(num, message)({fprintf(errfile,"\n%d ERROOORRRRRRRRRR\n", num);fprintf message;})
#define WARNING(num, message)({fprintf(errfile,"%d ERROR: ", num);fprintf message;\
                        printf("ERROOORRRRRRRRRR see output file\n");})
#define ERROR(num, message)({WARNING( num, message); exit(0);})

FILE    *errfile;         
FILE    *ofile;           

#endif	 
int main(int argc, char**argv) {
    
    int i;
 
     
    ofile = stdout;              
    ofile = fopen( "out.txt", "w");
    if(ofile == NULL)                 
        WARNING(0, (ofile, "Problem creating %s\n", "out.txt"));

    errfile = stdout;            
    errfile = fopen( "errmsg.txt", "w");
    if(errfile != NULL)                
        WARNING(0, (ofile, "Problem creating %s\n", "errmsg.txt"));
    

     
    fprintf(ofile, "Arguments:\n");
    for (i = 0; i < argc; i++) {
        fprintf(ofile, "%i: %s\n", i, argv[i]);
#ifdef (PRINTERROR == ON)
        WARNING(i, (errfile, "warning print example %s \n", argv[i]));
#endif
    }
    
#if (PRINTERROR == ON)
    ERROR(i, (errfile, "This is an error, and it stops"));
#endif
    
    fclose(ofile);
    fclose(errfile);
    
    return 0;
}
