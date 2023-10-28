#include <stdio.h>
#include <stdlib.h>
#include <pthcon_tra.h>
#include <semaphore.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARRAY_SIZE 100
#define MAX_FILES 100


typedef struct {
    int command_type; // 1 for "ins_del 1," 2 for "ins_del 2," 0 for "con_tra"
    char filename[50];
    char text[MAX_COMMAND_LENGTH];
} Command;

typedef struct _avllock_t {
    sem_t con_tralock;
    sem_t ins_dellock;
    sem_t ins_delr_present;
    int con_traers;
    int ins_delrs; 
}avllock_t;

void avllock_init(avllock_t *avl) {
    avl->con_traers = 0;
    avl->ins_delrs =0;
    sem_init(&avl->con_tralock, 0, 1);
    sem_init(&avl->ins_dellock, 0, 1);
    sem_init(&avl->ins_delr_present, 0, 1);
}

char* total_files[MAX_FILES];
avllock_t total_locks[MAX_FILES];
int files=0;

void avllock_acquire_con_tralock(avllock_t *avl) {
    sem_wait(&avl->ins_delr_present);
    sem_wait(&avl->con_tralock);
    avl->con_traers++;
    if (avl->con_traers == 1)
        sem_wait(&avl->ins_dellock); 
    sem_post(&avl->con_tralock);
    sem_post(&avl->ins_delr_present);
}

void avllock_release_con_tralock(avllock_t *avl) {
    sem_wait(&avl->con_tralock);
    avl->con_traers--;
    if (avl->con_traers == 0)
        sem_post(&avl->ins_dellock); 
    sem_post(&avl->con_tralock);
}

void avllock_acquire_ins_dellock(avllock_t *avl) {
    sem_wait(&avl->ins_delr_present);
    sem_wait(&avl->ins_dellock);
    avl->ins_delrs++;
    sem_post(&avl->ins_delr_present);
}

void avllock_release_ins_dellock(avllock_t *avl) {
    avl->ins_delrs--;
    sem_post(&avl->ins_dellock);
}
