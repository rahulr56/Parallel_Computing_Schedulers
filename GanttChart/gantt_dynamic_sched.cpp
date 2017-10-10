#include <iostream>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Global variables for sync type
#define SYNC_THREAD "thread"
#define SYNC_CHUNK "chunk"
#define SYNC_ITER "iteration"

#ifdef __cplusplus
extern "C" {
#endif

    float f1(float x, int intensity);
    float f2(float x, int intensity);
    float f3(float x, int intensity);
    float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

int numChunks = 0;
// Function pointer to define the function to be called from the library
float (*funcPtr) (float,int);
void (*computeFuncPtr) (void *) ;

pthread_mutex_t threadMutex, workQueueMutex, timeMutex;

struct threadData 
{
    int a, start, end, intensity, id; 
    float multiplier;
    double * result;
    double threadResult;
};

std::vector<struct threadData*> workQueue;

int convertToInt(char* val)
{
    try{
        return atoi(val);
    }
    catch(...)
    {
        std::cerr<<"An exception occured while convering string to int.\nPlease check the passed arguments\n";
        exit(-2);
    }
}

bool done()
{
    /*
       pthread_mutex_lock(&workQueueMutex);
       bool returnVal = workQueue.empty();
       pthread_mutex_unlock(&workQueueMutex);
       return returnVal; 
       */
    if(numChunks > 0)
        return true;
    return false;
}

struct threadData* getNext()
{
    struct threadData* val = (struct threadData*)malloc(sizeof(struct threadData));
    pthread_mutex_lock(&workQueueMutex);
    //    val = workQueue.front();
    //    workQueue.erase(workQueue.begin());
    if(numChunks){
        val = workQueue.at(numChunks-1);
        numChunks--;
    }
    else{
        pthread_mutex_unlock(&workQueueMutex);
        return NULL;
    }
    //    memcpy(*val,*work.back(), sizeof(struct threadData));
    //workQueue.pop_back(); 
    pthread_mutex_unlock(&workQueueMutex);
    return val;
}

void* workerThreadExecution(void* p)
{
    while(numChunks)
    {
        struct threadData *x = getNext();
        if(x)
            computeFuncPtr(x);
    }
}

void computeUsingThreads(void* p)
{
    if(p==NULL)
        return;
    struct threadData *it = (struct threadData*) p;
    double localSum = 0;
    time_t s;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    time(&s);
    for (register int i = it->start; i < it->end; i++)
    {
        it->threadResult +=  (float)funcPtr(it->a+(i+0.5)*it->multiplier, it->intensity) * it->multiplier;
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    
    {
        pthread_mutex_lock(&timeMutex);
        std::cout<<std::fixed;
        std::cout<<it->id<<"\t"<<(float)s<<"\t"<<std::chrono::duration<double>(end - start).count()<<std::endl;
        pthread_mutex_unlock(&timeMutex);
    }
}

void computeUsingChunks(void* p)
{
    if(p==NULL)
        return;
    struct threadData *it = (struct threadData*) p;
    double localSum = 0;
    time_t s;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    time(&s);
    for (register int i = it->start; i < it->end; i++)
    {
        localSum +=  (float)funcPtr(it->a+(i+0.5)*it->multiplier, it->intensity) * it->multiplier;
    }
    pthread_mutex_lock(&threadMutex);
    *it->result += localSum;
    pthread_mutex_unlock(&threadMutex);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    pthread_mutex_lock(&timeMutex);
    std::cout<<it->id<<"\t"<<(float)s<<"\t"<<std::chrono::duration<double>(end - start).count()<<std::endl;
    pthread_mutex_unlock(&timeMutex);
}

void computeUsingIterator(void* p)
{
    if(p==NULL)
        return;
    struct threadData *it = (struct threadData*) p;
    time_t s;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    time(&s);
    for (register int i = it->start; i < it->end; i++)
    {
        double val =  (float)funcPtr(it->a+(i+0.5)*it->multiplier, it->intensity) * it->multiplier;
        pthread_mutex_lock(&threadMutex);
        *it->result += val;
        pthread_mutex_unlock(&threadMutex);
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    pthread_mutex_lock(&timeMutex);
    std::cout<<it->id<<"\t"<<(float)s<<"\t"<<std::chrono::duration<double>(end - start).count()<<std::endl;
    pthread_mutex_unlock(&timeMutex);
}

void computeValue(int a , int b, int n, int intensity, int numThreads, short sync, int granularity)
{
    float multiplier  = (b - a)/(float)n;
    struct threadData *data[n/granularity];
    pthread_t threads[numThreads];
    double* sum = (double*)malloc(sizeof(double));
    *sum = 0;
    numChunks = n/granularity;
    int loop = 0;
    computeFuncPtr = computeUsingThreads;
    if ( sync == 1 )
    {
        computeFuncPtr = computeUsingChunks;
    }
    else if(sync == 0)
    {
        computeFuncPtr = computeUsingIterator;
    }

    for (register int i=0; i < n/granularity; i++)
    {
        data[i] = (struct threadData*)malloc(sizeof(struct threadData));
        (*data[i]).a = a;
        (*data[i]).id = i;
        data[i]->multiplier = multiplier;
        data[i]->intensity = intensity;
        data[i]->result = sum;
        data[i]->start  = loop;
        int val = loop + granularity;
        data[i]->end  = val;
        data[i]->threadResult = 0;
        loop = val;
        workQueue.push_back(data[i]);
    }
    for(int i=0;i<numThreads;i++)
    {
        pthread_create(&threads[i], NULL, workerThreadExecution, NULL); 
    }
    for(int i=0;i<numThreads;i++)
    {
        pthread_join(threads[i], NULL);
    }
    if(sync==2)
    {
        for(int i=0; i<n/granularity; i++)
        {
            *sum += data[i]->threadResult;
        }
    }
    std::cout<<*sum<<std::endl;
}

int main (int argc, char* argv[]) 
{
    if (argc < 9) {
        std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
        return -1;
    }
    short sync = 0;
    pthread_mutex_init (&threadMutex, NULL);
    pthread_mutex_init (&timeMutex, NULL);
    pthread_mutex_init (&workQueueMutex, NULL);

    if(strcmp(argv[7], SYNC_THREAD) == 0)
    {
        sync = 2;
    }
    else if (strcmp(argv[7], SYNC_CHUNK) == 0)
    {
        sync = 1;
    }
    switch(convertToInt(argv[1]))
    {
        case 1:
            funcPtr = &f1;
            break;
        case 2: 
            funcPtr = &f2;
            break;
        case 3: 
            funcPtr = &f3;
            break;
        case 4: 
            funcPtr = &f4;
            break;
        default:
            std::cerr<<"Invalid function number provided\n";
            exit(-1);
    }
    time_t s;
    time(&s);
    std::cout<<"Start of main :"<<std::fixed<<(double)s<<std::endl;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    computeValue(convertToInt(argv[2]), convertToInt(argv[3]), convertToInt(argv[4]), convertToInt(argv[5]), convertToInt(argv[6]), sync, convertToInt(argv[8]));

    // end time
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<<elapsed_seconds.count()<<std::endl;

    pthread_mutex_destroy(&threadMutex);
    pthread_mutex_destroy(&workQueueMutex);
    return 0;
}
