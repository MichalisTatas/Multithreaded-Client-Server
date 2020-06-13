
****FIUND BETTER WAY TO LOCK ALL THE THREADS THAN SLEEPING 5 TO MAKE SURE EVERYTHING IS LOCKED

for some reason it doesnt read all the messages or what it prints
just the first one investigate that after client



probably change statistics so far to do the circular buffer of the gods yeah
the workers send one byte to say w to know they are workers
the client sends a c to know its a client
if it is worker to the statistics stuff
if it is the client create thread to do the query stuff yeah


buffersize isnt needed anymore so probably i can read write the whole thing
at once?

master:
	- refactored completely i think kids gets new data  ready to sent statistics to server
	- have to find a way so that every workere must have unique port to seent to server so that
	  server camn talk back for queriesa

server:
	- can connect to client etc only single thread for now 

client:
	- able to connect to server

next in order to be done:
	- first connect master with server and send statistics and wait for queries
	- then connect client to server to send the queries
	- make server and client multithreaded

to do :
	- see more info about getsockopt and if i need to use it

today : 
	- min : be able to send queries and get answer 
	  max i guess : do the whole networking master->server<-client so tomorrow do the multithreads
