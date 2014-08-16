#include<iostream>
#include <fstream>
#include <limits>
#include<string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;


int main(){
 std::fstream file;

 file.open ("vlm2.conf", std::fstream::in | std::fstream::out);


for(int i=1;i<3;i++){
 char ch2[10];
 sprintf(ch2,"%d",i);
 string ch(ch2);
 string s1="new channel"+ch+ " broadcast enabled";
 string ip="192.168.0.112";
 string s2="setup channel"+ch+" input \"http://"+ip+":8080\"";
 string s3="setup channel"+ch+" output #http{mux=ts,dst=:8090/live"+ch+"}";
 
 string s4="setup channel"+ch+" option http-reconnect";
 string s5="setup channel"+ch+" option no-sout-rtp-sap option no-sout-standard-sap option ttl=1 option sout-keep";
 
 file<<s1<<endl<<s2<<endl<<s3<<endl<<s4<<endl<<s5<<endl<<endl;
}
                           

return 0;

}
