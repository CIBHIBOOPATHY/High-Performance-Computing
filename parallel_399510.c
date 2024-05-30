#include<stdio.h>
#include<mpi.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

//number of cities
#define n 17
int bound;

//initial matrices declaration
int res[n-1][n+1]; //result array
int init_route[n-1][n+1]; //initial route array
int temp_reach[n-1][n]; // visited cities array

//hardcoded input distances
int c[17][17]={
    {0,54,76,24,47,49,55,44,57,18,55,82,51,29,38,56,34},
    {54,0,30,51,30,44,18,20,53,62,41,36,32,58,64,3,20},
    {76,30,0,64,33,43,47,32,47,88,35,53,30,69,71,27,49},
    {24,51,64,0,31,27,60,33,33,42,34,86,34,7,16,52,38},
    {47,30,33,31,0,13,46,11,22,62,12,67,4,36,38,29,33},
    {49,44,43,27,13,0,59,24,10,66,8,81,13,29,29,43,44},
    {55,18,47,60,46,59,0,35,69,58,58,27,49,67,75,21,21},
    {44,20,32,33,11,24,35,0,33,57,23,57,14,40,44,19,23},
    {57,53,47,33,22,10,69,33,0,74,12,89,21,33,30,51,54},
    {18,62,88,42,62,66,58,57,74,0,71,84,67,46,55,65,41},
    {55,41,35,34,12,8,58,23,12,71,0,77,9,37,37,39,45},
    {82,36,53,86,67,81,27,57,89,84,77,0,68,94,100,38,49},
    {51,32,30,34,4,13,49,14,21,67,9,68,0,39,40,30,36},
    {29,58,69,7,36,29,67,40,33,46,37,94,39,0,9,59,46},
    {38,64,71,16,38,29,75,44,30,55,37,100,40,9,0,64,54},
    {56,3,27,52,29,43,21,19,51,65,39,38,30,59,64,0,23},
    {34,20,49,38,33,44,21,23,54,41,45,49,36,46,54,23,0}
};

/*
 * Function: read_distance
 * ------------------
 *   Reads the input from file containing the distance between cities.
 *
 *   file_path: Path of file
 *
 *   returns: None
 */
void read_distance(char* file_path)
{
    FILE* file = fopen(file_path, "r");
    char line[512];
    int m;
	fscanf(file, "%d", &m);

    int row=1;
    int col=0;

    c[row-1][col]=0;

    while (fgets(line, sizeof(line), file))
    {
        col=0;
        char *val = strtok(line, " ");
        while (val != NULL)
        {
            if(atoi(val)>0)
            {
                c[row-1][col] = atoi(val);
                c[col][row-1] = atoi(val);
            }
            val = strtok(NULL, " ");
            col++;
        }
        c[row][col]=0;
        row++;
        }
	fclose(file);
}

/*
 * Function: print_solution
 * ------------------
 *   Prints the final result obtained from worker nodes after comparing least bounds
 *
 *   returns: None
 */
void print_solution()
{
    int length=res[0][n];
    int tour=0;

    //Comparing the least bounds
    for(int i=0;i<n-1;i++)
    {
        if(res[i][n]<length)
        {
            length=res[i][n];
            tour=i;
        }
    }
	printf("\nOptimal Path:\n");
    for(int i=0;i<n;i++)
    {
        res[tour][i]+=1;
        printf("%d\t",res[tour][i]);
    }
	printf("\nLeast Bound of Optimal tour:%d\n",length);
    printf("\n");
}
//setting the start path
int start_path=0;


/*
 * Function: initialize
 * ------------------
 *   Initializes necessary variables before sending the work packets for
 *   Branch and bound algorithm
 *
 *   returns: None
 */
void initialize()
{

    int subtree=1; //route tree level
    int low_bound;

	for(int i=0;i<n-1;i++)
    {
        for(int j=0;j<n;j++)
        {
            init_route[i][j]=0;
        }
        init_route[i][subtree-1]=start_path;
    }

	for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            temp_reach[i][j]=n+1;
        }
        temp_reach[i][init_route[i][subtree-1]]=1;
    }

	subtree=2;

    int count=0;
    //Initializing route arrays for multiple subtrees
    for(int i=0;i<n;i++)
    {
        low_bound=0;
        if(temp_reach[count][i]==n+1)
        {
            init_route[count][subtree-1]=i;
            temp_reach[count][i]=1;
            low_bound+= c[start_path][i];

            init_route[count][n]=low_bound;

            count+=1;
        }
    }
	//Initial bound calculation
    for(int i=0;i<n-1;i++)
    {
        bound+=c[i][i+1];
    }

    for(int i=0;i<n-1;i++)
    {
        res[i][n]=bound;
    }
}
/*
 * Function: WSP
 * ------------------
 *   Uses branch and bound algorithm to find the least bound of the route tree
 *
 *   address: address of worker packet data location
 *   new_bound: bound calculated when traversing a subtree
 *   subtree: level of route tree that is to be travelled
 *
 *   returns: None
 */
