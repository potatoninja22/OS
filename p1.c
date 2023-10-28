#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARRAY_SIZE 100
#define MAX_FILES 100


typedef struct {
    int command_type; // 1 for "write 1," 2 for "write 2," 0 for "read"
    char filename[50];
    char text[MAX_COMMAND_LENGTH];
} Command;

typedef struct _rwlock_t {
    sem_t readlock;
    sem_t writelock;
    sem_t writer_present;
    int readers;
    int writers; 
}rwlock_t;

void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    rw->writers =0;
    sem_init(&rw->readlock, 0, 1);
    sem_init(&rw->writelock, 0, 1);
    sem_init(&rw->writer_present, 0, 1);
}

char* total_files[MAX_FILES];
rwlock_t total_locks[MAX_FILES];
int files=0;

void rwlock_acquire_readlock(rwlock_t *rw) {
    sem_wait(&rw->writer_present);
    sem_wait(&rw->readlock);
    rw->readers++;
    if (rw->readers == 1)
        sem_wait(&rw->writelock); 
    sem_post(&rw->readlock);
    sem_post(&rw->writer_present);
}

void rwlock_release_readlock(rwlock_t *rw) {
    sem_wait(&rw->readlock);
    rw->readers--;
    if (rw->readers == 0)
        sem_post(&rw->writelock); 
    sem_post(&rw->readlock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    sem_wait(&rw->writer_present);
    sem_wait(&rw->writelock);
    rw->writers++;
    sem_post(&rw->writer_present);
}

void rwlock_release_writelock(rwlock_t *rw) {
    rw->writers--;
    sem_post(&rw->writelock);
}


void *read_operation(void *arg) {
    Command *cmd = (Command *)arg;
    // rwlock_t rw;
    // rwlock_init(&rw);
    int i=0;
    for(i=0;i<files;i++){
        if(strcmp(total_files[i],cmd->filename) == 0){break;}
    }
    // printf("%s %d\n",cmd->filename,total_locks[i].readers);
    rwlock_acquire_readlock(&total_locks[i]);
    
    FILE *file = fopen(cmd->filename, "r");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fclose(file);
        printf("read %s of %ld bytes with %d readers and %d writers present\n", cmd->filename, size,total_locks[i].readers, total_locks[i].writers);
    } else {
        printf("Error: Could not open file %s for reading\n", cmd->filename);
    }
    rwlock_release_readlock(&total_locks[i]);
    pthread_exit(NULL);
}

void *write_operation(void *arg) {
    // rwlock_acquire_writelock(&rw);
    Command *cmd = (Command *)arg;
    
    // printf("destination file: %s",cmd->filename);

    if (cmd->command_type == 1) {
        // rwlock_t rwsrc; rwlock_t rwdes;
        // rwlock_init(&rwsrc);
        // rwlock_init(&rwdes);
        int src=-1,des=-1,k=-1;
        for(k=0;k<files;k++){
            if(strcmp(total_files[k],cmd->text) == 0){src=k;}
            if(strcmp(total_files[k],cmd->filename) == 0){des=k;}
        }
        rwlock_acquire_readlock(&total_locks[src]);
        rwlock_acquire_writelock(&total_locks[des]);
        FILE *destFile = fopen(cmd->filename, "a");
        FILE *srcFile = fopen(cmd->text, "r");

        if (srcFile == NULL || destFile == NULL) {
            printf("Error: Could not open files for writing\n");
        } else {
            long initialPosition = ftell(destFile); // Get the initial position before appending

            fprintf(destFile, "\n"); // Start a new line in the destination file before appending content

            char ch;
            while ((ch = fgetc(srcFile)) != EOF) {
                fputc(ch, destFile); // Append data to the file
            }

            long finalPosition = ftell(destFile); // Get the final position after appending
            long bytesAppended = finalPosition - initialPosition; // Calculate the size of appended content

            fclose(srcFile);
            fclose(destFile);

            printf("Writing to %s added %ld bytes with %d readers and %d writers present\n", cmd->filename, bytesAppended, total_locks[des].readers, total_locks[des].writers);
        }
        rwlock_release_readlock(&total_locks[src]);
        rwlock_release_writelock(&total_locks[des]);

    } else if (cmd->command_type == 2) {
        // rwlock_t rw;
        // rwlock_init(&rw);
        int i=0;
        for(i=0;i<files;i++){
            if(strcmp(total_files[i],cmd->filename) == 0){break;}
        }
        rwlock_acquire_writelock(&total_locks[i]);

        FILE *file = fopen(cmd->filename, "a");
        if (file == NULL) {
            printf("Error: Could not open file for writing\n");
        } else {
            long initialPosition = ftell(file); // Get the initial position before appending

            fprintf(file, "\n%s", cmd->text); // Append data to the file

            long finalPosition = ftell(file); // Get the final position after appending
            long bytesAppended = finalPosition - initialPosition; // Calculate the size of appended content

            fclose(file);

            printf("Writing to %s added %ld bytes with %d readers and %d writers present\n", cmd->filename, bytesAppended,total_locks[i].readers, total_locks[i].writers);
        }
        rwlock_release_writelock(&total_locks[i]);
    } else {
        printf("Error: Invalid write command type\n");
    }

    pthread_exit(NULL);
}



