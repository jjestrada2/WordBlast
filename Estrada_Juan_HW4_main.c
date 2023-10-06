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
int extraBytes = 0;  
int file, blockSize;
pthread_mutex_t lock;


typedef struct wordFrequency {
    char * word;
    int count;
} wordFrequency;


struct wordFrequency wordArray[MAXWORDS];


void *wordFrequencyCount(void *ptr) {  
    int result;
    char * buffer;
    char * token;

   // Allocate a buffer with enough space for blockSize and extraBytes
    buffer = malloc(blockSize+extraBytes);

   
     // Read data from the file into the buffer
    read(file, buffer, blockSize+extraBytes);

    // Tokenize the buffer using strtok_r
    while (token = strtok_r(buffer, delim, &buffer)) {  
         // Check if the token has at least 6 characters
        if ((int)strlen(token) >= 6) {  
           
           // Loop through the wordArray to find a match for the token
            for (int i = 0; i < MAXWORDS; i++) {
                // Compare the token with the current word in the array
                result = strcasecmp(wordArray[i].word, token);

                // If a match is found, update the count and exit the loop
                if (result == 0) {
                    //Synchronization (avoid race condition)
                    pthread_mutex_lock(&lock);   
                    wordArray[i].count++;
                    pthread_mutex_unlock(&lock); 
                    break;
                } 
            }
            // If no match is found, add the token to the wordArray
            if (result != 0) {
                if (count < MAXWORDS) {
                  //Synchronization (avoid race condition)
                    pthread_mutex_lock(&lock); 
                    strcpy(wordArray[count].word, token);
                    wordArray[count].count++;
                    pthread_mutex_unlock(&lock);  
                         
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
    file = open(argv[1], O_RDONLY);

    // Use lseek to determine the size of the file
    fileSize = lseek(file, 0, SEEK_END);

    // Reset the file position back to the start
    lseek(file, 0, SEEK_SET);

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
            extraBytes = fileSize % numOfThreads;
        }
        // start new thread 
        if (r2 = pthread_create(&thread[i], NULL, wordFrequencyCount, (void*) &i)) {
            printf("ERROR: Thread creation failed [%d]\n", r2);
            exit(EXIT_FAILURE);
        }
    }

 // Wait for all threads to complete execution
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
    close(file);
    // must destroy lock
    pthread_mutex_destroy(&lock);
    // Free  buffers
    for (int i = 0; i < MAXWORDS; i++) {
        free(wordArray[i].word);
    }
    return 0;
    }