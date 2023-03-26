#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


int fullRange;
int subRange;
int count;
int subPrimes[1000];
int main(int argc, char* argv[])
{
    clock_t start, end;
    double duration;
	int pid, np, elements_per_process, lower_bound, upper_bound,total_count=0;
    int n_elements_recieved;//for slaves
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &np);


	//----------------------master process---------------------------------
	if (pid == 0) {
        printf("Hello from master process.\n");
        printf("Number of slave processes is %d\n",np-1);
        //Read size of array
        printf("Enter lower bound:\n");
        scanf("%d",&lower_bound);
        
		//Read array
		printf("Enter upper bound:\n");
        scanf("%d",&upper_bound);

        fullRange = upper_bound - lower_bound;
        subRange= fullRange / (np-1);

        //print array///////////////////////////////////////////////////////////////////////////////////////////
        printf("Elements are:\n");
        for(int i=0;i<fullRange;i++)
        {
            printf("%d ",i+1);
        }

        printf("\n");
       
        start = clock();
	}//end master

    //process 0 sends lower bound and range to all other processes
    MPI_Bcast(&lower_bound,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&subRange,1,MPI_INT,0,MPI_COMM_WORLD);
    //calculate lower bound and upper bound
    int my_lower_bound = lower_bound + (pid-1)*subRange;
    int my_upper_bound = my_lower_bound + subRange;

    //slave scope
    if(pid != 0)
    {
        //count primes in range
         count = 0;
        for(int i=my_lower_bound;i<my_upper_bound;i++)
        {
            int isPrime = 1;
            for(int j=2;j<i;j++)
            {
                if(i%j == 0)
                {
                    isPrime = 0;
                    break;
                }
            }
            if(isPrime == 1 && i!=1)
            {
                subPrimes[count] = i;
                count++;
            }
        }
        printf("Process %d has %d primes in range %d to %d ",pid,count,my_lower_bound,my_upper_bound);
        printf("{");
            for (int j = 0; j < count; j++)
            {
                if (j == count-1)
                {
                    printf("%d",subPrimes[j]);
                    printf("}");
                }
                else printf("%d ,",subPrimes[j]);

            }
            printf("\n");

    }//end slave scope

        //master scope
       //master handle the remaining range
       if(fullRange %(np-1)!=0){
        if(pid == 0)
        {
            int my_lower_bound = lower_bound + (np-1)*subRange;
            int my_upper_bound = my_lower_bound + (fullRange % (np-1));
            //count primes in range
            count = 0;
            for(int i=my_lower_bound;i<my_upper_bound;i++)
            {
                int isPrime = 1;
                for(int j=2;j<i;j++)
                {
                    if(i%j == 0)
                    {
                        isPrime = 0;
                        break;
                    }
                }
                if(isPrime == 1 && i!=1)
                {
                    subPrimes[count] = i;
                    count++;
                }
            }
            printf("Process %d has %d primes in range %d to %d ",pid,count,my_lower_bound,my_upper_bound);
            printf("{");
            for (int j = 0; j < count; j++)
            {
                if (j == count-1)
                {
                    printf("%d",subPrimes[j]);
                    printf("}");
                }
                else printf("%d ,",subPrimes[j]);

            }
            printf("\n");
        }
        }//end master scope

        //recive counts from all other processes using MPI_Reduce
        MPI_Reduce(&count,&total_count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);


        if(pid == 0)
        {
            printf("Total number of primes in range %d to %d is %d ",lower_bound,upper_bound,total_count);
            printf("\n");
            end = clock();
            /* Get the time taken by program to execute in seconds */
            duration = ((double)end - start)/CLOCKS_PER_SEC;
            printf("Time taken to execute in seconds : %f \n", duration);
        }
    

	
    MPI_Finalize();

	return 0;
}