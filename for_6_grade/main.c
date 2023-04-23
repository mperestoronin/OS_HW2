#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

#define DEFAULT_NUM_THREADS 4
#define BUFFER_SIZE 256

typedef struct
{
    const char *text;
    char encrypted_text[BUFFER_SIZE];
    int start_index;
    int end_index;
    pthread_t thread_id;
    sem_t *semaphore;
} ThreadData;

typedef struct
{
    int num_threads;
    ThreadData *thread_data;
    sem_t *semaphore;
} ProcessManagerData;

void *encrypt_text(void *data)
{
    ThreadData *thread_data = (ThreadData *)data;
    int pos = 0;
    for (int i = thread_data->start_index; i < thread_data->end_index; i++)
    {
        pos += sprintf(thread_data->encrypted_text + pos, "%d ", (int)thread_data->text[i]);
    }
    sem_post(thread_data->semaphore);
    return NULL;
}

void *process_manager(void *data)
{
    ProcessManagerData *process_manager_data = (ProcessManagerData *)data;
    int num_threads = process_manager_data->num_threads;
    ThreadData *thread_data = process_manager_data->thread_data;

    char *encrypted_text = (char *)mmap(NULL, BUFFER_SIZE * num_threads, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (encrypted_text == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    for (int i = 0; i < num_threads; i++)
    {
        sem_wait(thread_data[i].semaphore);
        printf("Thread %lu has completed\n", (unsigned long)thread_data[i].thread_id);
        strcat(encrypted_text, thread_data[i].encrypted_text);
    }

    printf("Encrypted text: %s\n", encrypted_text);

    // Free the allocated memory
    if (munmap(encrypted_text, BUFFER_SIZE * num_threads) == -1) {
        perror("munmap");
        exit(1);
    }

    sem_post(process_manager_data->semaphore);

    return NULL;
}
int main(int argc, char *argv[])
{
    char *text = "Hello, World!";
    if (argc > 1)
    {
        text = argv[1];
    }

    printf("Original Text: %s\nStarting...\n", text);

    int segment_size = 4;

    int num_threads = (strlen(text) + segment_size - 1) / segment_size; // Divide by 4 and round up

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    int text_length = strlen(text);
    int chunk_size = segment_size;

    sem_t *semaphore = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (semaphore == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    if (sem_init(semaphore, 1, 0) == -1) {
        perror("sem_init");
        exit(1);
    }

    for (int i = 0; i < num_threads; i++)
{
thread_data[i].text = text;
thread_data[i].start_index = i * segment_size;
thread_data[i].end_index = (i + 1) * segment_size;
if (thread_data[i].end_index > text_length)
{
thread_data[i].end_index = text_length;
}
thread_data[i].semaphore = semaphore;
pthread_create(&thread_data[i].thread_id, NULL, encrypt_text, (void *)&thread_data[i]);
}
ProcessManagerData process_manager_data;
process_manager_data.num_threads = num_threads;
process_manager_data.thread_data = thread_data;

sem_t *process_manager_semaphore = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
if (process_manager_semaphore == MAP_FAILED) {
    perror("mmap");
    exit(1);
}
if (sem_init(process_manager_semaphore, 1, 0) == -1) {
    perror("sem_init");
    exit(1);
}

process_manager_data.semaphore = process_manager_semaphore;

pthread_t process_manager_thread;
pthread_create(&process_manager_thread, NULL, process_manager, (void *)&process_manager_data);

sem_wait(process_manager_semaphore);
printf("All threads completed\n");

// Free the allocated memory
if (sem_destroy(semaphore) == -1) {
    perror("sem_destroy");
    exit(1);
}
if (munmap(semaphore, sizeof(sem_t)) == -1) {
    perror("munmap");
    exit(1);
}
if (sem_destroy(process_manager_semaphore) == -1) {
    perror("sem_destroy");
    exit(1);
}
if (munmap(process_manager_semaphore, sizeof(sem_t)) == -1) {
    perror("munmap");
    exit(1);
}

return 0;
}
