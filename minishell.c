#include <stdio.h>    
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>  
#include <string.h>
#include <signal.h> 
#include <unistd.h>
#include "readcmd.h"
#include "process.h"
#include <errno.h>
#include <limits.h>


process liste_proc;         /* liste des processus */
pid_t pid_fils;             
int etat_fils;
char * path ;               /* current dir */

/*
traiter le signal SIGCHILD
*/
void suivi_fils (int sig) {
    int etat_fils, pid_fils;
    do {
        pid_fils = (int) waitpid(-1, &etat_fils, WNOHANG | WUNTRACED | WCONTINUED);
        if ((pid_fils == -1) && (errno != ECHILD)) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        } else if (pid_fils > 0) {
             if (WIFSTOPPED(etat_fils)) {
                /* traiter la suspension */
                mv_proc(pid_fils,'S',liste_proc);
            } else if (WIFCONTINUED(etat_fils)) {
                /* traiter la reprise */
                mv_proc(pid_fils,'A',liste_proc);
            } else if (WIFEXITED(etat_fils)) {
                /* traiter exit */
                rm_proc(pid_fils,&liste_proc);
            } else if (WIFSIGNALED(etat_fils)) {
                /* traiter signal */
                if (WTERMSIG(etat_fils) == 9){
                    rm_proc(pid_fils,&liste_proc);
                }
            }
        }
    } while (pid_fils > 0);
}

/*
traiter le signal sigint fils
*/
void sigint_handler (int sig) {
    kill(pid_fils,SIGINT);
    printf("\n killed by SIGINT \n");
}

/*
traiter le signal sigstop fils 
*/
void sigstop_handler (int sig) {
    kill(pid_fils,SIGSTOP);
    printf("\n stopped by SIGSTOP \n");
}




/*
savoir le nombre des commandes dans une pipe line
*/
int n_cmd(struct cmdline * line){
    int i = 0;
    while(line -> seq[i]){
        i++;
    }
    return i ;
}

/*
executer une commande externe
*/
void execute(int in , int out, char ***seq,char *backgrounded){
    pid_fils = fork();
    if (pid_fils == -1){                     /* erreur fork */
        printf("error: fork\n");
        exit(1);
    } else if (pid_fils == 0){               /* fils */
        if (in != STDIN_FILENO){
            dup2 (in, STDIN_FILENO);
            close(in);
        }
        if (out != STDOUT_FILENO){
            dup2 (out, STDOUT_FILENO);
            close(out);
        }
        if (execvp((seq[0][0]),(seq[0])) < 0) {
            printf("error: exec failed\n");
            exit(1);
        }
    } else {                                 /* pere */ 
        if (backgrounded == NULL){
            waitpid(pid_fils,&etat_fils,0);
        } else {
            char * name = malloc(strlen(seq[0][0])*sizeof(char));                 // on ajoute le processus a la liste active 
            name = strcpy(name,(seq[0][0]));
            add_proc(name,'A',pid_fils,&liste_proc,lenght(liste_proc));
        }
    }
}

/*
programme principale 
*/
int main(int argc, char **argv){
    struct cmdline * line ;
    liste_proc = NULL;

    // boucle principale
    while (1){
        
        // monter le repertoire courant
        path = malloc(sizeof(char)*PATH_MAX);
        if (getcwd(path,sizeof(path)*PATH_MAX) != NULL){
            printf("%s $ ",path);
        } else {
            perror("**ERROR**->getcwd");
        }
        
        // lire une commande
        line = readcmd();


        if (*(line->seq) != NULL){

            // les commandes internes
            if (strcmp((line -> seq[0][0]),"exit") == 0 ){
                exit(0);
            }
            else if (strcmp((line -> seq[0][0]),"cd") == 0 ){
                if (chdir(line -> seq[0][1]) != 0){
                    perror("error cd ");
                }
            }
            else if (strcmp((line -> seq[0][0]),"list") == 0 ){
                aff_proc(liste_proc);
            }
            else if (strcmp((line -> seq[0][0]),"stop") == 0 ){
                int y = atoi(line -> seq[0][1]);
                if (y <= lenght(liste_proc)){
                    process process_asusp = search(liste_proc,y);
                    kill(process_asusp->pid,SIGSTOP);
                }else{
                    printf("\n erreur argument \n");
                }
            }
            else if (strcmp((line -> seq[0][0]),"bg") == 0 ){
                int y = atoi(line -> seq[0][1]);
                if (y <= lenght(liste_proc)){
                process process_asusp = search(liste_proc,y);
                kill(process_asusp->pid,SIGCONT);
                } else {
                    printf("\n erreur argument \n");
                }
            }
            else if (strcmp((line -> seq[0][0]),"fg") == 0 ){
                int y = atoi(line -> seq[0][1]);
                if (y <= lenght(liste_proc)){
                process process_asusp = search(liste_proc,y);
                kill(process_asusp->pid,SIGCONT);
                waitpid(process_asusp->pid,&etat_fils,0);
                rm_proc(process_asusp->pid,&liste_proc);
                } else {
                    printf("\n erreur argument \n");  
                }
            }else {
                // traitement des signaux 
                signal(SIGCHLD,&suivi_fils);
                signal(SIGINT,&sigint_handler);
                signal(SIGSTOP,&sigstop_handler);
                int n = n_cmd(line);                      // nombre des commandes en pipeline  
                int p[2];                                 // pipe
                int in = STDIN_FILENO;
                int i;    
                int fd1 ;                                 
                int fd2 ;             
                for( i = 0; i < n - 1 ; ++i){
                    pipe(p);
                    execute(in,p[1],(line -> seq ) + i, NULL);
                    close(p[1]);
                    in = p[0];
                }

                if ((line -> in) != NULL){  
                    if ((fd1 = open((line -> in),O_RDONLY)) == -1){
                        perror("**ERROR**->open");
                    }
                    in = fd1 ;
                }

                int out = STDOUT_FILENO ;
                if ((line -> out) != NULL){  
                    if ((fd2 = open((line -> out),O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1){
                        perror("**ERROR**->open");
                    }
                    out = fd2;  
                }
                execute(in,out,(line -> seq)+i,line ->backgrounded);
            }
        } else {
            
        }
    }
}
