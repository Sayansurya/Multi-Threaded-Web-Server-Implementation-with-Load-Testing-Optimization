# Multi-Threaded-Web-Server-Implementation-with-Load-Testing-Optimization

Single-threaded echo client and server
Your first task is to write simple C/C++ client-server programs using the socket API. In this exercise, the client should send a message to the server, and the server should simply echo it back to the client. That is, the server must send the same message back to the client. The server should keep doing this as long as the client is connected to it. It is enough if your programs work for a single client connected to a server for now.

Below are two video lectures to understand the concept of socket programming required to solve this exercise (you can watch either one):

Network I/O using sockets video lecture 1

Network I/O using sockets video lecture 2

Now, consider the simple client and server programs provided to you in this repository. You can use them as a guide to write your own code. The server program given to you takes one command line argument: the port number on which to listen. The client program takes two arguments: the server hostname and port number. You can give localhost as the hostname if you are running the client and server on the same machine. Once the client and server are connected to each other, the client sends a message to the server and gets a reply back.

Here is the output from these simple programs given to you (the client and server programs must be run in separate terminals):

 $ gcc server.c -o server
 $ ./server 5000
 Here is the message: Hello
 $ gcc client.c -o client
 $ ./client localhost 5000
 Please enter the message: Hello
 I got your message
Please understand these sample programs and all the socket-related system calls completely. Also note the use of functions like htons to convert from host order to network order when communicating data over the network. Once you understand these examples, use this code as a template to write your own echo client and server in C or C++. Unlike this sample server, your server must echo back exactly the message received from the client. Also, it must repeatedly read data from the client and echo it back, and not just quit after one message exchange.

Complete writing this simple echo client and server, and test it thoroughly, before you proceed to the next exercise.

Other additional references to understand socket programming:

Beej’s Guide to Network Programming

Examples of socket programs in the textbook Peterson and Davie, Sec 1.4

Learning Socket Programming in cpp

Socket Programming

Multi-threaded server
In the server program written by you so far, the main server process is accepting the client connection and also serving it (by reading data and writing back a reply). Next, you will modify your server to handle the client processing in a separate thread. Once the server accepts a new connection, it will create a new worker thread, and it will pass the client's socket file descriptor (returned by accept) to this new thread. This thread will then read and write messages from/to the client.

To get started, here are some video lectures on threads: Threads video lecture 1, Threads video lecture 2

We will use the pthread library available in C/C++ to create threads. This document has a detailed explanation of the pthreads library and its functions: introduction to Pthread API. Sections 27.1 and 27.2 explain thread creation.

You can include the pthreads library in your programs by including this header file:

#include <pthread.h>

When writing code using this library, you must use the -lpthread flag to compile your code.

The pthread library has several useful functions. You can create a thread using the pthread_create() function present in this library. When you create a worker thread to handle a client request at your server, you must pass the accepted client file descriptor as an argument to the thread function, so that it can read and write from the assigned client. Understand how arguments are passed to threads, and be careful with pointers and casting. Here is sample code that creates a thread and passes it an argument:

 void *start_function(void *arg) {
   int my_arg = *((int *) arg);
   // ...thread processing...
 }

 int main(int argc, char *argv[]) {
   int thread_arg = 0;
   pthread_t thread_id;

   pthread_create(&thread_id, NULL, start_function, &thread_arg);

   // ...more code...
 }
Change your echo server to handle the accepted client in a separate thread as described above. The created thread will focus on communicating with the client given to it as an argument at creation time, while the main server thread can go back to accepting new connections. In this way, your server can perform the echo service with multiple clients at the same time.

You can check that your server is handling a small number of clients (say 5 or 10) at the same time by opening separate terminals, and connecting multiple clients to the server from the different terminals. You should find that the server is correctly able to echo back the messages received from the multiple clients.

HTTP server specification
Your HTTP web server must parse the data received from the client and construct a HTTP request. It must then generate a suitable HTTP response and send it back to the client over the socket. Before you start, skim through HTTP Made Really Easy to understand the HTTP specifications. This HTTP Tutorial is also a useful reference. You need not implement the complete HTTP specification, but only a subset. Below are some specifications of what all aspects of HTTP you must handle.

