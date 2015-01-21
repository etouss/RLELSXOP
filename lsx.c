#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#define PATH_MAX 4096

int idUser = 0;
int idGroup = 0;

int perm(struct stat srcStat){
    int who = 3;
    if (srcStat.st_uid == idUser)who = 1;
    else if(srcStat.st_gid == idGroup)who = 2;
    switch (who) {
        case 1:
            if (srcStat.st_mode & S_IXUSR)return 1;
            else return 0;
            break;
        case 2:
            if (srcStat.st_mode & S_IXGRP)return 1;
            else return 0;
            break;
        default:
            if (srcStat.st_mode & S_IXOTH)return 1;
            else return 0;
            break;
    }
}

void parcours(char * ref){
    DIR *dir = NULL;
    dir = opendir(ref);
    if(dir == NULL){fprintf(stderr,"lsx: %s:acces impossible\n",ref);return;}
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if ((entry->d_name[0] == '.' && strlen(entry->d_name) == 1) ||
            (entry->d_name[0] == '.'&& entry->d_name[1] == '.'
              && strlen(entry->d_name) == 2))
              continue;

        struct stat *tmp = malloc(sizeof(struct stat));
        char *chemin = malloc(sizeof(char)*(strlen(ref)+strlen(entry->d_name)+2));
        sprintf(chemin, "%s/%s",ref,entry->d_name);
        lstat(chemin, tmp);
        if(S_ISREG(tmp->st_mode)&&perm(*tmp))printf("%s\n",chemin);
        if(S_ISDIR(tmp->st_mode))parcours(chemin);
        free(tmp);
        free(chemin);
    }
    if(dir != NULL)closedir(dir);
}

int main(int argc, const char * argv[]) {
    idUser = getuid();
    idGroup = getgid();
    struct stat *srcStat = malloc(sizeof(struct stat));
    char * ref = NULL;
    if (argc != 2) {
        fprintf(stderr,"usage: %s <reference>\n",argv[0]);
        return 1;
    }
    else{
        //Relative path ERROR
        if (lstat(argv[1],srcStat)==-1)
          {fprintf(stderr,"lsx: %s: reference non valide\n",argv[1]); return 1;}
        else if(!perm(*srcStat))
          {fprintf(stderr,"lsx: %s: acces impossible\n",argv[1]); return 0;}
        else if(!(S_ISDIR(srcStat->st_mode)))
          {fprintf(stdout,"%s\n",argv[1]); return 0;}
        else{
            chdir(argv[1]);
            ref = getcwd(ref,PATH_MAX);
            parcours(ref);
        }
    }
    free(srcStat);
    free(ref);
    return 0;
}
