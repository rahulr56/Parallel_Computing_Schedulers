// C++ inclusions
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>
// C inclusions
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// Global variables for sync type
#define SYNC_THREAD "thread"
#define SYNC_ITER "iteration"

// C code for finction inclusions from the library
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

// Function pointer to define the function to be called from the library
float (*funcPtr) (float,int);

pthread_mutex_t iteratorMutex, threadMutex, time_lock;
//std::chrono::high_resolution_clock::time_point progStart ;

struct threadLevelSchedulerData 
{
    int a, start, end, intensity;
    float multiplier;
    double *result;
    int threadId;
};
// Function to convert char* to int
int convertToInt(char* val)
{
    try{
        return atoi(val);
    }
    catch(...)
    {
        std::cerr<<"An exception occured while convering string to int.\nPlease check the passed arguments\n";
        exit(-1);
    }
}

void* computeUsingIterator(void* data)
{
    struct threadLevelSchedulerData *it = (struct threadLevelSchedulerData*) data;
    time_t start1;
    time(&start1);
    std::chrono::high_resolution_clock::time_point end;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    for (register int i = it->start; i <= it->end ; i++)
    {
        double val =  (float)funcPtr(it->a+(i+0.5)*it->multiplier, it->intensity) * it->multiplier;
        pthread_mutex_lock(&iteratorMutex);
        *it->result += val;
        pthread_mutex_unlock(&iteratorMutex);
    }
    //time(&end1);
    //std::cout<<it->threadId<<"\t"<<(float)start1<<"\t"<<(((float)end1-(float)start1)/CLOCKS_PER_SEC)<<std::endl;
    end = std::chrono::high_resolution_clock::now();
    pthread_mutex_lock(&time_lock);
    std::cout<<it->threadId<<"\t"<<(float)start1<<"\t"<<std::chrono::duration<double>(end - start).count()<<std::endl;
    pthread_mutex_unlock(&time_lock);
}

void* computeUsingThreads(void* data)
{
    struct threadLevelSchedulerData *it = (struct threadLevelSchedulerData*) data;
    register double* localSum = (double*)malloc(sizeof(double));
    *localSum = 0;
    time_t start1, end1;
    std::chrono::high_resolution_clock::time_point end;
    time(&start1);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    for (register int i = it->start; i <= it->end ; i++)
    {
        *localSum += (float)funcPtr(it->a+(i+0.5)*it->multiplier, it->intensity) * it->multiplier;
    }
    pthread_mutex_lock(&threadMutex);
    *(it->result) += *localSum;
    pthread_mutex_unlock(&threadMutex);
    time(&end1);
    end = std::chrono::high_resolution_clock::now();
    pthread_mutex_lock(&time_lock);
    std::cout<<it->threadId<<"\t"<<(float)start1<<"\t"<<std::chrono::duration<double>(end - start).count()<<std::endl; // (float)end1<<std::endl;//std::chrono::duration<double>(end - start).count()<<std::endl;
    pthread_mutex_unlock(&time_lock);
}

void computeValue(int a , int b, int n, int intensity, int numThreads, bool sync)
{
    pthread_t threads[numThreads];
    struct threadLevelSchedulerData* tData[numThreads];
    float multiplier = (b - a) / (float)n;
    double *sum = (double *)malloc(sizeof(double));
    *sum = 0;

    for(register int i = 0; i < numThreads; i++)
    {
        tData[i] = (struct threadLevelSchedulerData*) malloc(sizeof(struct threadLevelSchedulerData));
        tData[i]->a = a;
        tData[i]->intensity = intensity;
        tData[i]->multiplier = multiplier;
        tData[i]->start = (n / numThreads) * i;
        tData[i]->end = (n / numThreads) * (i + 1) - 1;
        tData[i]->result = sum;
        tData[i]->threadId = i;
        int res ;
        if (sync)
        {
            res = pthread_create(&threads[i], NULL, computeUsingThreads, (void*)tData[i]);
        }
        else
        {
            res = pthread_create(&threads[i], NULL, computeUsingIterator, (void*)tData[i]);
        }
        if (res < 0)
        {
            std::cerr<<"Failed to create thread number :"<<i<<std::endl;
        }
    }

    for (register int i=0; i < numThreads; i++)
        pthread_join(threads[i],NULL);
//    std::cout<<*sum<<std::endl;
}

int main (int argc, char* argv[]) {

    if (argc < 8) {
        std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
        return -1;
    }
    pthread_mutex_init (&iteratorMutex, NULL);
    pthread_mutex_init (&time_lock, NULL);
    pthread_mutex_init (&threadMutex, NULL);

    bool sync = false;

    //Sync type Identifier	
    if(strcmp(argv[7], SYNC_THREAD) == 0)
    {
        sync = true;
    }

    // Function ID identifier
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
    time_t start_prog;
    //progStart = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    time(&start_prog);
    computeValue(convertToInt(argv[2]), convertToInt(argv[3]), convertToInt(argv[4]), convertToInt(argv[5]), convertToInt(argv[6]), sync);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    std::cerr<<"Main\t" <<(float)start_prog<<"\t"<<elapsed_seconds.count()<<std::endl;

    return 0;
}
