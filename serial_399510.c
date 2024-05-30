#include<time.h>
#include<stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

//number of cities declared
#define n 17

//Input matrix declaration
int d[n][n];

//Hardcoded input matrix
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

void WSP();

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

//Initialising necessary variables

int bound=0; //initial bound
int final_path[n]; // final optimal route
int visited[n]; // visited cities list

//starting city variable
int start_path;


/*
 * Function: wsp_init
 * ------------------
 *   Initialises the necessary values before calling the recursive branch and bound function
 *
 *   returns: None
 */
void wsp_init()
{
    int temp_path[n]; //temporary route

    //Initialise initial bound and optimal route
    for(int i=0;i<n-1;i++)
    {
        bound+=c[i][i+1];
        final_path[i]=i;
    }

	final_path[n-1]=n-1;


    printf("Init bound:%d\n",bound);
    printf("Init Best route:");
    for(int i=0;i<n;i++)
    {
        printf("%d\t",final_path[i]);
    }
	printf("\n");

    for(int i=0;i<n;i++)
    {
        visited[i]=n+1;
        temp_path[i]=-1;
    }
	temp_path[0]=start_path;
    visited[start_path]=1;
    int new_bound=0;

    //Call the recursive function
    WSP(temp_path,new_bound,1);
}


/*
 * Function: WSP
 * ------------------
 *   Uses branch and bound algorithm to find the least bound of the route tree
 *
 *   temp_path[]: Temporary route
 *   new_bound: bound calculated when traversing a subtree
 *   subtree: level of route tree that is to be travelled
 *
 *   returns: None
 */
void WSP(int temp_path[],int new_bound, int subtree)
{
    if(subtree==n)
    {
        //condition to update the initial bound
        if(new_bound<bound)
        {
            bound=new_bound;
            // replacing the old optimal path with the new optimal path
            for(int i=0;i<n;i++)
            {
                final_path[i]=temp_path[i];
            }
        }
        return;
    }

	for(int i=0;i<n;i++)
    {
        if(visited[i]==n+1 && c[temp_path[subtree-1]][i]!=0)
        {

            int temp_bound=new_bound;
            new_bound += c[temp_path[subtree-1]][i];

            //condition to proceed further in the subtree
            if(new_bound<bound)
            {
                temp_path[subtree]=i;
                visited[i]=1;
                //recursive calling of WSP function
                WSP(temp_path,new_bound,subtree+1);
            }

            //pruning the node if the condition is not satisfied
            new_bound = temp_bound;

            for(int i=0;i<n;i++)
            {
                visited[i]=n+1;
            }
            for(int i=0;i<=subtree-1;i++)
            {
                visited[temp_path[i]]=1;
            }
        }
    }
}


int main(int argc,char *argv[])
{
    clock_t start_time,end_time;
    double comp_time;

    //code for reading input using -i; It is turned into comments as segmentation fault occurs
    //char* file_path = argv[2];
    //read_distance(file_path);

    printf("\nEnter your starting city:");
    scanf("%d",&start_path);
    start_path-=1;

    start_time=clock();

    //Computation starts from calling wsp_init
    wsp_init();

    end_time=clock();

    comp_time = ((double) (end_time-start_time))/ CLOCKS_PER_SEC;

    //printing the results
    printf("\nComputational time:%f \n",comp_time);

    printf("\nOptimal Route found is:\n");
    for(int i=0;i<n;i++)
    {
        printf("%d\t", final_path[i]+1);
    }
	printf("\nRoute length:%d\n",bound);
    return 0;
}



