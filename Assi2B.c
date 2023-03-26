#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int a[];
int my_array[1000];
int arraySize;
int main(int argc, char* argv[])
{
    clock_t start, end;
    double duration;
	int pid, np, elements_per_process, lower_bound, upper_bound;//for master
    int my_lower_bound, my_upper_bound,n_elements_recieved;//for slaves
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

        arraySize = upper_bound - lower_bound;
        
        //generate array from lower bound to upper bound
        for(int i=0;i<arraySize;i++)
        {
            a[i] = lower_bound + i;
        }

        //print array///////////////////////////////////////////////////////////////////////////////////////////
        printf("Array is:\n");
        for(int i=0;i<arraySize;i++)
        {
            printf("%d ",a[i]);
        }
        printf("\n");
        ///////////////////////////////////////////////////////////////////////////////////////////
        start = clock();
		
		int index, i;
		elements_per_process = arraySize / (np-1);

		if (np > 1) {

			if(arraySize % (np-1) == 0){//array size is divisible by no. of processes
			// distributes the portion of array to child processes to calculate their local_maximums
				for (i = 1; i < np; i++) {
					index = (i-1) * elements_per_process;
                    lower_bound = a[index];
                    //send lower bound
                    MPI_Send(&lower_bound,1,MPI_INT,i,1,MPI_COMM_WORLD);
                    //send sub array size
                    MPI_Send(&elements_per_process,1,MPI_INT,i,2,MPI_COMM_WORLD);
                }//end for

			}else{//array size is not divisible by no. of processes
				for (i = 1; i < np-1; i++) {
					index = (i-1) * elements_per_process;
                    lower_bound = a[index];
                    //send lower bound
                    MPI_Send(&lower_bound,1,MPI_INT,i,1,MPI_COMM_WORLD);
                    //send sub array size
                    MPI_Send(&elements_per_process,1,MPI_INT,i,2,MPI_COMM_WORLD);

				}//end for

				// last process handles remaining elements
				index = (i-1) * elements_per_process;
				int remaining_elements = arraySize - (i*elements_per_process);
				int Last_process_portion=elements_per_process+remaining_elements;
                lower_bound = a[index];
                //send lower bound
                MPI_Send(&lower_bound,1,MPI_INT,i,1,MPI_COMM_WORLD);
                //send sub array size
                MPI_Send(&Last_process_portion,1,MPI_INT,i,2,MPI_COMM_WORLD);
				

			}//end else
		}//end if

        //collect results from slave processes
        int total_count = 0;
        int finalArrayOfPrimeNumbers[arraySize];
        int finalArrayCounter = 0;
        for(int i=1;i<np;i++)
        {
            int count;
            int lower,upper;
            MPI_Recv(&count,1,MPI_INT,i,3,MPI_COMM_WORLD,&status);
            MPI_Recv(&lower,1,MPI_INT,i,4,MPI_COMM_WORLD,&status);
            MPI_Recv(&upper,1,MPI_INT,i,5,MPI_COMM_WORLD,&status);

            //recieve sub array of prime numbers from slave
            int subArrayOfPrimeNumbers[count];
            for (int j = 0; j < count; j++)
            {
                MPI_Recv(&subArrayOfPrimeNumbers[j],1,MPI_INT,i,6,MPI_COMM_WORLD,&status);
               finalArrayOfPrimeNumbers[finalArrayCounter] = subArrayOfPrimeNumbers[j];
               finalArrayCounter++;
            }
            
            printf("P%d: calculated partial count of prime numbers from %d to %d -> %d ",i,lower,upper,count);
            printf("{");
            for (int j = 0; j < count; j++)
            {
                if (j == count-1)
                {
                    printf("%d",subArrayOfPrimeNumbers[j]);
                    printf("}");
                }
                else printf("%d ,",subArrayOfPrimeNumbers[j]);

            }
            printf("\n");

            total_count += count;
        }
        printf("Master announce that total count of prime numbers is %d ",total_count);
        /* Get the time taken by program to execute in seconds */
        end = clock();
        duration = ((double)end - start)/CLOCKS_PER_SEC;

        printf("{");
        for (int j = 0; j < finalArrayCounter; j++)
        {
            if (j == finalArrayCounter-1)
            {
                printf("%d",finalArrayOfPrimeNumbers[j]);
                printf("}");
            }
            else printf("%d ,",finalArrayOfPrimeNumbers[j]);

        }
        printf("\n");

        printf("Time taken to execute in seconds : %f \n", duration);

	
	}

	//----------------------slave processes---------------------------------
	else {
        //recieve lower bound
        MPI_Recv(&my_lower_bound,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
        //recieve sub array size
        MPI_Recv(&n_elements_recieved,1,MPI_INT,0,2,MPI_COMM_WORLD,&status);

        my_upper_bound = my_lower_bound + n_elements_recieved - 1;
        int subArrayOfPrimeNumbers[n_elements_recieved];

        //generate sub array
        for(int i=0;i<n_elements_recieved;i++)
        {
            my_array[i] = my_lower_bound + i;
        }

        //get count of prime numbers in sub array (with excluding 1)
        int count = 0;
        for(int i=0;i<n_elements_recieved;i++)
        {
            int flag = 0;
            for(int j=2;j<my_array[i];j++)
            {
                if(my_array[i] % j == 0)
                {
                    flag = 1;
                    break;
                }
            }
            if(flag == 0 && my_array[i] != 1)
            {
                subArrayOfPrimeNumbers[count] = my_array[i];
                count++;
            }
        }


        //send count to master
        MPI_Send(&count,1,MPI_INT,0,3,MPI_COMM_WORLD);
        //send my_lower and my_upper to master
        MPI_Send(&my_lower_bound,1,MPI_INT,0,4,MPI_COMM_WORLD);
        MPI_Send(&my_upper_bound,1,MPI_INT,0,5,MPI_COMM_WORLD);

        //send sub array of prime numbers to master
        for (int i = 0; i < count; i++)
        {
            MPI_Send(&subArrayOfPrimeNumbers[i],1,MPI_INT,0,6,MPI_COMM_WORLD);
        }

	}

    MPI_Finalize();
	return 0;
}
