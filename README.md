## Assignment Description:
Write a parallel C program to count the prime numbers within an input range using the 
following two methods:
- MPI_Bcast and MPI_Reduce ONLY
- MPI_Send and MPI_Recv ONLY

then compare the execution times of both programs

## Screenshots of the source code output:

### MPI_Bcast and MPI_Reduce ONLY (Assi2A.c)
![Capture1(Bcast Reduce)](https://user-images.githubusercontent.com/60941223/227805498-79b476f5-b58e-447a-8f9d-2fb9f07785bb.JPG)

###  MPI_Send and MPI_RecvONLY (Assi2B.c)
Count of numbers is divisible by number of processes (15/3 = 5) so each process takes 5 numbers
![Capture1(Send RCV)](https://user-images.githubusercontent.com/60941223/227805501-4b4ace9a-09a1-4cfd-9001-3d70f0ae7500.JPG)

Count of numbers is not divisible by number of processes (17/3) each process takes 5 numbers and master handles the remaining numbers
![Capture2(Send RCV)](https://user-images.githubusercontent.com/60941223/227805502-175ed4ec-481b-4e43-b8b3-a105d59feecc.JPG)



### Note:
  The non divisible case is handeled in both programs
