//
//  main.c
//  rle
//
//  Created by Etienne Toussaint on 05/11/2014.
//  Copyright (c) 2014 Etienne Toussaint. All rights reserved.
//
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define BUFFSIZE 1024

const char * fileOutPath;

void RLEcompress(int in, int out){
    //int br = 0;
    long seek = 0;
    signed char retain = -1;
    int cursorBuf = 0;
    int cursorBufOut = 0;
    int cursorWrit = 0;
    signed char buf[BUFFSIZE];
    signed char writ[BUFFSIZE];
    signed char bufOut[129];
    long rc;
    int i;

    while((rc = read(in,buf,BUFFSIZE))!=0){
        cursorBuf = -1;
        //int br = 0;
        while(42){
            if(cursorBuf>=rc-1){break;}
            else cursorBuf ++;
            if(retain == 1 && cursorBufOut == 128){
                writ[cursorWrit++]= 127;
                if(cursorWrit == BUFFSIZE){
                    write(out, writ, BUFFSIZE);cursorWrit = 0;
                }
                writ[cursorWrit++]=bufOut[0];
                if(cursorWrit == BUFFSIZE){
                    write(out, writ, BUFFSIZE);cursorWrit = 0;
                }
                cursorBufOut = 0;
                retain = -1;
            }
            if(retain == -1 && cursorBufOut == 128){
                //gestion relou.
                if((bufOut[cursorBufOut-1] == bufOut[cursorBufOut-2] && bufOut[cursorBufOut-1] == buf[cursorBuf])){}
                else if(cursorBuf+1<rc && bufOut[cursorBufOut-1] == buf[cursorBuf] && bufOut[cursorBufOut-1] == buf[cursorBuf+1]){}
                else if(cursorBuf+1>=1024 && bufOut[cursorBufOut-1] == buf[cursorBuf] && seek!=lseek(in,0,SEEK_CUR)){
                  seek = lseek(in,0,SEEK_CUR);
                  lseek(in,-1,SEEK_CUR);
                  break;
                  }
                else{
                  writ[cursorWrit++]= -128;
                  if(cursorWrit == BUFFSIZE){
                      write(out, writ, BUFFSIZE);cursorWrit = 0;
                  }
                  for(i=0;i<128;i++){
                      writ[cursorWrit++]=bufOut[i];
                      if(cursorWrit == BUFFSIZE){
                          write(out, writ, BUFFSIZE);cursorWrit = 0;
                      }
                  }
                  cursorBufOut = 0;
                  retain = -1;
                }
            }
            if (retain == -1 && cursorBufOut>1 && bufOut[cursorBufOut-1] == bufOut[cursorBufOut-2] && buf[cursorBuf] == bufOut[cursorBufOut-1]) {
                if((cursorBufOut - 1)>0){
                    writ[cursorWrit++] = -(cursorBufOut - 2);
                    if(cursorWrit == BUFFSIZE){
                        write(out, writ, BUFFSIZE);cursorWrit = 0;
                    }
                    for (i=0; i<cursorBufOut-2; i++) {
                        writ[cursorWrit++] = bufOut[i];
                        if(cursorWrit == BUFFSIZE){
                            write(out, writ, BUFFSIZE);cursorWrit = 0;
                        }
                    }
                }
                bufOut[0] = buf[cursorBuf];
                bufOut[1] = buf[cursorBuf];
                bufOut[2] = buf[cursorBuf];
                cursorBufOut = 3;
                retain = 1;
                continue;
            }
            if (retain == -1 && !(cursorBufOut>1 && bufOut[cursorBufOut-1] == bufOut[cursorBufOut - 2] && buf[cursorBuf] == bufOut[cursorBufOut-1])){
                bufOut[cursorBufOut++] = buf[cursorBuf];
                continue;
            }
            if (retain == 1 && buf[cursorBuf] == bufOut[cursorBufOut - 1]){
                bufOut[cursorBufOut++] = buf[cursorBuf];
                continue;
            }
            if (retain == 1 && buf[cursorBuf] != bufOut[cursorBufOut - 1]){
                writ[cursorWrit++] = cursorBufOut - 1;
                if(cursorWrit == BUFFSIZE){
                    write(out, writ, BUFFSIZE);cursorWrit = 0;
                }
                writ[cursorWrit ++] = bufOut[0];
                if(cursorWrit == BUFFSIZE){
                    write(out, writ, BUFFSIZE);cursorWrit = 0;
                }
                bufOut[0] = buf[cursorBuf];
                cursorBufOut = 1;
                retain = -1;
                continue;
            }
        }
    }
    if(retain == 1){
        writ[cursorWrit++] = cursorBufOut - 1;
        if(cursorWrit == BUFFSIZE){
            write(out, writ, BUFFSIZE);cursorWrit = 0;
        }
        writ[cursorWrit++]=bufOut[0];
        if(cursorWrit == BUFFSIZE){
            write(out, writ, BUFFSIZE);cursorWrit = 0;
        }
        cursorBufOut = 0;
        retain = -1;
    }
    if(retain == -1){
        writ[cursorWrit++] = -cursorBufOut;
        if(cursorWrit == BUFFSIZE){
            write(out, writ, BUFFSIZE);cursorWrit = 0;
        }
        for(i=0;i<cursorBufOut;i++){
            writ[cursorWrit++]=bufOut[i];
            if(cursorWrit == BUFFSIZE){
                write(out, writ, BUFFSIZE);cursorWrit = 0;
            }
        }
        cursorBufOut = 0;
        retain = -1;
    }
    write(out, writ, cursorWrit);
}


