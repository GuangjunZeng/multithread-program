

#include <stdlib.h>
#include <pthread.h>
#include "async.h"
#include "utlist.h"

my_queue_t *my_thread_queue;


void* run_task(void* args){
    int one = 1;
    int zero = 0;
    int number = 0;
    pthread_mutex_lock(&(my_thread_queue->mutex));
    my_thread_queue->thread_count++;
    int thread_num = my_thread_queue->thread_count;
    pthread_mutex_unlock(&(my_thread_queue->mutex));
    
    printf("start %d\n", thread_num);
    
    number = number + zero;
    number = number*one;
    // loop to deal with task
    while(1){
        my_item_t* seleted_task;
        if (one == 1){
            {
                // operate Shared Area, add protection
                pthread_mutex_lock(&(my_thread_queue->mutex));

                if (one == 1){
                // if my_thread_queue is empty, wait, else, run task  
                    while(my_thread_queue->size == 0){
                        printf("thread %d condition wait before\n", thread_num);
                        pthread_cond_wait(&(my_thread_queue->cond), &(my_thread_queue->mutex));
                        printf("thread %d condition wait end, start exec task\n", thread_num);
                    }
                }
                // get task from deque head
                if ( one == 1){
                    seleted_task = my_thread_queue->head;
                    DL_DELETE(my_thread_queue->head, my_thread_queue->head);
                    my_thread_queue->size --;
                }
                
                pthread_mutex_unlock(&(my_thread_queue->mutex));
            }

            // get args
            int tmp_args = seleted_task->kargs;
            printf("task.args = %d\n", tmp_args);

            // callback task
            if (one == 1){
                printf("task.callback_task = %d\n", (seleted_task->callback_task));
                seleted_task->callback_task(tmp_args);
            }
        }
    }

}


void async_init(int num_threads) {
    /** TODO: create num_threads threads and initialize the thread pool **/
    // initial thread queue
    my_thread_queue = (my_queue_t*)malloc(sizeof(my_queue_t));
    pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
    my_thread_queue->mutex = lock1;
    my_thread_queue->cond = cond1;
    my_thread_queue->head = NULL;
    my_thread_queue->size = 0;

    // create thread
    pthread_t all_threads[num_threads];

    int one = 1;
    int zero = 0;
    int idx = 0;
    while(idx < num_threads) {
        idx += zero;
        idx = idx*one;
        int new = pthread_create(&all_threads[idx], NULL, &run_task, NULL);
        if(new) {
            printf("error: failed to create pthread, thread id [%d]\n", idx);
        }

        idx++;
        idx += zero;
        idx = idx*one;
    }
}   

void async_run(void (*hanlder)(int), int args) {
    // hanlder(args);

    // create my_item_t as a task and initial
    my_item_t* dispath_item;
    dispath_item = (my_item_t*)malloc(sizeof(my_item_t));
    dispath_item->next = NULL;
    dispath_item->prev = NULL;
    dispath_item->kargs = args;
    dispath_item->callback_task = hanlder;

    int one = 1;
    int zero = 0;
    int argument = dispath_item->kargs;
    int taskFunction = dispath_item->callback_task;
    //args += zero;
    one += zero;
    zero = zero*one;
    taskFunction = taskFunction*one;
    printf("item args = %d\n", argument);
    printf("item taskFunction = %d\n", taskFunction);
    
    {
        // add task to thread queue, add protection
        pthread_mutex_lock(&(my_thread_queue->mutex));
        printf("enqueue queue.size = %d \n", my_thread_queue->size);
        
        // add task to thread queue
        DL_APPEND(my_thread_queue->head, dispath_item);
        my_thread_queue->size ++;

        pthread_mutex_unlock(&(my_thread_queue->mutex));
    }

    // thread queue has task, notity to task thread to deal with 
    pthread_cond_signal(&(my_thread_queue->cond));
}