It is enough if you handle only HTTP GET requests for simple HTML files for now. The URL specified in the HTTP GET request must be read from the local filesystem and returned in the HTTP response. A URL can resemble the path of a directory, e.g., /dir_a/sub_dir_b/, or that of a file, e.g., /dir_a/sub_dir_b/index.html. If the URL is that of a directory, we will be looking for an index.html file in that directory and serve that. You can use any starting root folder for your HTML files. We have provided a sample set of HTML files for you to use in your testing, which are stored in the directory html_files. You can use this as the root directory for your server's HTML files.

The communication between the client and the server will be through TCP protocol. If you do not have root permissions on your machine to open a socket on port 80, you can use a higher port number like 8080 for the server to listen on. It is enough to support HTTP 1.0 for now, in which the server closes the TCP connection immediately after responding to the client’s request. That is, your server worker thread can close the connection once it sends a HTTP response.

The HTTP response returned by the server should return the status code of 200 in its initial line if the requested file is found and can be successfully returned. The server must return a response with status code 404 if the requested resource cannot be found. When returning the 404 error code, this error message must be wrapped in simple HTML headers for the browser to display it correctly.

It is not required for the server to parse any of the headers in the HTTP request. For a first implementation, you can also skip returning any of the HTTP headers in the response file, and only return the status of the response followed by the actual HTML content. Once this basic version works, you should try to support the HTTP headers of Date, Content-Type, and Content-Length in the HTTP response. Note that there is a end-of-line sequence (\r\n) present after every header line, and an extra end of line present after the headers and before the start of the body. There is no need for an end-of-line sequence after the message body. To fill the headers, you may use the stat function in the C library to get information about a file like its size. You can use functions like time to get the current time. It is also fine to return dummy values in these headers for an initial implementation.

If you complete building your server with these simple specifications, you can proceed to support more HTTP functionality as well, e.g., parse more headers, or add support for more content types. (This part is optional.)

Skeleton code
Recall that the HTTP server must read data from the socket, parse the received data to form a HTTP request structure from it, generate a suitable HTTP response structure, convert this response back into a string, and write it into the socket. We have provided some skeleton code for you to get started with the HTTP processing at the server. This skeleton code is only provided as a hint, and you may choose to ignore it and write all the HTTP processing by yourself as well.

The file http_server.h defines data structures to store the HTTP request and response at the server. You can add more fields or modify the data structures in any way you want. We have provided you with some starting code to parse HTTP requests and generate HTTP responses in http_server.cpp, which you can complete based on the specification given above. In this file, you must fill in code to extract the HTTP request from a string (received on the socket) in the constructor function of the HTTP request. You must fill in code to generate a HTTP response from a received HTTP request in the function handle_request. You must fill in code in the function get_string to convert the HTTP response structure into a string to write into the socket.

You must now integrate this HTTP parsing into your simple echo server built in the previous week. The worker thread of your server that is handling the client must read data from the client socket, create a HTTP request object from it, call the function handle_request to generate a HTTP response, call the function get_string on this object to convert the response to a string, and write it into the client socket before closing the connection.

For your server code to compile correctly, you will need to include http_server.h in the server file, so that the HTTP related functions and data structures are accessible. You also need to compile the server with the http_server.cpp to generate your final executable. You can use a Makefile to compile your server correctly.

Testing your HTTP server
Open your favourite web browser (e.g., Mozilla Firefox), and type http://localhost:8080/<filename\> (with a suitable port number and filename as per your implementation) in the address bar to request for the particular file from the server. If your HTTP server works properly, then you should be able to see a corresponding webpage (if the requested URL exists) or a 404 message (if the requested URL doesn’t exist).

You can also test your server using the wget command-line tool which is used to download files from the web. If you use the command wget http://localhost:8080/<filename\> -O <output_file>, then the file at the requested url should be downloaded and saved in the output_file. Test your server thoroughly before proceeding to the next exercise.