void WSP(int address,int new_bound, int subtree)
{
    if(subtree==n)
    {
        //condition to update bound
        if(new_bound<res[address][n])
        {
            res[address][n]=new_bound;
            //updating the optimal path
            for(int i=0;i<n;i++)
            {
                res[address][i]=init_route[address][i];
            }
        }
        return;
    }
    for(int i=0;i<n;i++)
    {
        if(temp_reach[address][i]==n+1 && c[init_route[address][subtree-1]][i]!=0)
        {

            int temp_bound=new_bound;
            new_bound += c[init_route[address][subtree-1]][i];

            //condition to traverse into the subtree further
            if(new_bound<res[address][n])
            {
                init_route[address][subtree]=i;
                temp_reach[address][i]=1;
                WSP(address, new_bound,subtree+1);
            }

            //pruning the subtree route

            new_bound = temp_bound;

            for(int i=0;i<n;i++)
            {
                temp_reach[address][i]=n+1;
            }
            for(int i=0;i<subtree;i++)
            {
                temp_reach[address][init_route[address][i]]=1;
            }
        }
    }
}
int main(int argc,char *argv[])
{
    int rank,size;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;
    MPI_Request request;

    //code for reading input using -i; It is turned into comments as segmentation fault occurs
    //char* file_path = argv[2];
    //read_distance(file_path);



    start_time=MPI_Wtime();
    //broadcasting input distance matrix to all nodes
    MPI_Bcast(&c, n*n, MPI_INT, 0, MPI_COMM_WORLD);

    int work_size;
    int lower_index;
    int upper_index;

    //MASTER NODE
    if(rank==0)
    {
        initialize();

        //problem decomposition

        int work_size=(n-1)/(size-1);
        int rem1=(n-1)%(size-1);
        int low[size-1];
        int upper[size-1];
        int rem[size-1];
        for(int i=0;i<size-1;i++)
        {
            if(i<rem1)
            {
                rem[i]=1;
            }
            else
            {
                rem[i]=0;
            }
        }
        // load balancing while decomposing the problem
        for(int i=0;i<size-1;i++)
        {
            if(i==0)
            {
                low[i]=i*work_size;
                upper[i]=low[i]+work_size+rem[i];
            }
            else
            {
                low[i]=upper[i-1];
                upper[i]=low[i]+work_size+rem[i];
            }
        }
        //sending work packets to worker nodes
        for(int i=0;i<size-1;i++)
        {
            lower_index=low[i];
            upper_index=upper[i];

            MPI_Isend(&lower_index, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD, &request);
            MPI_Isend(&upper_index, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD, &request);
            MPI_Isend(&res[lower_index][0], (upper_index - lower_index) * (n+1), MPI_INT, i+1, 2, MPI_COMM_WORLD, &request);
            MPI_Isend(&init_route[lower_index][0], (upper_index - lower_index) * (n+1), MPI_INT, i+1, 3, MPI_COMM_WORLD, &request);
            MPI_Isend(&temp_reach[lower_index][0], (upper_index - lower_index) * (n+1), MPI_INT, i+1, 4, MPI_COMM_WORLD, &request);
        }
    }

    // WORKER NODE
    if(rank>0)
    {
        //receive work packets from master
        MPI_Recv(&lower_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&upper_index, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&res[lower_index][0], (upper_index - lower_index) * (n+1), MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&init_route[lower_index][0], (upper_index - lower_index) * (n+1), MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
        MPI_Recv(&temp_reach[lower_index][0], (upper_index - lower_index) * (n+1), MPI_INT, 0, 4, MPI_COMM_WORLD, &status);

        //branch and bound algorithm function is called upon
        for(int i=lower_index;i<upper_index;i++)
        {
            WSP(i,init_route[i][n],2);
        }

        //sending results to master
        MPI_Isend(&lower_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Isend(&upper_index, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &request);
        MPI_Isend(&res[lower_index][0], (upper_index - lower_index) * (n+1), MPI_INT, 0, 2, MPI_COMM_WORLD, &request);
    }
    //MASTER NODE
    if (rank==0)
    {
        //receives the results from worker nodes
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(&lower_index, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&upper_index, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&res[lower_index][0], (upper_index - lower_index) * (n+1), MPI_INT, i, 2, MPI_COMM_WORLD, &status);
        }
        end_time=MPI_Wtime();

        //print results
        printf("\nComputational Time = %f\n", end_time - start_time);

        for(int i=0;i<n-1;i++)
        {
            for(int j=0;j<n+1;j++)
            {
                printf("%d\t",res[i][j]);
            }
            printf("\n");
        }
        print_solution();
    }
	MPI_Finalize();
    return 0;
}
