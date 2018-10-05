 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <string.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 
 int main()
 {
 	char buff[250];
 	char ans;
 	int c_sock;
 	c_sock = socket(AF_INET, SOCK_STREAM, 0);
 	struct sockaddr_in client;
 	memset(&client,0,sizeof(client));
 	client.sin_family = AF_INET;
 	client.sin_port = htons(9003);
 	client.sin_addr.s_addr = inet_addr("127.0.0.1");
 	
 	if(connect(c_sock,(struct sockaddr*)&client,sizeof(client))==-1)
 	{
 		printf("Connection error\n");
 		return 0;
 	}

 	recv(c_sock, buff, sizeof(buff),0);
 	printf("%s",buff);

 	printf("Your answer(reply fast):");
 	scanf("%c",&ans);
	send(c_sock, &ans, sizeof(ans), 0);

	recv(c_sock, buff, sizeof(buff),0);
 	printf("%s",buff);

 	close(c_sock);
 	return 0;
 	}		
