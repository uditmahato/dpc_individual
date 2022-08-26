#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int main(int argc, char **argv[])
{
    // counting letters
    int templettercount[26] = {0};
    int tag = 0;
    int ch;

    // settting up MPI program
    int size;
    int rank;
    MPI_Init(&argc, argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        char *filename = "WarAndPeace.txt"; // this is the file name that we are using
        char *chararray;
        FILE *charfile = fopen(filename, "r"); //lets open the file
        long filelength = 0;
        int lettercount[26] = {0};
        int i = 0;
        int chunksize = 0;
        int start = 0;
        int rem = 0;
        int charcount = 0;
        // checking the file if not found then returning error
        if (charfile == NULL)
        {
            printf("Error: File not found");
            exit(0);
        }
        // finding the number of letters in the file
        while ((ch = fgetc(charfile)) != EOF)
        {
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
            {
                filelength++;
            }
        }
        rewind(charfile);

        printf("File has %ld letters\n", filelength);  // displaying  the length
        chararray = malloc(sizeof(char) * filelength); // allocating memory for the array

        // reading the file and storing it in the array
        while ((ch = fgetc(charfile)) != EOF)
        {
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
            {
                chararray[i] = ch;
                i++;
            }
        }

        // sending the array to the other processes
        chunksize = filelength / size;
        rem = filelength % size;
        charcount = filelength / size;
        printf("keeping %d leters in rank %d\n", charcount, rank);
        for (int receiver = 1; receiver < size; receiver++)
        {
            start = charcount * receiver + 1;
            if (receiver == size - 1)
            {
                charcount += rem;
            }
            printf("sending %d letters to rank %d\n", charcount, receiver);
            MPI_Send(&chararray[start], charcount, MPI_CHAR, receiver, tag, MPI_COMM_WORLD);
        }
        // calculating results with rank 0
        for (int letter = 0; letter <= chunksize; ++letter)
        {
            ch = chararray[letter];
            if (ch >= 65 && ch <= 90)
            {
                lettercount[(ch - 65)]++;
            }
            else if (ch >= 97 && ch <= 122)
            {
                lettercount[(ch - 97)]++;
            }
        }
        // receiving results from other processes
        for (int sender = 1; sender < size; sender++)
        {
            MPI_Recv(templettercount, 26, MPI_INT, sender, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int index = 0;
            for (int count = 0; count < 26; count++)
            {
                lettercount[count] += templettercount[index];
                index++;
            }
        }
        printf("The letter count is:\n");
        for (int count = 0; count < 26; count++)
        {
            printf("The Letter %c occurs %d times\n", count + 'a', lettercount[count]);
        }

        // freeing up memory
        free(chararray);
    }
    else
    {
        MPI_Status status;
        int charcount;
        char *chararray;
        MPI_Probe(0, tag, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &charcount);
        // allocating memory for the array
        chararray = malloc(charcount * sizeof(char));
        // receiving the message
        MPI_Recv(chararray, charcount, MPI_CHAR, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // calculating results
        for (int letter = 0; letter <= charcount; ++letter)
        {
            ch = chararray[letter];
            if (ch >= 65 && ch <= 90)
            {
                templettercount[ch - 65]++;
            }
            else if (ch >= 97 && ch <= 122)
            {
                templettercount[ch - 97]++;
            }
        }
        // sending back the results
        MPI_Send((&templettercount[0]), 26, MPI_INT, 0, tag, MPI_COMM_WORLD);
        // freeingup  memory
        free(chararray);
    }
    MPI_Finalize();
    return 0;
}