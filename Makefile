objects=tinyhtm.o tinyhtmparser.o htmtest.o

htmtest: $(objects)
	g++ -o htmtest $(objects)

htmtest.o: htmtest.cpp
	g++ -c htmtest.cpp

tinyhtm.o: tinyhtm.h tinyhtm.cpp
	g++ -c tinyhtm.cpp

tinyhtmparser.o: tinyhtm.h tinyhtmparser.cpp
	g++ -c tinyhtmparser.cpp

clean:
	rm $(objects)
