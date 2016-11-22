CFLAGS= -g -Wall -O3
LDFLAGS= -lm

SOURCEDIR= src
TESTDIR= tests
BUILDDIR= build
HEADERDIR= src

SOURCES= $(wildcard $(SOURCEDIR)/*.c)
OBJECTS= $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))
TESTS= $(wildcard $(TESTDIR)/*.c)
TESTOBJ= $(patsubst $(TESTDIR)/%.c, $(BUILDDIR)/%.o, $(TESTS))
TESTBIN= $(patsubst $(BUILDDIR)/%.o, $(BUILDDIR)/%, $(TESTOBJ))

all: tests

tests: $(TESTBIN)

$(TESTBIN): $(BUILDDIR)/$(OBJECTS) $(BUILDDIR)/$(TESTOBJ)
	$(CC) $(CFLAGS) -I$(HEADERDIR) $(OBJECTS) $@.o -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -I$(HEADERDIR) -c $< -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -I$(HEADERDIR) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TESTBIN) $(OBJECTS) $(TESTOBJ)
