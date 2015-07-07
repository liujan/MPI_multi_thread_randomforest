obj = Node.o Tree.o RandomForest.o main.o
target=random

$(target):$(obj)
	mpic++ -o $(target) $(obj)  -lpthread

Node.o:Node.h
	mpic++ -c Node.cpp
Tree.o:Node.h Tree.h
	mpic++ -c Node.cpp Tree.cpp
RandomForest.o:RandomForest.h Tree.h Node.h
	mpic++ -c Tree.cpp Node.cpp  RandomForest.cpp  -lpthread
main.o:main.cpp RandomForest.h Tree.h Node.h
	mpic++ -c main.cpp RandomForest.cpp Tree.cpp Node.cpp  -lpthread

clean:
	rm -rf $(obj)
