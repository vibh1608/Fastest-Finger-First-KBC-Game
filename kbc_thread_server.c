#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#define maxClient 2

char ansClient[maxClient];
double timeClient[maxClient];
int c_client[maxClient];
int correct_answer_giver[maxClient];

void *thread(void *param)
{	
	char ans;
	int index = atoi(param);
	recv(c_client[index], &ans, sizeof(ans),0);
	ansClient[index] = ans;
	timeClient[index]=(double)(clock());
	pthread_exit(0);
}

int main()
{
	//default global array initialisation
	for(int j=0; j<maxClient; j++)
	{
		ansClient[j]='X';
		timeClient[j]=0.0;
		correct_answer_giver[j]=0;
	}

	//Local variable
	clock_t start;

	// Networking code
	char buff[250]="How many times Rohit Sharma made double century in ODI's?\n(A) 1\n(B) 0\n(C) 3\n(D) 5\n";
	char correct_ans='C';

	int s_sock;
	socklen_t addr_size;
	s_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server, other;
	memset(&server,0,sizeof(server));
	memset(&other,0,sizeof(other));
	server.sin_family = AF_INET;
	server.sin_port = htons(9003);
	server.sin_addr.s_addr = INADDR_ANY;
	socklen_t add;
	
	// Binding of socket
	if(bind(s_sock,(struct sockaddr*)&server,sizeof(server))==-1)
	{
		printf("[-] Bind error\n");
		return 0;
	}
	
	printf("[+] Bind with port:%d",9000);
	
	// Number of server request queues
	if(listen(s_sock, maxClient) == 0)
	{
		printf("[+] listening...\n");
		
		for(int j=0; j<maxClient; j++)
			{
				c_client[j] = accept( s_sock, (struct sockaddr*)&other, &addr_size);
				if(c_client[j] == -1)
				{
					printf("Connection error\n");
					exit(1);
				}
				printf("Client is connected to server with ip:%s, port: %d\n",inet_ntoa(other.sin_addr),ntohs(other.sin_port));
			}

		//Broadcasting question 
		for(int j=0; j<maxClient; j++)
			send(c_client[j], buff, sizeof(buff),0);
		
		//Timer starts now
		start = clock();

		//Initiliasing the time to start
		// for(int j=0; j<maxClient; j++)
		// 	timeClient[j]= (double)start;
		
		//Thread creation
		pthread_t pid[maxClient];
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		for(int i=0 ; i<maxClient; i++)
			pthread_create(&pid[i],&attr,thread,(void *)&i);

		//Timer for 30 second
		while(1)
		{
			clock_t temp = clock();
			double seconds = (double)(temp-start)/CLOCKS_PER_SEC;
			if(seconds>=30.0)
			break;
		}

		//Killing of threads after the timer
		for(int i=0; i<maxClient; i++)
			pthread_cancel(pid[i]);

		//Processing of winner
		double time_min= timeClient[0];
		int index=0;
		for (int j=0; j <maxClient ;++j)
		{
			if(ansClient[j]==correct_ans)
				correct_answer_giver[j]=1;
			timeClient[j]= timeClient[j]-(double)start;
			if(timeClient[j] < time_min)
			{
				time_min = timeClient[j];
				index = j;
			}
		}

		//Multicasting of result
		char winner[200]="You are winner!";
		char loser[200]="You lost!";
		// chat time[100]="";
		for (int j=0; j < maxClient; ++j)
		{
		        char msg1[20];
		        char msg2[100]=" You gave answer in :";
		        sprintf(msg1,"%lf",timeClient[j]);
		        strcat(msg2,msg1);
		        strcat(msg2," seconds\n");		        
			if(j == index)
			{
			   strcat(winner,msg2);   
			   send(c_client[j], winner, sizeof(winner),0);
			}	
			else
			{
			   strcat(loser,msg2);
			   send(c_client[j], loser, sizeof(loser),0);
			}	
		}
	}

for(int j=0; j<maxClient; j++)
	close(c_client[j]);
return 0;
}	
