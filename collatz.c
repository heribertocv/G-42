#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

typedef struct{
    long int *n;
    long int *orbit;
    long int size;
}collatz_table;

typedef struct {
    int x;
    double y;
}data_xy;


data_xy* profiler_process( collatz_table (*fun_ptr)(int), int *args, int num_args, int num_runs )
{
    data_xy * run_times = malloc( sizeof(data_xy)*num_runs*num_args );
    if( run_times == NULL)
        return NULL;

    time_t tic, toc;
    collatz_table table;

    for( int ii=0; ii<num_args; ++ii){
        printf("------------------------------\n");
        printf("For size table %d : \n",args[ii]);
        
        for( int run=0; run<num_runs; ++run ){
            printf("\trunning test %d... ",run);

            tic = clock();
            table = fun_ptr( args[ii] );
            toc = clock();
            
            run_times[ii].x = args[ii];
            run_times[ii].y = (double)(toc - tic)/CLOCKS_PER_SEC;

            printf(" took %f secs\n", run_times[ii].y);

            free(table.n);
            free(table.orbit);
        }
    }

    return run_times;
}

void plot_profile(data_xy *data, int num_data)
{
    FILE *gnuplot = popen("gnuplot", "w");
    //fprintf(gnuplot, "set terminal pngcairo\n");
    //fprintf(gnuplot, "set output 'primes.png'\n");
    fprintf(gnuplot, "set grid\n");
    fprintf(gnuplot, "plot '-'\n");

    for (int i = 0; i < num_data; i++)
        fprintf(gnuplot, "%g %g\n", (double)data[i].x, data[i].y);
    fprintf(gnuplot, "e\n");
    fprintf(gnuplot, "pause mouse \n"); 
    fprintf(gnuplot,"unset output\n");
    fflush(gnuplot);
}


/**
 * @brief 
 * 
 * @param n 
 */
long int orbit_collatz( long int n )
{
    long int len = 1;
    if( n<1 )
        return 0;

    while( n != 1 ){
        if( n%2 == 0)
            n /= 2;
        else
            n = n*3 + 1;
        
        ++len;
    }

    return len;
}

/**
 * @brief 
 * 
 * @param size_table 
 * @return collatz_table 
 */
collatz_table generate_collatz_table( long int  size_table )
{
    collatz_table theTable;
    theTable.size = size_table;
    theTable.n = NULL;
    theTable.orbit = NULL;
    theTable.n = malloc( sizeof(long int) * size_table );
    theTable.orbit = malloc( sizeof(long int) * size_table );
    if( theTable.n == NULL || theTable.orbit == NULL ){
        printf("ERROR: out of memory in generate_collatz_table\n");
        return theTable;
    }

    for( long int ii=0; ii<size_table; ++ii)
    {
        theTable.n[ii] = ii+2;
        theTable.orbit[ii] = orbit_collatz(ii+2);
    }

    return theTable;
}

void plot_collatz_table( collatz_table table )
{
   FILE *gnuplot = popen("gnuplot", "w");
    //fprintf(gnuplot, "set terminal pngcairo\n");
    //fprintf(gnuplot, "set output 'primes.png'\n");
    fprintf(gnuplot, "set grid\n");
    fprintf(gnuplot, "plot '-'\n");

    for (long int i = 0; i < table.size; i++)
        fprintf(gnuplot, "%g %g\n", (double)table.n[i], (double)table.orbit[i]);
    fprintf(gnuplot, "e\n");
    fprintf(gnuplot, "pause mouse \n"); 
    fprintf(gnuplot,"unset output\n");
    fflush(gnuplot);

    printf("\n\n\n");

}

int main(void)
{
    printf(" \n\n\t\t Profiling Collatz \n\n");

    int size_table = 4e5;

    printf("size table : %d\n",size_table);
    time_t tic, toc;
    tic = clock();
    collatz_table table_data = generate_collatz_table( size_table );
    toc = clock();

    printf(" took %f secs\n", (double)(toc - tic)/CLOCKS_PER_SEC);

    //plot_collatz_table( table_data );
    
    return 0;
}