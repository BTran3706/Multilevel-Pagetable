all: pagetable

pagetable: byu_tracereader.o output_mode_helpers.o pagetable.o main.o
	g++ -std=c++11 byu_tracereader.o output_mode_helpers.o pagetable.o main.o -o pagetable

byu_tracereader.o: byutr.h byu_tracereader.c
	gcc -c byu_tracereader.c
	
output_mode_helpers.o: output_mode_helpers.h output_mode_helpers.c
	gcc -c output_mode_helpers.c
	
pagetable.o: pagetable.h pagetable.cpp
	g++ -std=c++11 -c pagetable.cpp
	
main.o: map.h level.h pagetable.h main.cpp
	g++ -std=c++11 -c main.cpp

clean:
	rm -f *.o