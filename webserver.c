/*
	Simple web server
	Nedim Babić
	Fakultet elektrotehnike
*/

//Kompajlirati sa: gcc webserver.c -lpthread -o webserver
//Onda pokrenuti sa: ./webserver

/*
Sa web browsera na adresu http://localhost:8080
HTML kod(stranica) je embedovana u C kod, 
pa je dovoljno samo ovu adresu bez /index.html ili sl.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>



static void *doit(void *arg);

unsigned long fsize(char* file)

{

    FILE *f = fopen(file, "r");

    fseek(f, 0, SEEK_END);

    unsigned long len = (unsigned long)ftell(f);

    fclose(f);

    return len;

}






static void *doit(void *arg)
{  
   
   char webpage[] = 

"HTTP/1.1 200 Ok\r\n"

"Content-Type: text/html; charset=UTF-8\r\n\r\n"

"<!DOCTYPE html>\r\n"

"<html><head><title>Web server u C-u</title>\r\n"

"<style>body { background-color: #A9D0F5 }</style></head>\r\n"

"<body><center><h1>Pozdrav!</h1><br>\r\n"

"<img src=\"icon.jpeg\"/></center></body></html>\r\n";

    int fdimg;
    char buf[2048];
   
	pthread_detach(pthread_self());
	memset(buf, 0, 2048);
	read((intptr_t)arg, buf, 2047);

                        

    if(!strncmp(buf, "GET /favicon.ico", 16))

   {

    fdimg = open("favicon.ico", O_RDONLY);

    sendfile((intptr_t)arg, fdimg, NULL, fsize("favicon.ico"));
    close(fdimg);
	close((intptr_t)arg);
    pthread_exit(0);
    return NULL;
   }
                        

    else if(!strncmp(buf, "GET /icon.jpeg", 14)) {

    fdimg = open("icon.jpeg", O_RDONLY);

    sendfile((intptr_t)arg, fdimg, NULL, fsize("icon.jpeg"));
    close(fdimg);
	close((intptr_t)arg);
	pthread_exit(0);
    return NULL;



                       

   } else  {

    write((intptr_t)arg, webpage, sizeof(webpage) -1);
           }
   close((intptr_t)arg);
   pthread_exit(0);
   return NULL;
}


//Main function



int main(int argc, char *argv[]) 

 {

 struct sockaddr_in server_addr, client_addr; 

 socklen_t sin_len =sizeof(client_addr);

 int fd_server, fd_client;


 int on = 1;
 pid_t pid;
 pthread_t th;

        

       

 fd_server = socket(AF_INET, SOCK_STREAM, 0); 

        

 if (fd_server < 0) {

  perror("socket");

  exit(1); 

 }

        

     

 setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

 

        

 server_addr.sin_family = AF_INET;

 server_addr.sin_addr.s_addr = INADDR_ANY;

 server_addr.sin_port = htons(8080);

 

 if (bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)

 {

  perror("bind");

  close(fd_server);

  exit(1);

 }



 if (listen(fd_server, 10) == -1)

 {

  perror("listen");

  close(fd_server);

  exit(1);

 } 

 

       

 while(1) {
      

  fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);

  if (fd_client == 1)

  {

   perror("Connection failed...\n");

   continue;

  } 

  printf("Connection established.\n");



                //Thread creating

  pthread_create(&th,NULL,&doit,(void*)(intptr_t) fd_client);


   

  }
 
return 0;

}
