#include <stdio.h>
#include <stdlib.h>

int recupInt(char x){
        return (int)(x-48);
}

//strcomp()
int myStrcmp(const char * a, const char *b){
        int i = 0;
        //printf("%s",a);
        while(a[i]!=0 && b[i]!=0) {
                //printf("%c,%c",a[i],b[i]);
                if (a[i] > b[i]) return 1;
                else if (a[i] < b[i]) return -1;
                i++;
        }
        if (a[i] == 0 && b[i] == 0) return 0;
        else if (a[i] != 0) return 1;
        else return -1;
}

int action(int resultat,int number,int type){
        if (type == 1) {
                return resultat += number;
        }
        else if (type == 2) {
                return resultat *= number;
        }
        return 0;
}

int myAtoi(const char * arg){
        int resultat = 0;
        int j = 0;
        while(arg[j] != 0) {
                if (arg[j] <48 || arg[j]>57) {
                        return -1;
                }
                else {
                        resultat *= 10;
                        resultat += recupInt(arg[j]);
                }
                j++;
        }
        return resultat;
}

int main(int argc, const char * argv[])
{
        int type = 0;
        if (myStrcmp(argv[0],"./addition") == 0) {
                type = 1;
        }
        else if(myStrcmp(argv[0],"./multiplication") == 0) {
                type = 2;
        }
        else{
                printf("%s: operation non autorisee.\n",argv[0]);
                return 1;
        }
        //printf("%d",type);
        int resultat = (type==1) ? 0 : 1;
        int i = 1;
        while (argv[i] != NULL) {
                int ajout = myAtoi(argv[i]);
                if (ajout == -1) {
                        printf("usage : %s <liste d’entiers>\n",argv[0]+2);
                        return 1;
                }
                resultat = action(resultat,ajout,type);
                i++;
        }
        printf("%d\n",resultat);

        return 0;
}
