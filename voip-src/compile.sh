c++ -std=c++14 server.cpp -o server -Wall -O3 -pthread -g
c++ -std=c++14 client.cpp -o client -Wall -O3 -pthread -g
mv server ../bin/
mv client ../bin
