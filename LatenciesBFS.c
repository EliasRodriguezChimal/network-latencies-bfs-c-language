// Calculation of latencies in a network.
// Elias Rodriguez Chimal

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

struct node{
	int numNode;
	float queueTime;
};

struct edge{
	int originNode;
	int endNode;
	int linkVeloc;
	int distance;
	int controlData;
	int userData;
};

typedef struct _node {
   	int value;
   	struct _node *next;
}nodeType;

typedef nodeType *pNode;
typedef nodeType *Stack;


void Push(Stack *stack, int v)
{
   pNode new;

   new = (pNode)malloc(sizeof(nodeType));
   new->value = v;

   new->next = *stack;
   *stack = new;
}

//-------------------------------------------------------------------------------------------------------------------------------

int Pop(Stack *stack)
{
   	pNode node;
   	int v;

   	node = *stack;
   	if(!node){
		 return 0;
	}
   	*stack = node->next;

   	v = node->value;

   	free(node);
   	return v;
}



bool Empty(Stack *stack)
{
	pNode node;
	int v;

	node = *stack;
	if(node==NULL){
		return true;
	}
}


//--------------------------------------------------------------------------------------------------------------------------------

void recordVector(int path[], FILE *file)
{
	fwrite(path, sizeof(int), 6, file);
}
//---------------------------------------------------------------------------------------------------------------------------------

void recoverVector(int eNode, int numNodes, int edgesNumber, struct edge edges[], struct node nodes[], int oNode, float finalSize, int *totalSize, float *time)
{

	Stack stack1 = NULL;
	Stack stack2 = NULL;
	FILE *file;
	float latency=0.0f, roof=0.0f, finalLatency=0.0f;
	int substract, data, counter=0, aux=0, paths=0;
	file=fopen("Vectores.dat","rb");
	int vec[6];
	while(!feof(file)){
		fread(vec, sizeof(int), 6, file);
		paths++;
	}
	fclose(file);
	float V[paths];
	file=fopen("Vectores.dat","rb");
	paths=0;
	while(!feof(file)){
		fread(vec, sizeof(int), 6, file);
		paths++;
		printf("\nPath %d: ", paths);
		for(int f=0;vec[f]!=99 && f<6;f++){
			printf("%d ",vec[f]);
		}
		for(int f=0;vec[f]!=99 && f<6;f++){	//Calculating size of each package and their subdivisions in every link.
			for(int i=0; i<=edgesNumber; i++){	//They're 3 cass; When we're working with first link, an intermediate link and the final link.
				if((edges[i].originNode==vec[f] && edges[i].endNode==vec[f+1] && vec[f+1]!=eNode && vec[f]==oNode) || (edges[i].originNode==vec[f+1] && edges[i].endNode==vec[f] && vec[f]!=eNode && vec[f+1]==oNode)){
					latency+=(edges[i].distance/300000000.0f)+(((edges[i].userData+edges[i].controlData)*8.0f)/(edges[i].linkVeloc*1000000.0f))+(nodes[edges[i].endNode-1].queueTime);
					Push(&stack1, edges[i].userData);
					*totalSize=ceil((float)finalSize/(float)edges[i].userData);
				}else if((edges[i].originNode==vec[f] && edges[i].endNode==vec[f+1] && vec[f+1]==eNode) || (edges[i].originNode==vec[f+1] && edges[i].endNode==vec[f] && vec[f]==eNode)){
					counter=0;

					while(Empty(&stack1)!=true){
						data=Pop(&stack1);
						do{					//Storing packages in stacks to avoid losing subpackages.
							substract = (data - edges[i].userData);
							roof=((float)data/(float)edges[i].userData);
							counter+=ceil(roof);
							if((substract > 0) && (substract <= (edges[i].userData))){
								Push(&stack2, edges[i].userData);
								Push(&stack2, substract);
							}else if(substract>0 && substract > edges[i].userData){
								Push(&stack2, edges[i].userData);
							}else if(substract<0){
								Push(&stack2, data);
							}
						}while(substract > edges[i].userData);
					}
					while(Empty(&stack2)!=true){
						data=Pop(&stack2);
						Push(&stack1, data);
					}
					if(aux==counter){
						counter=1;
					}else{
						aux=counter;
					}

					//Calculating latency of each link.
					latency+=counter*((edges[i].distance/300000000.0f)+((edges[i].userData+edges[i].controlData)*8.0f)/(edges[i].linkVeloc*1000000.0f));

				}else if((edges[i].originNode==vec[f] && edges[i].endNode==vec[f+1] && vec[f+1]!=eNode && vec[f]!=oNode) || (edges[i].originNode==vec[f+1] && edges[i].endNode==vec[f] && vec[f]!=eNode && vec[f+1]!=oNode)){
					counter=0;

					while(Empty(&stack1)!=true){
						data=Pop(&stack1);
						do{
							substract = (data - edges[i].userData);
							roof=((float)data/(float)edges[i].userData);
							counter+=ceil(roof);
							if((substract > 0) && (substract <= (edges[i].userData))){
								Push(&stack2, edges[i].userData);
								Push(&stack2, substract);
							}else if(substract>0 && substract > edges[i].userData){
								Push(&stack2, edges[i].userData);
							}else if(substract<0){
								Push(&stack2, data);
							}
						}while(substract > edges[i].userData);
					}
					while(Empty(&stack2)!=true){
						data=Pop(&stack2);
						Push(&stack1, data);
					}
					if(aux==counter){
						counter=1;
					}else{
						aux=counter;
					}

					latency+=counter*((edges[i].distance/300000000.0f)+((edges[i].userData+edges[i].controlData)*8.0f)/(edges[i].linkVeloc*1000000.0f)+(nodes[edges[i].endNode-1].queueTime));
				}
			}
		}
		for(int i=0; Empty(&stack1)!=true; i++){
			Pop(&stack1);
		}
		for(int i=0; Empty(&stack2)!=true; i++){
			Pop(&stack2);
		}
		substract=0;
		aux=0;
		counter=0;
		V[paths-1]=latency;	//Storing the latency data in this vector.
		printf("\nTotal latency of path %d: %.10f",paths, latency);
		latency=0.0f;
	}
	for(int i=0; i<paths; i++){
		if(finalLatency==0){
			finalLatency=V[i];
		}else{
			if(finalLatency>V[i]){
				finalLatency=V[i];
			}
		}
	}
	*time=finalLatency;
	printf("\nTotal de paths: %d\n", paths);
	fclose(file);
}
//--------------------------------------------------------------------------------------------------------------------------------

