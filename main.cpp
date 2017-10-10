#include <iostream>
#include <time.h>
#include<unistd.h>
#include <pthread.h>

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
float (*funcPtr) (float,int);

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

//int computeValue(float (*ptr) (float,int), int a , int b, int n, int intensity)
void computeValue( int a , int b, int n, int intensity)
{
    //std::cerr<<"a = "<<a<<"\nb = "<<b<<"\nn = "<<n<<"\nintensity = "<<intensity<<std::endl;
    register double sum =0;
    register float multiplier = ((b-a)/(float)n);
    std::cerr<<"Multiplier = "<<multiplier<<std::endl;
    for (register int i=0;i<n;i++)
    {
        sum+=funcPtr(a+(i+0.5)*multiplier, intensity)*multiplier;
    }
    std::cout<<sum;
}

int main (int argc, char* argv[]) {

    if (argc < 6) {
        std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
        return -1;
    }
    timespec begin, end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin);

    switch(convertToInt(argv[1]))
    {
        case 1:
            funcPtr = &f1;
            computeValue(convertToInt(argv[2]), convertToInt(argv[3]), convertToInt(argv[4]), convertToInt(argv[5]));
            break;
        case 2: 
            funcPtr = &f2;
            computeValue(convertToInt(argv[2]), convertToInt(argv[3]), convertToInt(argv[4]), convertToInt(argv[5]));
            break;
        case 3: 
            funcPtr = &f3;
            computeValue(convertToInt(argv[2]), convertToInt(argv[3]), convertToInt(argv[4]), convertToInt(argv[5]));
            break;
        case 4: 
            funcPtr = &f4;
            computeValue(convertToInt(argv[2]), convertToInt(argv[3]), convertToInt(argv[4]), convertToInt(argv[5]));
            break;
        default:
            std::cerr<<"Invalid function number provided\n";
            break;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    double timeTaken = ((end.tv_sec - begin.tv_sec )+(end.tv_nsec - begin.tv_nsec)/1e9);
    std::cerr<<timeTaken<<std::endl;
    return 0;
}
