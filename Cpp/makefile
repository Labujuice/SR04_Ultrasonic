CC:=g++
exe:=test
obj:=test.o

all:$(obj)
	$(CC) -o $(exe) $(obj)  
%.o:%.cpp
	$(CC) -c $^ -o $@

.PHONY:clean
clean:
	rm -rf $(obj) $(exe)