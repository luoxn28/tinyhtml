objects= tinyhtmparser.o tinyhtm.o htmtest.o

htmtest: $(objects)
	g++ -o htmtest $(objects)

htmtest.o: htmtest.cpp
	g++ -c htmtest.cpp

tinyhtmparser.o: tinyhtm.h tinyhtmparser.cpp
	g++ -c tinyhtmparser.cpp

tinyhtm.o: tinyhtm.h tinyhtm.cpp tinyhtmparser.o
	g++ -c tinyhtm.cpp

clean:
	rm $(objects)

cleanall:
	rm $(objects) htmtest
