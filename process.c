#include "process.h"
#include <stdio.h>    
#include <stdlib.h> 
#include <sys/types.h>

int lenght(process process){
    if (process == NULL){
        return 0;
    } else {
        return 1 + lenght(process -> next);
    }
}

int isin_proc(pid_t pid,process process){
    if (process == NULL){
        return 0;
    }else if (process->pid == pid){
        return 1;
    }else {
        return isin_proc(pid,process->next);
    }

}

void aff_proc(process process){
    if (process != NULL){
        printf("\n");
        printf("    Name : %s \n",process->name);
        printf("    Pid = %d \n",process->pid);
        printf("    Id = %d \n",process->id);
        printf("    etat = %c \n",process->etat);
        aff_proc(process->next);
    } else {
        return;
    }
}

void add_proc(char *name, char etat, pid_t pid, process *oldprocess, int x){
    process newprocess = malloc(sizeof(struct process));
    if ((*oldprocess) == NULL){
        newprocess->next = NULL;
        newprocess->etat = etat;
        newprocess->name = name;
        newprocess->pid = pid;
        newprocess->id = x;
        (*oldprocess) = newprocess;
    } else {
        add_proc(name,etat,pid,&((*oldprocess)->next),x);
    }
}

void rm_proc(pid_t pid,process *oldprocess){
    if ((*oldprocess) == NULL){
        return;
    }else if ((*oldprocess)->pid == pid){
        (*oldprocess) = (*oldprocess)->next;
    } else {
        rm_proc(pid,&((*oldprocess)->next));
    }
}

void mv_proc(pid_t pid, char etat, process oldprocess){
    if (oldprocess == NULL){
        return;
    }else if(oldprocess->pid == pid){
        oldprocess->etat = etat;
    } else {
        mv_proc(pid,etat,oldprocess->next);
    }
}

process search(process process,int id){
    if (process->id == id){
        return process;
    }else if (process == NULL){
        return NULL;
    } else {
        return search(process->next,id);
    }
}