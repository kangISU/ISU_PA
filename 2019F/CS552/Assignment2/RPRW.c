#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */

#include <unistd.h> /* sleep */
#include <sys/types.h>
#include <sys/wait.h> /*wait*/

typedef struct MySem {
  sem_t wmutex;
  sem_t fmutex;
  sem_t mutex;
  int nreader;
} Sem;

int do_read(Sem * s, int process_id, char * type) {
  sem_wait(&s->mutex);
  printf("Process %d (%s) arrives.\n", process_id, type);
  if (s->nreader == 0) {
    s->nreader += 1;
    // printf("Process %d needs fmutex.\n", process_id);
    sem_wait(&s->fmutex);
    // printf("Process %d pass fmutex.\n", process_id);
  } else {
    s->nreader += 1;
  }
  sem_post(&s->mutex);
  printf("Process %d starts reading.\n", process_id);
  sleep(2); // reading file
  printf("Process %d ends reading.\n", process_id);
  sem_wait(&s->mutex);
  s->nreader -= 1;
  if (s->nreader == 0) {
    sem_post(&s->fmutex);
  }
  sem_post(&s->mutex);
  printf("Process %d (%s) leaves.\n", process_id, type);
  return 0;
}

int do_write(Sem *s, int process_id, char * type) {
  printf("Process %d (%s) arrives.\n", process_id, type);
  sem_wait(&s->wmutex);
  // printf("Process %d pass wmutex.\n", process_id);
  sem_wait(&s->fmutex);
  printf("Process %d starts writing.\n", process_id);
  sleep(2);
  printf("Process %d ends writing.\n", process_id);
  sem_post(&s->fmutex);
  // printf("Process %d release fmutex.\n", process_id);
  sem_post(&s->wmutex);
  printf("Process %d (%s) leaves.\n", process_id, type);
  // printf("Process %d release wmutex.\n", process_id);
  return 0;
}

int main(int argc, char *argv[]) {
  // Sem s = {2,1,2};
  // Sem sem;
  // Sem *t = &s;
  // printf("s.mutex = %d\n", t->mutex);
  // s.mutex = 1;
  // printf("%d\n", (int)sizeof(s));
  int segment_id;
  Sem *p_sem;

  // printf("%d\n", (int)sizeof(Sem));
  segment_id = shmget(IPC_PRIVATE, sizeof(Sem), S_IRUSR | S_IWUSR);
  p_sem = (Sem *) shmat(segment_id, NULL, 0);
  sem_init(&p_sem->mutex, 1, 1);
  sem_init(&p_sem->fmutex, 1, 1);
  sem_init(&p_sem->wmutex, 1, 1);
  p_sem->nreader = 0;

  for (int i = 0; argv[1][i] != '\0'; i++) {
    // printf("%c\t %d\n", argv[1][i], i);
    int pid = fork();
    if (pid == 0) {
      if (argv[1][i] == 'r') {
        do_read(p_sem, i, "reader");
      }
      if (argv[1][i] == 'w') {
        do_write(p_sem, i, "writer");
      }
      // sleep(2);
      shmdt(p_sem); // release the link to shared memory
      return 0;
    }
    // if (argv[1][i] == 'w') {
    //   printf(java)
    // }
    sleep(1);
  }

  // int	segment_id;
  // int	*p;
  // int	pid;
  // segment_id = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR|S_IWUSR);
  // p = (int*)shmat(segment_id,	NULL,0);
  // *p = 0;
  // printf("%s", argv[1]);
  wait(NULL);
  sleep(8);
  // sem_destory(&p_sem->mutex);
  // sem_destory(&p_sem->fmutex);
  // sem_destory(&p_sem->wmutex);
  shmdt(p_sem);
  shmctl(segment_id, IPC_RMID, NULL); //remove the shared memory
  printf("All Done!\n");
  return 0;
}
