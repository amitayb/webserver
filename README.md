# webserver
# This is a Simple Web server that listen to address and port given, send GET request to given address in argv.
# Request will be sent after connecting to server from any client and Answer will be returned to client.


# How to compile

Change the include for libs and headers in the make file :  
-I /usr/local/Cellar/boost/1.62.0/include/ 
-L /usr/local/Cellar/boost/1.62.0/lib 

run make
run server with command :   ./webserver 127.0.0.1 80 www.boost.org,10.0.0.1,  /index.html  (notice there is "," delimiter for every address to send request to).
open browser and go to 127.0.0.1/index.html
