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
    const char* text;
    char encrypted_text[BUFFER_SIZE];
    int start_index;
    int end_index;
    pthread_t thread_id;
    sem_t* semaphore;
} ThreadData;

typedef struct
{
    int num_threads;
    ThreadData* thread_data;
} ProcessManagerData;

void* encrypt_text(void* data)
{
    ThreadData* thread_data = (ThreadData*)data;
    int pos = 0;
    for (int i = thread_data->start_index; i < thread_data->end_index; i++)
    {
        pos += sprintf(thread_data->encrypted_text + pos, "%d ", (int)thread_data->text[i]);
    }
    sem_post(thread_data->semaphore);
    return NULL;
}

void* process_manager(void* data)
{
    ProcessManagerData* process_manager_data = (ProcessManagerData*)data;
    int num_threads = process_manager_data->num_threads;
    ThreadData* thread_data = process_manager_data->thread_data;

    int fd = shm_open("/encrypted_text", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(fd, BUFFER_SIZE * num_threads) == -1) {
        perror("ftruncate");
        exit(1);
    }

    char* encrypted_text = (char*)mmap(NULL, BUFFER_SIZE * num_threads, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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

    if (shm_unlink("/encrypted_text") == -1) {
        perror("shm_unlink");
        exit(1);
    }

    return NULL;
}

int main(int argc, char* argv[])
{
    char* text = "Hello, World!";
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

    for (int i = 0; i < num_threads; i++)
    {
        int start_index = i * chunk_size;
        int end_index = (i == num_threads - 1) ? text_length : start_index + chunk_size;
        thread_data[i].text = text;
        thread_data[i].start_index = start_index;
        thread_data[i].end_index = end_index;
        thread_data[i].semaphore = sem_open("/encrypt_semaphore", O_CREAT, 0666, 0);
        if (thread_data[i].semaphore == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        pthread_create(&thread_data[i].thread_id, NULL, encrypt_text, &thread_data[i]);
    }
    ProcessManagerData process_manager_data = { num_threads, thread_data };

    pthread_t process_manager_thread;
    pthread_create(&process_manager_thread, NULL, process_manager, &process_manager_data);

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(thread_data[i].thread_id, NULL);
    }

    pthread_join(process_manager_thread, NULL);

    // Close and unlink the semaphore
    sem_close(thread_data[0].semaphore);
    sem_unlink("/encrypt_semaphore");

    printf("Done!\n");
    return 0;
}