void RLEdecompress(int in, int out){
    signed char retain = 0;
    int cursorBuf = 0;
    int cursorWrit = 0;
    signed char buf[BUFFSIZE];
    signed char writ[BUFFSIZE];
    long rc;
    int i;

    //Parcours Tout le fichier.
    while((rc = read(in,buf,BUFFSIZE))!=0){
        cursorBuf = 0;
        while(42){
            if(retain == 0){
                if(cursorBuf>=rc)break;
                retain=buf[cursorBuf++];
            }
            if(retain>0){
                if(cursorBuf>=rc)break;
                for(i=0;i<retain+1;i++){
                    writ[cursorWrit++]=buf[cursorBuf];
                    if(cursorWrit == BUFFSIZE){
                        write(out, writ, BUFFSIZE);cursorWrit = 0;
                    }
                }
                cursorBuf ++;
                retain = 0;
            }
            if(retain<0){
                int temp = retain;
                for(i=0;i>temp;i--){
                    if(cursorBuf>=rc)break;
                    writ[cursorWrit++]=buf[cursorBuf++];
                    if(cursorWrit == BUFFSIZE){write(out, writ, BUFFSIZE);cursorWrit = 0;}
                    retain ++;
                }
            }
            if(cursorBuf>=rc)break;
        }
    }
    write(out, writ, cursorWrit);
    if(retain != 0){
        close(in);
        close(out);
        //printf("%s",fileinPath);
        unlink(fileOutPath);
        printf("Fichier non conforme");
        exit(1);
    }
    //free(cursor);
}


int main(int argc, char const *argv[]){
    int in=-1,out=-1;
    if(argc<4){
       printf("usage : rle fileIn fileOut");exit(1);}
    if((in = open(argv[2],O_RDONLY))==-1){
        printf("impossible d ouvrir le fichier %s ",argv[2]);exit(1);}
    if((out = open(argv[3],O_WRONLY | O_CREAT | O_TRUNC,0600)) < 0)
      {close(in);printf("impossible d ouvrir le fichier %s ",argv[3]);exit(1);}
    fileOutPath = argv[3];
    if(argv[1][1] == 'd')RLEdecompress(in,out);
    if(argv[1][1] == 'c')
    RLEcompress(in,out);

    close(in);
    close(out);
    return 0;
}
