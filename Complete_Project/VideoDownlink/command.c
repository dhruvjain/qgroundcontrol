#include<stdio.h>
#include<stdlib.h>

int main(){

system("nohup cvlc -Ihttp --file-logging --log-verbose=1 --logfile=\"vlc.log\" --vlm-conf vlm.conf & echo $! > vlc.pid");



}
