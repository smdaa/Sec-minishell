#include <sys/types.h>
#ifndef _PROCESS

  #define _PROCESS
    /*
    structure de données pour une liste de processus (liste chainée)
    */
    struct process {
        char *name;
        char etat;
        int id;
        pid_t pid;
        struct process *next;
    };typedef struct process *process;

    /*
    savoir la taille d'un liste
    In  : process = liste chainée
    Out : int     = la taille de la liste 
    */
    int lenght(process process);

    /*
    savoir si un element est present
    In  : process = liste chainée
    In  : pid     = le pid du processus su'on cherche
    Out : int     = 1 si le processus existe ,0 sinon
    */
    int isin_proc(pid_t pid,process process);

    /*
    afficher la liste des processus
    In  : process = liste chainée
    */
    void aff_proc(process process);

    /*
    ajouter un processus a la liste
    In  : name = nom du processus
    In  : etat = etat du processus (A:actif,S:stopped)
    In  : pid  = pid du processus qu'on veut ajouter
    In  : oldprocess = la liste des processus a laquelle on veut ajouter le processus
    In  : x    = l'id du processus propre au minishell
    */
    void add_proc(char *name, char etat, pid_t pid, process *oldprocess, int x);

    /*
    supprimer un processus de la liste
    In  : pid = pid du processus qu'on veut supprimer
    In  : oldprocess = la liste des processus a laquelle on veut supprimer le processus
    */
    void rm_proc(pid_t pid,process *oldprocess);

    /*
    modifier l'etat d'un processus
    In  : pid = pid du processus qu'on veut modifier
    In  : etat = nouveau etat du processus (A/S)
    In  : oldprocess = la liste des processus
    */
    void mv_proc(pid_t pid, char etat, process oldprocess);

    /*
    rechercher un processus par son id
    In  : process = la liste des process dans laquelle on cherche
    In  : id      = l'id du processus qu'on cherche 
    */
    process search(process process,int id);

#endif