#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<iostream>
#include<sys/time.h>
#include<time.h>
using namespace std;
#define OMPI_IMPORTS
#include "mpi.h"
#define MAX_BUFFER 100
/*
   Eric's NOTE:
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

 *Collective Communication:
 data motion:
 Bcast
 Gather
 AllGather
 Scatter
 Alltoall
 *Data aggregation:
 Reduce
 Allreduce
 Reduce_scatter
 Scan
 *Synchronization:
 Barrier

 Some tricks:
 all means all processes having the same result..

 detail function definition:
//MPI_Bcast(buffer,count,MPI_Datatype,int root,MPI_Comm);
//MPI_Reduce(sendBuff,recvBuff,count,MPI_Datatype,MPI_Op,root,MPI_Comm);

 * */
int old_main(int argc,char *argv[]){
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
	MPI_AINT;
	return 0;
}
#define Trace(m) cout<<#m"="<<(m)<<endl;
inline double f(double a){
	return 4.0/(1.0+a*a);
}
class Tic{
	//accuration in milliseconds
	private:
		static long lastTime;
		Tic(){}
	public:
		inline static long getTimeMs(){
			timeval timeStart;
			gettimeofday(&timeStart,NULL);
			long res=((long)timeStart.tv_sec)*1000+(long)timeStart.tv_usec/1000;
			return res;
		}
		static long tic(){
			//in milliseconds.
			long currentTime=getTimeMs();
			long dur=currentTime-lastTime;
			lastTime=currentTime;
			return dur;
		}
		inline static double dtic(){
			//in seconds.
			return (double)tic()/1000.0;
		}
		static void test(){
			Tic::tic();
			usleep(1234000);//sleep for 1234 milliseconds.(1.234seconds)
			cout<<Tic::dtic()<<"seconds"<<endl;
		}

};
long Tic::lastTime=0;
int main2(){
	//Tic::tic();
	Tic::test();
	Tic::tic();
	usleep(1234000);
	Trace(Tic::tic());
	int n=100000000;
	double h=1.0/(double)n;
	double sum=0.0;
	for(int i=0;i<=n;i++){
		double x=h*((double)i-0.5);
		sum+=f(x);
	}
	double pi=h*sum;
	Trace(pi);
	Trace(Tic::dtic());
	return 0;
}

#include<math.h>
double fpi(double x){
	return 4.0/(1+x*x);
}
int mainPiTestOK(int argc,char *argv[]){
	int myRank;
	int groupSize;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
	MPI_Comm_size(MPI_COMM_WORLD,&groupSize);
	long n=0;
	long i;
	double sum=0.0;
	if(myRank==1){
		n=100000;
	}
	MPI_Bcast(&n,1,MPI_LONG,1,MPI_COMM_WORLD);
	double width=1.0/(double)n;
	for(i=myRank+1;i<=n;i+=groupSize){
		sum+=fpi(width*((double)i-0.5));
	}
	double mypi,pi;
	mypi=sum*width;
	MPI_Reduce(&mypi,&pi,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	if(myRank==0||myRank==1){
		cout<<"id:"<<myRank<<" pi:"<<pi<<endl;
	}
	MPI_Finalize();
	return 0;
}
void printArr(int *arr,int size){
	cout<<"(";
	for(int i=0;i<size;i++){
		if(i!=0)cout<<',';
		cout<<arr[i];
	}
	cout<<")";
}
int old_main(int argc,char *argv[]){
	int id,idSize;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	MPI_Comm_size(MPI_COMM_WORLD,&idSize);
	//
	int sendData[4][5];
	if(id==0){
		for(int i=0;i<4;i++){
			for(int j=0;j<5;j++){
				sendData[i][j]=i*5+j;
			}
		}
	}
	int recvData[5];
	//
	MPI_Scatter(&sendData[0][0],5,MPI_INT,recvData,5,MPI_INT,0,MPI_COMM_WORLD);
	//print myself.
	if(id==0){
		cout<<"id:"<<id<<" sendData:{";
		for(int i=0;i<4;i++){
			for(int j=0;j<5;j++){
				cout<<sendData[i][j]<<",";
			}
		}
		cout<<"}"<<endl;
	}

	if(id==3){
		cout<<"id:"<<id<<" recvData:";printArr(recvData,5);cout<<endl;
	}
	//Gather.
	int *recvBuf;
	if(id==0){
		recvBuf=new int[5*idSize];
	}
	MPI_Gather(recvData,5,MPI_INT,recvBuf,5,MPI_INT,0,MPI_COMM_WORLD);
	if(id==0){
		cout<<"after:";
		printArr(recvBuf,5*idSize);
	}

	//
	MPI_Finalize();
	return 0;
}
class MPI{
	int id;
	int numberOfProcess;
	char processorName[MPI_MAX_PROCESSOR_NAME];
	public:
		MPI(){
			int nameLen;
			MPI_Init(&argc,&argv);
			MPI_Comm_size(MPI_COMM_WORLD,&numberOfProcess);
			MPI_Comm_rank(MPI_COMM_WORLD,&id);
			MPI_Get_processor_name(processorName,&nameLen);
		}
		const char *getName(){
			char name[100];
			sprintf(name,"Process("<<id<<","<<processorName<<")");
			return name;
		}
		void run(){
			cout<<getName()<<" is saying hello"<<endl;
		}
		~MPI(){
			MPI_Finalize();
		}
}
int main(int argc,char *argv[]){
	MPI mpi;
	mpi.run();
	return 0;
}
