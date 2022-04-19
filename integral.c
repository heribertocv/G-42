#include <stdio.h>
#include <math.h>
#include <time.h> 
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>


/**
 * @brief 
 * 
 * @param x 
 * @return double 
 */
double f( double x )
{
    return 3*sin(x/4) + 1.45;
//    return 3*x + 1.45; 
}

double f_test( double x )
{
    return 1.0; 
}

/**
 * @brief 
 * 
 * @param fun_ptr 
 * @param a 
 * @param b 
 * @param num_subdivisions 
 * @return double 
 */
double numerical_integration( double (*fun_ptr)(double), double a, double b, int num_subdivisions )
{
    double dx = (b-a) / num_subdivisions;
    double integral = fun_ptr(a);

    for(long int n=1; n<num_subdivisions; ++n )
        integral += fun_ptr( a + n*dx );

    return integral * dx;
}


typedef struct{
    double (*fun_ptr)(double);
    double a;
    double b;
    int num_subdivisions;
} chunk_work;

typedef struct{
    double partial_integral;
}partial_result;

typedef struct{
    double total_integral;
}total_result;

typedef struct{
    int fd[2];
}child_chann_com;

typedef struct{
    int num_jobs;
    chunk_work* list_chunk_works;
    child_chann_com* list_child_chann_com;
    int* list_pid_slaves;
}admin_jobs;

void release_admin_jobs(admin_jobs theAdminJobs )
{
    free( theAdminJobs.list_chunk_works );
    free( theAdminJobs.list_child_chann_com );
    free( theAdminJobs.list_pid_slaves );
}

chunk_work* generate_limits_chunk_works(double (*fun_ptr)(double), int a, int b, int num_total_subdivisions, int num_jobs )
{
    double size_chunk = (b-a) / num_jobs;
    int chunk_subdivisions = num_total_subdivisions / num_jobs;
    chunk_work* chunk_jobs = (chunk_work*) malloc( sizeof(chunk_work) * num_jobs );

    for(int ii=0; ii<num_jobs; ++ii)
    {
        chunk_jobs[ii].a = size_chunk*ii + a;
        chunk_jobs[ii].b = size_chunk*(ii+1) + a;
        chunk_jobs[ii].num_subdivisions = chunk_subdivisions;
        chunk_jobs[ii].fun_ptr = fun_ptr;
    }
    chunk_jobs[num_jobs-1].b = b;

    return chunk_jobs;
}

child_chann_com* create_channel_comunication( int num_channels )
{
    child_chann_com* theChannelsCom = (child_chann_com*)malloc( sizeof(child_chann_com)*num_channels);
    for(int ii=0; ii<num_channels; ++ii)
    {
        pipe( theChannelsCom[ii].fd );
    }

    return theChannelsCom;
}

admin_jobs distribute_work(  double (*fun_ptr)(double), int a, int b, int num_total_subdivisions, int num_jobs )
{
    // populate the JCB
    admin_jobs theJobs;
    theJobs.num_jobs = num_jobs;
    theJobs.list_chunk_works = generate_limits_chunk_works( fun_ptr, a, b, num_total_subdivisions, num_jobs );
    theJobs.list_child_chann_com = create_channel_comunication( num_jobs );
    theJobs.list_pid_slaves  = (int*) malloc( (sizeof(int)*num_jobs ) );
    return theJobs;
}


void slave_work( chunk_work theChunkWork, int fd_pipe_out)
{
    double partial_result = numerical_integration( theChunkWork.fun_ptr, theChunkWork.a, theChunkWork.b, theChunkWork.num_subdivisions );

    // send the partial_result to parent process
    char buffer[128];
    sprintf(buffer,"%f",partial_result);
    write( fd_pipe_out, buffer, strlen(buffer)+1);

    exit(0);
}

partial_result get_slave_partial_result( int pid_slave, int fd_chann_com )
{
    partial_result result;
    char buffer_recieve[128];
    int status;


    // waiting for partial results from the slaves
    waitpid( pid_slave, &status, 0);
    //unserealize the partial result 
    read( fd_chann_com, buffer_recieve, sizeof(buffer_recieve) ); 

    // dump to wrapper result 
    sscanf(buffer_recieve,"%lf", &result.partial_integral );

    return result;
}

void assamble_total_result( total_result *theTotalResult, partial_result thePartialResult )
{
    theTotalResult->total_integral += thePartialResult.partial_integral;
}


double parallel_numerical_integratio( double (*fun_ptr)(double), double a, double b, long int num_subdivisions, int num_sub_process )
{
    admin_jobs admin_slaves = distribute_work(fun_ptr, a, b, num_subdivisions, num_sub_process );

    // Launch the slaves ...
    for( int ii=0; ii<num_sub_process; ++ii)
    {
        // create slaves ...
        admin_slaves.list_pid_slaves[ii] = fork();

        if( admin_slaves.list_pid_slaves[ii] == 0 )
        {  // Child's Code
            slave_work( admin_slaves.list_chunk_works[ii],
                        admin_slaves.list_child_chann_com[ii].fd[1] );
        }
    }

    // Wait the partial result from the slaves ...
    // compute the total result
    partial_result slave_result;
    total_result theTotalResult;
    theTotalResult.total_integral = 0;

    for( int ii=0; ii< num_sub_process; ++ii){
        slave_result = get_slave_partial_result( admin_slaves.list_pid_slaves[ii],
                                                    admin_slaves.list_child_chann_com[ii].fd[0] );
        // assemble total result 
        assamble_total_result( &theTotalResult, slave_result );
    }

    release_admin_jobs( admin_slaves );

    return theTotalResult.total_integral;
}

void numerical_integration_serial( void )
{
    printf(" \n\n\t\t TESTING Numerical Integration \n\n");

    long int num_subdivisions = 10;
    double estimate_integral;

    printf("num_subdivisions : %ld\n",num_subdivisions);
    time_t tic, toc;
    tic = clock();
    estimate_integral = numerical_integration( f, 1, 11, num_subdivisions );
    toc = clock();
    printf(" took %f secs\n\n", (double)(toc - tic)/CLOCKS_PER_SEC);

    printf("Estimate Integral: %f\n\n\n",estimate_integral);
}

int main(void)
{ 
    printf(" \n\n\t\t TESTING Numerical Integration \n\n");

    long int num_subdivisions = (int)1e7;
    double estimate_integral;

    estimate_integral = numerical_integration( f, 1, 11, num_subdivisions );
    printf("Serial --- Estimate Integral: %f\n",estimate_integral);

    estimate_integral = parallel_numerical_integratio( f, 1, 11, 
    num_subdivisions, 4 );
    printf("Parallel --- Estimate Integral: %f\n\n\n",estimate_integral);

    return 0;
}
