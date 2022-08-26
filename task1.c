#include <stdio.h>
#include <mpi.h>
#define NUMDATA 10000
int data[NUMDATA];
void LoadData(int data[])
{
    for (int i = 0; i < NUMDATA; i++)
    {
        data[i] = 1;
    }
}
int AddUp(int data[], int count)
{
    int sum = 0;
    for (int i = 0; i < count; i++)
    {
        sum += data[i];
    }
    return sum;
}
int main(void)
{
    int sum;
    int size;
    int rank;
    int tag = 0;
    int chunksize;
    int start;
    int result;

    chunksize = NUMDATA / size;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    chunksize = NUMDATA / size;
    if (rank == 0)
    {
        LoadData(data);
        for (int i = 1; i < size; i++)
        {
            start = i * chunksize;
            MPI_Send(&(data[start]), chunksize, MPI_INT, i, tag, MPI_COMM_WORLD);
        }
        sum = AddUp(data, chunksize);
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, tag,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += result;
        }
        printf("The total sum of data is %d\n", sum);
    }
    else
    {
        MPI_Recv(data, chunksize, MPI_INT, 0, tag, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        sum = AddUp(data, chunksize);
        MPI_Send(&sum, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}