all: httpd

ifeq ($(CXX),)
  CXXD=g++
else
  CXXD=$(CXX)
endif

run:
	./web220.wt --http-address 0.0.0.0 --http-port 8080 --docroot .

httpd:
	$(CXXD) -ggdb -I/usr/local/include -lwthttp -lglib-2.0 -lvterm -lutil -o web220.wt *.cpp
