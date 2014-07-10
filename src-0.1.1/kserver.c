/*

K Web Server  0.1.1

By Ark 2014.7.10

*/

#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#define PORT 80
#define BUFFER 5
#define DEFAULT_INDEX_PAGE "index.html"
void app_exit ();
int sock_fd;

typedef struct http_request_param
{
	char *method;
	char *path;
	char *http_version;
} http_request;

int split (char *token, char *buffer, char **output, int max_size)
{
	char *tok = NULL;
	int count = 0;

	tok = strtok (buffer, token);

	while (tok != NULL)
	{
		if (count > max_size - 1)
			break;
		output[count] = tok;
		count++;
		tok = strtok (NULL, token);
	}
	return count;
}

void not_found_404 (int send_to)
{
	send_msg (send_to, "<h3>Not Found 404</h3>");
}

/*Send Message*/
int send_msg (int fd, char *msg)
{
	int len = strlen (msg);
	if (len = send (fd, msg, len, 0) == -1)
	{
		perror ("send_msg");
	}
	return len;
}

/* Send Header Info
 * Parameters: int send_to 
*              char* content_type*/
void send_header (int send_to, char *content_type)
{

	char *head = "HTTP/1.0 200 OK\r\n";
	int len = strlen (head);
	if (send (send_to, head, len, 0) == -1)
	{
		printf ("send header error");
		return;
	}
	if (content_type)
	{
		char temp_1[30] = "Content-type: ";
		strcat (temp_1, content_type);
		strcat (temp_1, "\r\n");
		send_msg (send_to, temp_1);
	}

}

void send_data (int send_to, char *content_type, char *filepath)
{
	if (strcmp (filepath, "./") == 0)
	{
		strcat (filepath, DEFAULT_INDEX_PAGE);
	}
	FILE *file;
	file = fopen (filepath, "r");
	if (file != NULL)
	{
		send_header (send_to, content_type);
		send (send_to, "\r\n", 2, 0);

		char buf[1024];

		fgets (buf, sizeof (buf), file);
		while (!feof (file))
		{
			send (send_to, buf, strlen (buf), 0);
			fgets (buf, sizeof (buf), file);
		}

	}
	else
	{
		send_header (send_to, content_type);
		send (send_to, "\r\n", 2, 0);
		not_found_404 (send_to);
	}
	if (file != NULL)
		fclose (file);

}

int get_line (int sock, char *buf, int size)
{
	int i = 0;
	char c = '\0';
	int n;

	while ((i < size - 1) && (c != '\n'))
	{
		n = recv (sock, &c, 1, 0);
		if (n > 0)
		{
			if (c == '\r')
			{
				n = recv (sock, &c, 1, MSG_PEEK);
				if ((n > 0) && (c == '\n'))
					recv (sock, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}
		else
			c = '\n';
	}
	buf[i] = '\0';

	return (i);
}

void send_cgi (int send_to, char *content_type)
{
//to implement
}

void process_php (char *path)
{
//to implement  
}

struct http_request_param *get_client_request (int new_fd)
{
	struct http_request_param *hrp = malloc (sizeof (struct http_request_param));
	char buf[1024];
	char parsebuf[1024];
	char *lines[10];
	int numchars = 1;

	buf[0] = 'A';
	buf[1] = '\0';

	numchars = get_line (new_fd, buf, sizeof (buf));
	if (numchars > 0)
	{
		memcpy (parsebuf, buf, sizeof (buf));
		split (" ", parsebuf, lines, numchars);
		hrp->method = lines[0];
		hrp->path = lines[1];
		hrp->http_version = lines[2];

		char *ext = memchr (hrp->path, '.', sizeof (hrp->path));
		if (ext == NULL)
		{
			ext = ".html";
		}


	}

	while ((numchars > 0) && strcmp ("\n", buf))	
	{
		numchars = get_line (new_fd, buf, sizeof (buf));
		//printf("Browser Request: %s\n",buf);
	}

	return hrp;
}

void *worker_thread (void *arg)
{

	int new_fd = *((int *) arg);
	struct http_request_param *hrp;
	hrp = get_client_request (new_fd);
	char path[512];

	sprintf (path, ".%s", hrp->path);
	//printf("File Path:%s \n",path);
	send_data (new_fd, "text/html", path);

	close (new_fd);
	int reval = 0;
	pthread_exit (&reval);
}

void init (int *init_sock_fd)
{
	int bind_fd, listen_fd;

	struct sockaddr_in server_addr;

	sock_fd = socket (PF_INET, SOCK_STREAM, 0);

	if (sock_fd == -1)
	{
		perror ("socket");
		exit (1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	bzero (&(server_addr.sin_zero), 8);

	bind_fd = bind (sock_fd, (struct sockaddr *) &server_addr, sizeof (struct sockaddr));

	if (bind_fd == -1)
	{
		perror ("bind");
		exit (1);
	}

	listen_fd = listen (sock_fd, 5);
	if (listen_fd == -1)
	{
		perror ("listen");
		exit (1);
	}
	else
	{
		int opt = SO_REUSEADDR;
		setsockopt (listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));
		*init_sock_fd = sock_fd;
	}

}

int main ()
{

	int sock_fd, new_fd;

	int sin_size;

	struct sockaddr_in client_addr;

	signal (SIGINT, app_exit);
	init (&sock_fd);
	int count = 0;
	while (1)
	{

		sin_size = sizeof (struct sockaddr_in);
		new_fd = accept (sock_fd, (struct sockaddr *) &client_addr, &sin_size);
		count++;
		printf ("Total : %d \n", count);
		if (new_fd == -1)
		{
			perror ("accept");
			continue;

		}

		printf ("Server:got connection from %s\n", inet_ntoa (client_addr.sin_addr));
		pthread_t worker_t;
		int err;
		void *tret;
		err = pthread_create (&worker_t, NULL, worker_thread, &new_fd);
		if (err != 0)
		{
			printf ("pthread_create error:%s\n", strerror (err));
			exit (-1);
		}

	}

	while (waitpid (-1, NULL, 0) > 0) ;
	close (sock_fd);
	return 0;
}

void app_exit ()
{
	signal (SIGINT, SIG_DFL);
	close (sock_fd);
	printf ("\nExit,Thank You For Using!\n");
	exit (0);
}
