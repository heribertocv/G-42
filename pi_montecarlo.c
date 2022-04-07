#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 


/**
 * @brief 
 * 
 * @param num_throws 
 * @return double 
 */
double estimate_pi_montecarlo( long int num_throws )
{
    double x,y;
    long int num_inside = 0;

    for(long int ii=0; ii< num_throws; ++ii)
    {
        x = (double)rand()/(double)RAND_MAX;
        y = (double)rand()/(double)RAND_MAX;
        if( x*x + y*y <= 1.0 )
            ++num_inside;
    }

    return (double)num_inside / (double)num_throws * 4;
}

int main(void)
{
    printf(" \n\n\t\t TESTING PI-Montecarlo \n\n");

    long int num_throws = 1e8;
    double estimate_pi;

    printf("num throws : %ld\n",num_throws);
    time_t tic, toc;
    tic = clock();
    estimate_pi = estimate_pi_montecarlo( num_throws );
    toc = clock();
    printf(" took %f secs\n\n", (double)(toc - tic)/CLOCKS_PER_SEC);

    printf("Estimate value PI: %f\n\n\n",estimate_pi);

    return 0;
}