CFLAGS += -g
CC := hipcc

.PHONY: all
all: build
build: prog prog.inst prog.proto

prog: prog.cc
	$(CC) $(CFLAGS) -o $@ prog.cc

prog.proto: prog.inst
	. ./setenv.sh
	rocprof-sys-run "--" ./prog.inst
	mv rocprofsys-tests-output/prog/perfetto-trace.proto ./prog.proto

clean:
	rm -rf prog prog.inst

rebuild: clean build

run: build
	./main

prog.inst: prog
	. ./setenv.sh
	rocprof-sys-instrument -o prog.inst \
	    --log-file mylog.log --verbose --debug \
		"--print-instrumented" "functions" "-e" "-v" "2" "--caller-include" \
		"inner" "-i" "4096" "--" ./prog

