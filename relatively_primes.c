#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

typedef struct {
    int x;
    double y;
}data_xy;


data_xy* profiler_process( char* (*fun_ptr)(int), int *limits, int num_limits, int num_runs )
{
    data_xy * run_times = malloc( sizeof(data_xy)*num_runs*num_limits );
    if( run_times == NULL)
        return NULL;

    time_t tic, toc;
    char *data = NULL;

    for( int ii=0; ii<num_limits; ++ii){
        printf("------------------------------\n");
        printf("For size table %d : \n",limits[ii]);
        
        for( int run=0; run<num_runs; ++run ){
            printf("\trunning test %d... ",run);

            tic = clock();
            data = fun_ptr( limits[ii] );
            toc = clock();
            
            run_times[ii].x = limits[ii];
            run_times[ii].y = (double)(toc - tic)/CLOCKS_PER_SEC;
            free(data);

            printf(" took %f secs\n", run_times[ii].y);
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

    printf("\n\n\n");

}

/**
 * @brief decide if two numbers are relatively prime
 *     In mathematics, two integers a and b are coprime, relatively prime or mutually prime 
 *     if the only positive integer that is a divisor of both of them is 1. 
 *     Consequently, any prime number that divides a does not divide b, and vice versa. 
 *     This is equivalent to their greatest common divisor (GCD) being 1.
 *     One says also a is prime to b or a is coprime with b. 
 * 
 * @param a positive number
 * @param b positive number
 * @return int - 1 if are relatively primes 0 in c.c.
 */
int are_relatively_prime( int a , int b)
{
    int min_value = a < b ? a : b;

    for( int i=2; i<= min_value; ++i)
        if( a%i == 0 && b%i==0 )
            return 0;

    return 1;
}

/**
 * @brief generate a square data table that contains 1 in the position i,j if GCD(i,j) == 1
 *         In mathematics, the greatest common divisor (GCD) of two or more integers, 
 *         which are not all zero, is the largest positive integer that divides each of the integers.
 * 
 * @param size_table width and height of the table 
 * @return char* linear array that contains a table data, each row in table is a chunk data in the array. 
 *               i-th row is allocated from the i*size_table position to (i+1)*size_table - 1 position
 */
char * generate_table_relatively_prime( int size_table )
{
    char *data_table = malloc( sizeof(char) * size_table * size_table );

    if( data_table == NULL )
        return NULL;

    memset( data_table, ' ', size_table*size_table );

    int offset = 0;
    for( int row=0; row<size_table; ++row) {
        for( int col=0; col<size_table; ++col) {
            if( are_relatively_prime(row,col) )
                data_table[offset] = '1';
            ++offset;
        }
    }

    return data_table;
}

/**
 * @brief Given data from a flat table, create an image file in PPM format. 
 *      Assigns a pixel color to each one table entry in the image. 
 *      WARNING: data overflow if exist mistmatch between memory allocated/reseved for data
 *               and passed arguments width and height 
 * @param data the flatten data array
 * @param width  num of cols in the image/data  
 * @param height num of rows in the image/data
 * @param name_file the name file to give the PPM image 
 */
void dump_to_PPM_file( char *data, int width, int height, const char* name_file )
{
    FILE* ppm_file = fopen(name_file,"w");

    fprintf(ppm_file,"P3\n%d %d\n255\n", width, height );

    int value, offset = 0;
    for(int row=0; row<height; ++row) {
        for(int col = 0; col<width; ++col ) {
            value = data[offset++] == '1' ? 255 : 0;
            fprintf(ppm_file,"%d %d %d   ", value, value, value );
        }
        fprintf(ppm_file,"\n");
    }

    fclose(ppm_file);
}


void do_process()
{
    printf(" \n\n Table Relative Prime \n\n");
    int size_table = 1248;
    const char* out_ppm = "relatively_primes.ppm";

    printf( "size table = %d \n\n", size_table);

    clock_t tic = clock();  //time_t tic = clock();

    char* data = generate_table_relatively_prime( size_table );

    clock_t toc = clock();  //time_t toc = clock();
    double time_spent = (double)(toc - tic) / CLOCKS_PER_SEC; // toc - tic
    printf("The elapsed time is %f seconds\n\n", time_spent);

    dump_to_PPM_file( data, size_table, size_table, out_ppm );

    printf( "image generated %s \n\n", out_ppm );

    // release dynamic memory 
    free( data );

}

int main()
{
    printf(" \n\n\t\t Profiling Relative Primes \n\n");

    int num_limits = 3;
    int limits[10] = { 500, 600, 700 }; //, int(1e9), int(1e10), int(1e11), int(1e12) };
    int num_runs = 3;

    data_xy *run_times = profiler_process( generate_table_relatively_prime, limits, num_limits, num_runs );

    plot_profile( run_times, num_limits*num_runs );

    free(run_times);

    return 0;
}