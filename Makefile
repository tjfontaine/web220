all: httpd

run:
	./ajaxconsole.wt --http-address 0.0.0.0 --http-port 8080 --docroot .

httpd:
	g++ -ggdb -I/usr/local/include -lwthttp -lglib-2.0 -lvterm -lutil -o ajaxconsole.wt *.cpp
