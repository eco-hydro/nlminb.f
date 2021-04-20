## Linux version -------------------------------------------------------------

CC = gcc
FC = gfortran

# copied from Rcpp
CCFLAGS = -fPIC -DNDEBUG  -I'include' -O2 -Wall  -std=gnu99 -mfpmath=sse -msse2 -mstackrealign
FCFLAGS = -fPIC -fno-optimize-sibling-calls -O2  -mfpmath=sse -msse2 -mstackrealign

OBJS := src/portsrc.o src/d1mach.o
SRCS := $(patsubst %o, %f, $(OBJS)) 

target=nlminb.so

# -lblas
# -Llib/x64 -lopenblas64_ -L"C:/Program Files/julia/Julia 1.5.1/bin"
# echo $(shell uname)
# lib/libblas_linux.a
${target}.dll:$(OBJS) src/port.o 
	gfortran -shared -s -static-libgcc -o ${target} $^ lib/BLAS-3.8.0/blas_LINUX.a

src/port.o:src/port2.c
	gcc $(CCFLAGS) -c $^ -o $@

$(OBJS): %.o:%.f
	$(FC) $(FCFLAGS) -c $^ -o $@ 

# -c $< 
# echo "$(FC) $(FCFLAGS) -c $^ #-o $@ "

clean:
	@rm -f *.mod *.o *.dll *.so *.exe
	@rm -f  src/*.o

# $@    the file name of the target
# $<    the name of the first prerequisite (i.e., dependency)
# $^    the names of all prerequisites (i.e., dependencies)
# $(@D)    the directory part of the target
# $(@F)    the file part of the target
# $(<D)    the directory part of the first prerequisite (i.e., dependency)
# $(<F)    the file part of the first prerequisite (i.e., dependency)
