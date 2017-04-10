default: all

all: qmaze objdump

.FORCE:
qmaze: .FORCE
	cd qmaze && make
objdump: .FORCE
	cd objdump && make
clean:

clean:
	cd qmaze   && make clean && cd ..
	cd objdump && make clean && cd ..
