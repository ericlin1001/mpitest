#include<stdio.h>
#define OMPI_IMPORTS
#include "mpi.h"
#define MAX_BUFFER 100
/*
 *MPI_DOUBLE
 *
 * Source=MPI_ANY_SOURCE
 * Tag=MPI_ANY_TAG
 *
 * About:status
 * status.MPI_SOURCE, status.MPI_TAG
 * MPI_Get_count(MPI_Status status,MPI_Datatype datatype,int *count)
 *
 *non-blocking send&recv
 MPI_Isend compared with MPI_send, has a addtional out param:MPI_Request*request.
 MPI_Irecv no Status, has an out param:MPI_Recv*request.

 * */
int main(int argc,char *argv[]){
	int myid, numProcs,source;
	MPI_Status status;
	char mess[100];

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
	const int mainProcessID=0;
	const int TAG=99;
	//
	if(myid!=mainProcessID){
		//secondary process
		sprintf(mess,"Hello from process(%d).",myid);
		MPI_Send(mess,strlen(mess)+1,MPI_CHAR,mainProcessID,TAG,MPI_COMM_WORLD);
		MPI_Send(&myid,1,MPI_INT,mainProcessID,TAG,MPI_COMM_WORLD);
		printf("{hello this is process %d}\n",myid);
	}else{
		//if this is the main process
		for(source=1;source<numProcs;source++){
			int tmp=-1;
			MPI_Recv(mess,MAX_BUFFER,MPI_CHAR,source,TAG,MPI_COMM_WORLD,&status);
			MPI_Recv(&tmp,1,MPI_INT,source,TAG,MPI_COMM_WORLD,&status);
			printf("main recv id(%d):%s\n",tmp,mess);
		}
	}
	//
	
	MPI_Finalize();
return 0;
}


