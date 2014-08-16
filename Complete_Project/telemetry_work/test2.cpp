/* system example : DIR */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <string.h>
int main ()
{

        FILE *in;
	    //extern FILE *popen();
	    char idbuff[512];
	    char* id[10];
	    int i;
	    for(i=0;i<10;i++){
           id[i]=(char*)malloc(512*sizeof(char)); 
           id[i][0]='\0';

	    }

     	if(!(in = popen("ps -ef | grep \"./test\" | awk '{print $2}'", "r"))){
	        exit(1);
	    }

	 
	    i=0;
	    while(fgets(idbuff, sizeof(idbuff), in)!=NULL){
	        //printf("%s", buff);
	        
	        strcpy(id[i],buff);
	        i++;

	    }	  
        i=0;
	    while(id[i][0]!='\0'){
             printf("%s",id[i]);
             i++;

	    }  
        
	    pclose(in);

}
