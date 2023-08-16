#include "http_server.hh"
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <queue>
#define Max_limit 10000
pthread_t t_arr[20];
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wcond=PTHREAD_COND_INITIALIZER;
pthread_cond_t mcond;
std::queue<int> que;
void handle(int a){
  exit(0);
}

vector<string> split(const string &s, char delim)
{
  vector<string> elems;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim))
  {
    if (!item.empty())
      elems.push_back(item);
  }
  return elems;
}

HTTP_Request::HTTP_Request(string request)
{
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');
  this->HTTP_version = "1.0";
  /*
   TODO : extract the request method and URL from first_line here
  */
  this->method = first_line[0];
  this->url = first_line[1];
  if (this->method != "GET")
  {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req)
{
  HTTP_Request *request = new HTTP_Request(req);
  HTTP_Response *response = new HTTP_Response();
  string url = string("html_files") + request->url;
  response->HTTP_version = "1.0";
  //string db1;
  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";
    string body;
    if (S_ISDIR(sb.st_mode))
    {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
      url = url + string("/index.html");
    }
    /*
    TODO : open the file and read its contents
    */
    ifstream ifs(url);
    string c((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    body = c;
    /*
    TODO : set the remaining fields of response appropriately
    */
    response->body = body;
    response->content_length = to_string(body.size());
  }
  else
  {
    response->status_code = "404";
    response->status_text = "Not Found";
    response->content_type = "text/html";
    response->body = "404";
    response->content_length = "6";
  }
  delete request;
  return response;
  
}
void error(string msg)
{
  perror(msg.c_str());
  exit(1);
}
void *myThreadFun(void *id)
{
  int sockfd, portno, newfd;
  //int *newsockfd = (int *)id;
  //socklen_t clilen;
  char buffer[256];
  //struct sockaddr_in serv_addr, cli_addr;
  int n;
  while (1)
  {
    pthread_mutex_lock(&mutex);
    while (que.empty())
    {
      pthread_cond_wait(&wcond, &mutex);
    }

    newfd = que.front();
    que.pop();
    pthread_cond_signal(&mcond);
    pthread_mutex_unlock(&mutex);

    bzero(buffer, 256);
    n = read(newfd, buffer, 256);
    if (n <= 0)
    {
      //printf("socket closed\n");
      continue;
      // break;
    }
     //printf("Here is the message: %s", buffer);
    //string buffered = string(buffer);

    HTTP_Response *response_struct = handle_request(buffer);
    string b = response_struct->body;
    string s= response_struct->status_code;
    string fin="";
    if(s[0]=='2' && s[1]=='0' && s[2]=='0'){
       fin="HTTP/1.0 200 OK \nContent-Type: text/html\n"+b;
    }
    else{
      fin="HTTP/1.0 404 Not Found \nContent-Type: text/html\n"+b;
    }
    //string final_res = response_struct->get_string(b);

    //int x = fin.length();
    //char arr[x + 1];
    
    //strcpy(arr, fin.c_str());
    n = write(newfd, fin.c_str(), strlen(fin.c_str()));

    if (n < 0)
      error("ERROR writing to socket");
    
    delete response_struct;
    close(newfd);
    //  sleep(10);
  }
  //delete response_struct;
  return NULL;
}
int main(int argc, char *argv[])
{
  //pthread_mutex_init(&mutex, NULL);
  //pthread_cond_init(&wcond, NULL);
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;

  if (argc < 2)
  {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }
  signal(SIGINT,handle);
  /* create socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  // fill in port number to listen on. IP address can be anything (INADDR_ANY)
  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

  listen(sockfd, 2000);
  clilen = sizeof(cli_addr);
   //pthread_t t_arr[20];
  for (int i = 0; i < 20; i++)
  {
    pthread_create(&t_arr[i], NULL, myThreadFun, NULL);
  }

  /* accept a new request, create a newsockfd */
  while (1)
  {
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    while (que.size() >= Max_limit)
    {
      pthread_cond_wait(&mcond,&mutex);
    }
    pthread_mutex_lock(&mutex);
    que.push(newsockfd);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&wcond);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&wcond);
  close(newsockfd);
  return 0;
}
