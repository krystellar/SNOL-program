CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET   = snol

all: $(TARGET)

$(TARGET): main.cpp lexer.cpp parser.cpp lexer.h parser.h symboltable.h
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp lexer.cpp parser.cpp

clean:
	rm -f $(TARGET)
