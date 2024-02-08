#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    printf("HHHHHHHHHHHHHHHHHHHHHH\n");
    struct thread_data* thread_func_args = (struct thread_data *)thread_param;
    sleep((thread_func_args->wait_to_obtain_ms)/1000);
    pthread_mutex_lock(thread_func_args->mutex);
    sleep((thread_func_args->wait_to_release_ms)/1000);
    printf("LLLLLLLLLLLLLLLLLLLLLLLLLLL\n");
    pthread_mutex_unlock(thread_func_args->mutex);
    printf("OOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    thread_func_args->thread_complete_success = true;
    return thread_param;
        // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    // typecasting (void *)thread param to struct thread_data*

}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    struct thread_data* new_data = (struct thread_data*)malloc(sizeof(struct thread_data));
    new_data->thread = thread;
    new_data->mutex = mutex;
    new_data->wait_to_obtain_ms = wait_to_obtain_ms;
    new_data->wait_to_release_ms = wait_to_release_ms;
    new_data->thread_complete_success = false;
    int rc = pthread_create(thread, NULL,threadfunc,new_data);
    if(rc)
    {
        errno = rc;
        perror("pthread_create");
        new_data->thread_complete_success = false;
        return false;
    }
    return true;
}

