CC=gcc
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


xor_test: $(BUILDDIR)/$(OBJECTS) $(BUILDDIR)/xor_test.o
	$(CC) $(CFLAGS) -I$(HEADERDIR) $(OBJECTS) $(BUILDDIR)/xor_test.o -o $@ $(LDFLAGS)

sin_test: $(BUILDDIR)/$(OBJECTS) $(BUILDDIR)/sin_test.o
	$(CC) $(CFLAGS) -I$(HEADERDIR) $(OBJECTS) $(BUILDDIR)/sin_test.o -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -I$(HEADERDIR) -c $< -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -I$(HEADERDIR) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f sin_test xor_test $(OBJECTS) $(TESTOBJ)
