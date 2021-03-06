# installation prefix; executables are installed in $(PREFIX)/bin
PREFIX = $(HOME)

# test for ccache
CCACHE = $(shell which ccache 2>/dev/null)

ifneq ($(CCACHE),)
CC = ccache gcc
else
CC = gcc
endif

# test for architecture
UNAME = $(shell uname)

ifeq ($(UNAME),Linux)
CFLAGS = -Wall -O3
LIBFLAGS = -lgsl -lgslcblas -lm
else
ifeq ($(UNAME),Darwin)
CFLAGS = -Wall -O3 -I/sw/include -I/sw/include/gnugetopt -L/sw/lib
LIBFLAGS = -lgsl -lgslcblas -lgnugetopt -lm
else
CFLAGS = -Wall -O3
LIBFLAGS = -lgsl -lgslcblas -lm
endif
endif

# the core fewbody objects
FEWBODY_OBJS = fewbody.o fewbody_classify.o fewbody_coll.o fewbody_hier.o \
	fewbody_int.o fewbody_io.o fewbody_isolate.o fewbody_ks.o \
	fewbody_nonks.o fewbody_scat.o fewbody_utils.o

all: cluster triplebin binbin binsingle sigma_binsingle bin scatter_binsingle

cluster: cluster.o $(FEWBODY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

triplebin: triplebin.o $(FEWBODY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

binbin: binbin.o $(FEWBODY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

binsingle: binsingle.o $(FEWBODY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

sigma_binsingle: sigma_binsingle.o $(FEWBODY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

bin: bin.o $(FEWBODY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

scatter_binsingle: scatter_binsingle.o $(FEWBODY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

triple: triple.o $(FEWBODY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

cluster.o: cluster.c cluster.h fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

triplebin.o: triplebin.c triplebin.h fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

binbin.o: binbin.c binbin.h fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

binsingle.o: binsingle.c binsingle.h fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

sigma_binsingle.o: sigma_binsingle.c sigma_binsingle.h fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

bin.o: bin.c bin.h fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

scatter_binsingle.o: scatter_binsingle.c fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

triple.o: triple.c triple.h fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c fewbody.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

install: cluster triplebin binbin binsingle sigma_binsingle bin
	mkdir -p $(PREFIX)/bin/
	install -m 0755 cluster triplebin binbin binsingle sigma_binsingle bin \
	scatter_binsingle $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/cluster $(PREFIX)/bin/triplebin $(PREFIX)/bin/binbin \
	$(PREFIX)/bin/binsingle $(PREFIX)/bin/sigma_binsingle $(PREFIX)/bin/bin \
	$(PREFIX)/bin/scatter_binsingle

clean:
	rm -f $(FEWBODY_OBJS) cluster.o triplebin.o bin.o binbin.o binsingle.o \
	sigma_binsingle.o cluster triplebin binbin binsingle sigma_binsingle bin \
	scatter_binsingle.o scatter_binsingle

mrproper: clean
	rm -f *~ *.bak *.dat ChangeLog
	rm -f */*~ */*.bak */*.dat
