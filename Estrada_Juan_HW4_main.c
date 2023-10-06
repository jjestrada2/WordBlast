/**************************************************************
* Class:  CSC-415-03 Fall 2023
* Name: Juan Estrada
* Student ID: 923058731
* GitHub ID: jjestrada2
* Project: Assignment 4 – Word Blast
*
* File: Estrada_Juan_HW4_main.c
*
* Description:implement a concurrent word frequency counter 
in C using POSIX threads. The program takes a text file as 
input, divides it into chunks, and assigns each chunk to a 
separate thread. Each thread processes its assigned chunk, 
tokenizes the text, and updates a shared data structure to 
count the frequency of words. To ensure thread safety, a 
mutex is employed to protect critical sections. After all 
threads complete their tasks, the program sorts and 
identifies the top 10 most frequently occurring words 
with at least six characters. The execution time is measured,
 and the results are displayed, providing an efficient and 
 parallelized solution for analyzing word frequencies in l
 arge text files.

*
**************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define MAXWORDS 2500 
#define TOPWORDS 10  
// You may find this Useful
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

int count = 0;     
int setFlag = 0;  
int excessBytes = 0;  
int fileDescriptor, blockSize;
pthread_mutex_t lock;

//Struct to hold word and it's number of occurence
typedef struct wordFrequency {
    char * word;
    int count;
} wordFrequency;

//struct object to hold array of words
struct wordFrequency wordArray[MAXWORDS];

// this function is used by threads to process file in blocks
void *wordProcess(void *ptr) {  
    int result;
    char * buffer;
    char * token;
    // allocatee buffer with excess bytes
    buffer = malloc(blockSize+excessBytes);

    //Read will attempt to read upto blockSize+excessBytes
    //from file descriptor into the buffer
    read(fileDescriptor, buffer, blockSize+excessBytes);

    // loop through buffer and parse strings into sequence of tokens
    while (token = strtok_r(buffer, delim, &buffer)) {  
        // check if token is greater or equal to  6 character,
        if ((int)strlen(token) >= 6) {  
            //loop through wordarray to check token 
            for (int i = 0; i < MAXWORDS; i++) {
                //strcmp will comapare word in wordarray with token
                //if matched sucessfully, returns 0
                result = strcasecmp(wordArray[i].word, token);
                //if match result is 0 and increase count
                if (result == 0) {
                    //avoids race condition, critical section
                    pthread_mutex_lock(&lock);   
                    wordArray[i].count++;
                    // End critical section
                    pthread_mutex_unlock(&lock); 
                    break;
                } 
            }
            // if no match found
            if (result != 0) {
                if (count < MAXWORDS) {
                    // avoids race condition, critical section
                    pthread_mutex_lock(&lock); 
                    //insert token to word array           
                    strcpy(wordArray[count].word, token);
                    wordArray[count].count++;
                    // End critical section
                    pthread_mutex_unlock(&lock);  
                    //increment count        
                    count++;
                }
            }
        }
    }
}



int main (int argc, char *argv[])
    {
    //***TO DO***  Look at arguments, open file, divide by threads
    //             Allocate and Initialize and storage structuresult

    int r1, r2, fileSize;

     
    // Convert the number of threads from a string to an integer in a thread-safe manner
    int numOfThreads = atoi(argv[2]);  

    // If the flag is 0, initialize the wordArray and set the flag to 1
    if (setFlag == 0) {
        for (int i = 0; i < MAXWORDS; i++) {
            // Allocate memory for each word in the wordArray
            wordArray[i].word = malloc(TOPWORDS);
            wordArray[i].count = 0;
        }
        setFlag = 1;
    }
    
    // Initialize the mutex lock and check for errors
    if (r1 = pthread_mutex_init(&lock, NULL)) {
        printf("ERROR: Mutex initialization failed [%d]\n", r1);
    }

    // Open the file from the given command-line argument
    fileDescriptor = open(argv[1], O_RDONLY);

    // Use lseek to determine the size of the file
    fileSize = lseek(fileDescriptor, 0, SEEK_END);

    // Reset the file position back to the start
    lseek(fileDescriptor, 0, SEEK_SET);

    // Determine the block size based on the number of threads
    blockSize = fileSize / numOfThreads;

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish
     // Declare pthread with number of threads
    pthread_t thread[numOfThreads];

    for (int i = 0; i < numOfThreads; i++) {
        //checking for last threads to adjust excess bytes with leftovers
        if (i == numOfThreads-1) {
            excessBytes = fileSize % numOfThreads;
        }
        // start new thread in calling process with some error checking
        if (r2 = pthread_create(&thread[i], NULL, wordProcess, (void*) &i)) {
            printf("ERROR: Thread creation failed [%d]\n", r2);
            exit(EXIT_FAILURE);
        }
    }

    // blocks the execution of calling threads
    //until target threads terminates
    // acts like wait()
    for (int i = 0; i < numOfThreads; i++) {
        pthread_join(thread[i], NULL);
    }

    // ***TO DO *** Process TOP 10 and display
    //temp variable
    wordFrequency temp;

    // loop will sort in descending order
    for (int i = 0; i < MAXWORDS; i++) {
        for (int j = i+1; j < MAXWORDS; j++) {
            if (wordArray[i].count < wordArray[j].count) {
                temp = wordArray[i];
                wordArray[i] = wordArray[j];
                wordArray[j] = temp;
            }
        }
    }

    printf("\n\n");
    printf("Word Frequency Count on %s with %d threads\n", argv[1], numOfThreads);
    printf("Printing top 10 words 6 characters or more.\n");

    // ***TO DO *** Process TOP 10 and display
      for (int i = 0; i < 10; i++) {
        printf("Number %d is %s with a count of %d\n", i+1, wordArray[i].word, wordArray[i].count);
    }

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************


    // ***TO DO *** cleanup
     // do necessary closing of the file
    close(fileDescriptor);
    // must destroy lock
    pthread_mutex_destroy(&lock);
    // Free  buffers
    for (int i = 0; i < MAXWORDS; i++) {
        free(wordArray[i].word);
    }
    return 0;
    }