Worker thread pool
So far, your web server was handling multiple clients by creating a separate thread for each client. However, thread creation is a high overhead task. Therefore, real-life multi-threaded servers use a pool of reusable worker threads instead. The main server creates a pool of worker threads at the start. Whenever a new client connection is accepted, the server places the accepted client file descriptor in a queue/array shared with the workers. Each worker thread fetches a client from this queue, and serves it as long as the client is connected. Once the client finishes and terminates, the worker thread goes back to the queue to get the next client to handle. In this way, the same pool of worker threads can serve multiple clients.

To begin, please watch this video to understand the multi-threaded design pattern of servers: Video lecture on multi-threaded application design.

To design a thread pool, you may follow the given steps:

First, create multiple threads.

Next, you will need a shared buffer or queue to store the accepted client file descriptors. You can use a large array or a C++ queue or any such data structure for this purpose.

The main server thread and the worker threads must use locks to access this queue without race conditions. Here's a video lecture on Locking.

You will need to use condition variables. Here's a video lecture on Condition Variables. You may also read these notes on CVs. The main server thread and worker threads will use condition variables to signal each other when clients are added or removed from the queue.

Carefully study the various functions available to correctly use locks and condition variables from this introduction to Pthread API

Once your worker thread dequeues a client socket file descriptor from the shared queue, the rest of the handling of the client request will be the same as before. You should be able to reuse all of your server HTTP processing in the code of the worker thread. Ensure that you release any locks that the thread is holding before performing operations like disk reads for files: otherwise, the other threads may stall for the lock.

After you make these changes, your server will be able to handle multiple clients concurrently as before, but without having to create a new thread for every client. Start your server in one terminal. Open separate terminals to start several different clients, and check that all of them can correctly communicate with the HTTP server. You can also print out some debug output to check that clients are correctly being assigned to worker threads, and that the worker thread is serving another client after the first client terminates.

Dynamic HTTP responses (Optional)
The simple HTTP server built in the previous week could only handle static web pages stored in the file system.

Modify the server to handle dynamic HTTP responses as well. That is, you will also handle some URLs that do not correspond to any file in the HTML root directory, but will instead run some function to generate a dynamic HTTP response.

Your server should work like the following:

All the previous URLs should still work:
Previous URL

The URL /hello?<name> should return a HTML response that says Hello <name>!:
Hello cserl!

The URL /fib?<n> should return a HTML response with the nth fibonacci number:
The 24th fibonacci number

Any other URL should return the standard 404 response.

To do this exercise, you can modify the function that generates HTML responses to handle a wide variety of URLs in this manner.

Templates
The above can be accomplished simply by generating the HTML response body using C++ strings. Something like:

response->body = "<html> <h2> Hello " + name + "!</h2> </html>";

However, you will need a more convenient way to generate HTML dynamically. The most common approach relies on templates. A template contains the static parts of the desired HTML output as well as some special syntax describing how dynamic content will be inserted. For example, the HTML file will have <html> <h2> Hello {{name}}!</h2> </html>. Then, while returning the response, you can parse this HTML file to replace instances of {{name}} with the name you get from the URL.

Checking for Memory leaks
Memory leak occurs when programmers create a memory in heap and forget to delete it. The consequences of memory leak is that it reduces the performance of the computer by reducing the amount of available memory. Eventually, in the worst case, too much of the available memory may become allocated and all or part of the system stops working correctly, the application fails, or the system slows down vastly. Memory leaks are particularly serious issues for programs like servers which by definition never terminate. Since performance testing requires running the server under high load, a server with memory leak(s) would likely run out of memory.

Valgrind is a tool used to check for memory leaks. Refer to the Memory Leaks using Valgrind to learn about Valgrind and do some practice exercises. Then, use Valgrind to remove memory leaks from servers written by you in the last three weeks.

Closed-loop load generator client
To measure the capacity of your web server, you will first build a load generator client to rapidly fire requests at the server. Begin with writing a simple HTTP client that sends a HTTP request to the web server. You can add HTTP request generation logic to your simple echo client, for example. Next, modify this client to act as a closed-loop load generator, i.e., load is generated from a certain number of concurrent emulated users. You are not expected to modify your server in any way, except removing memory leaks or fixing any bugs that may arise during the testing.

