CC = gcc 
CFLAGS = -Wall 
LDFLAGS = -std=c++20
OBJFILES = Main.o Credentials.o 
TARGET = pwm
all : $(TARGET)

$(TARGET) : Main.cpp Credentials.cpp
	$(CC) $(CFLAGS) Main.cpp Credentials.cpp -lstdc++ -o $(TARGET) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~