int main() {
    FILE *file;
    file = fopen("input.txt", "r");
    if (file == NULL) {
        printf("Failed to open the input file.\n");
        return 1;
    }

    char line[MAX_COMMAND_LENGTH];
    Command commands[MAX_ARRAY_SIZE];
    int num_commands = 0;

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, " ");
    
        if (strcmp(token, "read") == 0) {
            commands[num_commands].command_type = 0;
            token = strtok(NULL, "\r"); 
            if (token != NULL && strlen(token) > 0) {
                strcpy(commands[num_commands].filename, token);
                int flag=0;
                
                for(int i=0;i<files;i++){
                    if(strcmp(total_files[i],token) == 0){flag=1;break;}
                    
                }
                if(!flag){
                    rwlock_t rw ;
                    rwlock_init(&rw);
                    total_files[files] = (char*)malloc(strlen(token));
                    strcpy(total_files[files], token); 
                    total_locks[files]= rw; files++;
                    // printf("%s %d cnaicia \n",token, rw.readers);
                }
                
            } else {
                printf("Error: Invalid read command format\n");
            }
            num_commands++;
            continue;
        }
    
        if (strcmp(token, "write") == 0) {
            token = strtok(NULL, " "); 
            if (token != NULL) {
                int command_type = atoi(token); // Convert the token to an integer
        
                token = strtok(NULL, " "); // Get the next token after command_type
                
                if(token != NULL){
                    char filename[50];
                    strcpy(filename, token);
                    int flag=0;
                    for(int i=0;i<files;i++){
                        if(strcmp(total_files[i],token) == 0){flag=1;break;}
                    }
                    if(!flag){
                        rwlock_t rw; 
                        rwlock_init(&rw);
                        total_files[files] = (char*)malloc(strlen(token));
                        strcpy(total_files[files], token);
                        // total_files[files]=token; 
                        total_locks[files]= rw; files++;
                    }
                    token = strtok(NULL, "\r"); // Get the rest of the line as text
                    int flag2=0;
                    for(int i=0;i<files;i++){
                        if(strcmp(total_files[i],token) == 0){flag2=1;break;}
                    }
                    if(!flag2){
                        rwlock_t rw;
                        rwlock_init(&rw);
                        total_files[files] = (char*)malloc(strlen(token));
                        strcpy(total_files[files], token);
                        // total_files[files]=token; 
                        total_locks[files]= rw; files++;
                    }
                    if (token != NULL) {
                        char text[MAX_COMMAND_LENGTH];
                        strcpy(text, token);
                        commands[num_commands].command_type = command_type;
                        strcpy(commands[num_commands].filename, filename);
                        strcpy(commands[num_commands].text, text);
                        
                    } else {
                        printf("Error: Invalid write command format\n");
                    }
                } else {
                    printf("Error: Invalid write command format\n");
                }
            } else {
                printf("Error: Invalid write command format\n");
            }
            num_commands++;
            continue;
        }
 
        else if(strcmp(token, "exit") == 0){
            break;
        } 
        else{
            printf("Invalid command: %s", line);
            continue;
        }
        
        num_commands++;
    }
    
    fclose(file);

    pthread_t threads[MAX_ARRAY_SIZE];
    int num_threads = 0;

    for (int i = 0; i < num_commands; i++) {
        // printf("%d\n",i);
        if (commands[i].command_type == 0) {
            pthread_create(&threads[num_threads], NULL, read_operation, (void *)&commands[i]);
        } else if (commands[i].command_type == 1 || commands[i].command_type == 2) {
            pthread_create(&threads[num_threads], NULL, write_operation, (void *)&commands[i]);
        }
        num_threads++;
    }
    

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("\n%d\n",files);
    for(int i=0;i<files;i++){
        printf("%s\n",total_files[i]);
    }
    for (int i = 0; i < files; i++) {
        free(total_files[i]);
    }
    return 0;
}