Your load generator will be a closed-loop multi threaded program, with the number of concurrent users/threads, think time between requests, and the duration of the load test specified as command line arguments. A simple template is provided here, which you may choose to use. Each thread of the load generator will emulate a HTTP user/client, by sending a HTTP request to the server, waiting for a response from the server, and firing the next request after the think time. You can use a pre-defined set of URLs to request from each emulated user, and ensure that the corresponding resources are available at the server as well. The load generator threads also need not display the received HTTP responses. After all the load generator threads run for the specified duration, the load generator must compute (across all its threads) and display the following performance metrics before terminating.

Average throughput of the server, defined as the average number of HTTP requests per second successfully processed by the server for the duration of the load test.

Average response time of the server, defined as the average amount of time taken to get a response from the server for any request, as measured at the load generator.

After writing the code for your load generator to generate load and compute throughput/response time statistics, you must run a load test on your server. Run multiple experiments by varying the load level (i.e., number of concurrent load generating threads) at the load generator. You can experiment with different think times as well to see what works well. In the end, you must generate plots of the average throughput and response time of the server as a function of the load level. You must then use these performance graphs to estimate the capacity of your server, and identify the resource causing the performance bottleneck.

Load testing guidelines
A few things to keep in mind when running this load test:

Each experiment at a given load level must run for at least a minute to ensure that the throughput and response time have converged to steady state values. Your plots for throughput and response time should include at least 5 throughput/latency measurements at 5 different load levels, and must show the server reaching saturation capacity.

If all goes well, you will notice that the average server throughput initially increases with increasing load, but eventually flattens out at the server's capacity. The response time of the server starts small, but rapidly grows as the server approaches its maximum capacity. At the load when the server hits capacity, you will also notice that some hardware resource (e.g., CPU or network or disk) has hit close to 100% utilization.

You may use a client and server running on one or more CPU cores for your capacity measurement. It is highly recommended that you perform your load test with multiple cores assigned to the server, in order to fully test that your server can saturate multiple cores with its threading model. You may use as many worker threads as needed to saturate all the cores of the server. Of course, assign resources based on the availability of machines at your end.

It is recommended that you use two separate (physical or virtual) machines to host the server and the load generator, to easily separate their resource usages. If you cannot manage two separate machines, you must at least pin the server and the load generator processes to separate CPU cores, e.g., using the taskset command. Without a clear separation of client and server resources, your results of the load test will not make any sense.
For example, you may want to pin the server and load generator processes to (virtual) CPU cores 0 and 1, respectively. (taskset -c 0 ./server 8000 and taskset -c 1 ./load_gen localhost 8000 100 0.5 60).

When you find that the server's throughput has flattened out, you must verify that the server's capacity is limited by some hardware bottleneck (e.g., CPU or network or disk). If you find that the throughput of your server is flattening out even with no apparent hardware bottleneck, you must investigate why your server is not able to handle more requests. Perhaps your load generator is not generating enough load, or your server does not have enough worker threads to handle all the requests coming in. Or perhaps you are printing out too much debug output to the screen, causing the server to wait for I/O most of the time. You must carefully debug your experiments until you are convinced that you have really saturated some hardware resource at your server. You can use tools like top or iostat to measure the utilization of various hardware resources at the server.
For example, you may want to use top to measure the CPU utilization of the server. (top -p <pid of server>).

Note that the throughput and response time you measure will be a function of how many files your server is serving, what your think time is, and so on. For example, if your server is serving only a small set of files, the files may be served mainly from the disk buffer cache. But if the server is serving a large set of files, it may have to go to disk more often, resulting in lower throughput and higher response time. Therefore, you must carefully interpret your throughput and response time measurements to convince yourself that they make sense.

You may assign as many hardware resources (CPU cores, memory etc.) as required to your load generator, in order to ensure that it is capable of generating enough load to saturate the server. You must ensure that the system whose capacity is being measured (the HTTP server) is saturated by some hardware resource, while the system that is generating load (the load generator) is not saturated and is able to generate enough load.

You may find that your server cannot process all client requests, especially at high loads. You must carefully write your code to gracefully handle all possible failure scenarios that may occur under high loads. For example, if a load generator thread fails to connect to the server, it must retry again before proceeding to send requests.
