# Systems Programming 

## Architecture

This client-server model utilizes pipes, sockets and threads.

## Server

The server uses sockets to establish and accept connections between itself and one or more client(s).
As each new connection is requested the server accepts and forks using the lazy approach, thereby
making a separate process for this newly arrived client. This way multiple clients are handled as
each is enclosed in a separate process from the server parent, leaving the server to accept more
connections and handle/process other requests, while the server children(clients) are free to carry
out their functions simultaneously.

Within each server child are functions outlined ahead. There is an infinite while loop
which iterates over to process each command and sends that output back the client over the socket.

Server interactivity with the user is achieved by pipes and threads, one thread is created in the
server (parent) while the other thread is in each server child. Two pipes are then created in the
server parent so that after forking each child gets its own copy of the pipe. In the server thread user
input is taken in and piped to each child. This input is then read by the child from it's copy of the
pipe and processed in the child thread as it has access to the data regarding the client, e.g in case all
processes of all children are requested from the user by the command “list.” This is picked up from
the child from the reading end of its pipe, who takes it as an indication that the server is requesting
the subsequent data. It then retrieves the process list stored in the structure and then sends it back to
the parent (server) over the pipe. The server thread then reads the data from the pipe and then prints
it out on the screen. Other commands such as retrieving the list of client IPs that are connected to
the Server, are handled by the server thread itself as it already has the data, and is printed on the
screen.

## Client

The client establishes a connection with the server over a socket. It achieves interactivity with the
user by two threads. One thread runs an infinite loop and takes in input from the user thus multiple
commands can be inputted, these are then sent to the server over the socket. The other thread is
responsible for receiving from the server through the socket and outputting those results sent by the
server onto the screen.

## Usage

### Pre-requisites
Linux/Ubuntu required. Mac OS may cause some signals to behave unexpectedly.

### Setup
Run the Server file on a different computer than the client. The client file can be copied to run as many clients as you want. This number is however restricted arbitrarily to 100 at the moment.

## Commands

### For the client:
help – shows the user how to use the add, sub, div, mul commands.<br/>
run [program name] - to run any program.<br/>
list – to list all active processes, shows their names, pids, start time, end time, elapsed time and their status.<br/>
listall – shows a list of all active and inactive processes and their subsequent info.<br/>
kill [pid] – kills the process identified by the ID given.<br/>
kill [name] – kills program whose name is given, if multiple instances of the same program are open, then only one is killed.<br/>
Kill all – kills all active processes open by the client.<br/>
add – followed by the numbers that the user wants to add together.<br/>
sub – followed by numbers that user wants to subtract.<br/>
div - followed by numbers that user wants to divide.<br/>
mul – followed by numbers that user wants to multiply.<br/>
connect – followed by IP and port number to which the client wishes to connect to.<br/>
disconnect – to disconnect client from the server.<br/>
exit – to kill all it's (client's) active processes and shut itself down.<br/>

### For the server
list – lists all processes of all clients.<br/>
list [IP] – lists all processes of the client with the given IP.<br/>
list connections – lists IP and port number of all the clients connected.<br/>
exit – kills all client processes, thus disconnecting them and then kills itself.<br/>

Limitations 
Failed run command not handled.<br/>
Client cannot reconnect once it requests disconnection.<br/>

