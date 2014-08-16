#include<stdio.h>
#include<string.h>

#include<stdlib.h>
void * process(char** c){
 //char* c[10];
 //c=(char*)ptr;
 int i=0; 
while(c[i][0]!='\0')
       { 
         printf("%s \n",c[i]); 
         i++;
       }
       


}

int main(){
           char* command[10];
          int i;
          for(i=0;i<10;i++){
            
          command[i]=(char*)malloc(20* sizeof(char));         
          command[i][0]='\0';
          }
          command[0]="hello world \n";
          command[1]="Hello world";
          process((void * )command);

}
