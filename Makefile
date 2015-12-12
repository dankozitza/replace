# 980e22acaf7aa1e7ffdf8fa6fd0b0c6c
# Generated with vfnmkmc by the mc program.
PREFIX=/usr/local
CFLAGS=-O$(O)  -std=c++11
O=2
LFLAGS= -l pcre2-8
OBJS=objs/replace.o objs/radix.o objs/vectors.o objs/pcre2.o objs/system.o objs/utils.o objs/strings.o


.PHONY: all
all: objs replace

./replace:  $(OBJS)
	@ echo "    LINK ./replace"
	@ $(CXX) $(OBJS) -o "./replace" $(LFLAGS)

objs/replace.o: src/replace.cpp src/tools.hpp src/sorters.hpp
	@ echo "    CXX  src/replace.cpp"
	@ $(CXX) $(CFLAGS) -c "src/replace.cpp" -o $@
objs/radix.o: src/sorters/radix.cpp src/sorters/../sorters.hpp
	@ echo "    CXX  src/sorters/radix.cpp"
	@ $(CXX) $(CFLAGS) -c "src/sorters/radix.cpp" -o $@
objs/vectors.o: src/tools/vectors.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/vectors.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/vectors.cpp" -o $@
objs/pcre2.o: src/tools/pcre2.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/pcre2.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/pcre2.cpp" -o $@
objs/system.o: src/tools/system.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/system.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/system.cpp" -o $@
objs/utils.o: src/tools/utils.cpp src/tools/../sorters.hpp \
 src/tools/../tools.hpp
	@ echo "    CXX  src/tools/utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/utils.cpp" -o $@
objs/strings.o: src/tools/strings.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/strings.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/strings.cpp" -o $@

objs:
	@ mkdir "objs"

.PHONY: c clean
c: clean
clean:
	@ if [ -d "objs" ]; then rm -r "objs"; fi
	@ rm -f "./replace"
	@ echo "    CLEAN"
.PHONY: f fresh
f: fresh
fresh: clean
	@ make all --no-print-directory
.PHONY: r run
r: run
run: all
	@ ././replace

.PHONY: d debug
d: debug
debug: CFLAGS += -DDEBUG -g3 -Wall -Wextra
debug: O=0
debug: all

.PHONY: sc superclean
sc: superclean
superclean: clean
	@ rm -rf "deps"
	@ echo "    SUPERCLEAN"

.PHONY: install
install: all
	@ install -D -m 755 replace $(PREFIX)/bin/replace
	@ echo "[1;32m*[0m replace installed in $(PREFIX)/bin"
	@ echo "[1muse \`make uninstall\` to remove replace[0m"

.PHONY: uninstall
uninstall:
	@ rm $(PREFIX)/bin/replace
	@ echo "[1;32m*[0m replace removed from $(PREFIX)/bin[0m"

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S src/*
