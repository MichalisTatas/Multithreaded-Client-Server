PREPEI NA DW IP WORKER na to stelnw oci 0.0.0.0

kati paoize me fd tou master

kanonika 8elei htnos pantou alla isws oxi edw popu ta exw string ola?


****make the child thats created after signal send port w etc


max size on buffer

the workers send one byte to say w to know they are workers
the client sends a c to know its a client
if it is worker to the statistics stuff
if it is the client create thread to do the query stuff yeah


buffersize isnt needed anymore so probably i can read write the whole thing
at once?

to do :
	- create trwo funtions
		one for the statistics handling
		one for queries handling
		and send the queries to the worker so he can answer?
		maybe server can have something like the queriesHandling
		to send the queries to the right worker(s)
	- set max size on buffer (and dischange more connections?) 
	- maybe change buffersize ie msgComposer/msgDecomposer on client/server
	- workers send a port that they will wait for info