void findPaths(int numN, int u, int d, bool visited[], int path[], int path_index, int ady[numN][numN], FILE *file)
{	//Recursive algorithm to find all possible paths.
	int k;
    	visited[u] = true;
    	path[path_index] = u+1;
    	path_index++;

    	if (u == d){
		for (int i = path_index; i<6; i++){
        		path[i]=99;
        	}
		recordVector(path, file);
		path_index--;
    		visited[u] = false;
	}else{
        	for (int i = 0;ady[u][i]!=99;i++){
			k=ady[u][i];
			if (visited[k-1]!=true){
                		findPaths(numN, (ady[u][i]-1), d, visited, path, path_index, ady, file);
			}
		}
		path_index--;
    		visited[u] = false;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void read(FILE *f, int numNodes, int edgesNumber, struct node nodes[], struct edge edges[], float *finalSize, int *oNode, int *eNode, char c[], char c1[], char *ptr, char s[], int counter){
	//Function to read data from text file. We introduce the name of the file with data that we want to be readed from.
	for(int i=0; i<numNodes; i++){
		fscanf(f,"%s",c);
		ptr=strtok(c,c1);
		strcpy(s,ptr);
		nodes[i].numNode=atoi(s);
		ptr=strtok(NULL,c1);
		strcpy(s,ptr);
		nodes[i].queueTime=atof(s);
	}
	for(int i=0; i<edgesNumber; i++){
		counter=0;
		fscanf(f,"%s",c);
		ptr=strtok(c,c1);
		strcpy(s,ptr);
		edges[i].originNode=atoi(s);
		while( (ptr = strtok( NULL, c1 )) != NULL ){
    			if(counter==0){
				strcpy(s,ptr);
				edges[i].endNode=atoi(s);
				counter++;
			}else if(counter==1){
				strcpy(s,ptr);
				edges[i].linkVeloc=atoi(s);
				counter++;
			}else if(counter==2){
				strcpy(s,ptr);
				edges[i].distance=atoi(s);
				counter++;
			}else if(counter==3){
				strcpy(s,ptr);
				edges[i].controlData=atoi(s);
				counter++;
			}else if(counter==4){
				strcpy(s,ptr);
				edges[i].userData=atoi(s);
				counter++;
			}
		}

	}
	fscanf(f,"%s",c);
	*finalSize=atof(c);
	fscanf(f,"%s",c);
	ptr=strtok(c,c1);
	strcpy(s,ptr);
	*oNode=atoi(s);
	ptr=strtok(NULL,c1);
	strcpy(s,ptr);
	*eNode=atoi(s);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

int main(){

	//Read data from text file.


	FILE *f,*file;
	char c[100],s[100],*ptr,fileName[100];
	char c1[2]=",";
	div_t division;
	int numNodes, edgesNumber, counter=0, oNode, eNode, u, w, aCounter, totalSize=0, h, m, seconds;
	float finalSize, totalTime=0.0f, time=0.0f, hours, minutes;

	printf("Write filename to get data: ");
	scanf("%s",&fileName);

	file=fopen("Vectores.dat","wb");
	f=fopen(fileName,"r");

	fscanf(f, "%s" ,c);
	ptr = strtok( c, c1 );	//"srtok()" split string when finds 'c1' char.
	strcpy(s,ptr);
	numNodes=atoi(s);
	ptr = strtok( NULL, c1 );
	strcpy(s,ptr);
	edgesNumber=atoi(s);		//"atoi()" function transform string 's' to int.

	struct node nodes[numNodes];
	struct edge edges[edgesNumber];

	read(f, numNodes, edgesNumber, nodes, edges, &finalSize, &oNode, &eNode, c, c1, ptr, s, counter);

	//Finding all possible paths.

	bool visited[numNodes];
    	int path[numNodes];
	int ady[numNodes][numNodes];
	int matAdy[numNodes][numNodes];

	for(int i=0; i<numNodes; i++){
		path[i]=99;
	}

	for(int i=0; i<numNodes; i++){
		for(int j=0; j<numNodes; j++){
			matAdy[i][j]=99;
		}
	}

	for(int i=0; i<edgesNumber; i++){
		matAdy[edges[i].originNode-1][edges[i].endNode-1]=1;
		matAdy[edges[i].endNode-1][edges[i].originNode-1]=1;
	}

	for(int i=0; i<numNodes; i++){
		aCounter=0;
		for(int j=0; j<numNodes; j++){
			ady[i][j]=99;
			if(matAdy[i][j]==1){
				ady[i][aCounter]=j+1;
				aCounter++;
			}
		}
	}

    	int path_index = 0;

     	for (int i = 0; i < numNodes; i++){
        	visited[i] = false;
	}

	finalSize=finalSize*1024.0f*1024.0f*1024.0f;

	u=oNode-1;
	w=eNode-1;

  	findPaths(numNodes, u, w, visited, path, path_index, ady, file);
	fclose(file);
	recoverVector(eNode, numNodes, edgesNumber, edges, nodes, oNode, finalSize, &totalSize, &time);

	totalTime=(time)*((float)totalSize);	//Total time printed in different formats.
	hours=totalTime/3600.0f;
	minutes=totalTime/60.0f;
	printf("Total time (hours): %.10f h\n", hours);
	printf("Total time (minutes): %.10f min\n", minutes);
	printf("Total time (seconds): %.10f seg\n", totalTime);

	division = div(totalTime, 3600);
	h=division.quot;
	division = div(division.rem,60);
	m=division.quot;
	seconds=division.rem;
	printf("Total time (format hours:minutes:seconds) -> %d:%d:%d\n",h,m,seconds);
}

