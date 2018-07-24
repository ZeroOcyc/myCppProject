CC := g++  
CFLAGS := -g  
TARGET := test  
SRCS := $(wildcard *.cpp)  
OBJS := $(patsubst %cpp,%o,$(SRCS))  
LIBS := -lpthread
all:$(TARGET)  
%.o:%.cpp  
	$(CC) $(CFLAGS) -c $^ 
$(TARGET):$(OBJS)  
	$(CC) $(CFLAGS) -o $@ $^  $(LIBS)
clean:  
	rm -rf $(TARGET) *.o 