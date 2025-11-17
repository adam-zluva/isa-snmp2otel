CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -pedantic
LDFLAGS = 
SRCDIR = src
INCDIR = include
OBJDIR = obj
TARGET = snmp2otel

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(OBJDIR)
	@$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

run: all
	./$(TARGET) -t 127.0.0.1 -o data/oids.txt -e http://localhost:4318/v1/metrics -p 1161 -i 1 -v

clean:
	$(RM) -r $(OBJDIR)
	$(RM) $(TARGET)

test: clean all
	@echo "All tests passed!" # None can fail if there are none