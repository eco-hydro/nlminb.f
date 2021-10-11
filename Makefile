## Linux version -------------------------------------------------------------

CC = gcc
FC = gfortran

# copied from Rcpp
# CFLAGS = -g -fPIC -DNDEBUG  -I'include' -O2 -Wall  -std=gnu99 -mfpmath=sse -msse2 -mstackrealign
# FFLAGS = -g -fPIC -fno-optimize-sibling-calls -O2  -mfpmath=sse -msse2 -mstackrealign
# target=libnlminb.so

OBJS := src/portsrc.o src/d1mach.o src/dv7ipr.o #src/drmnfb.o #
SRCS := $(patsubst %o, %f, $(OBJS)) 


# openblas=lib/BLAS-3.8.0/blas_LINUX.a
# openblas=/mnt/d/WSL/Ubuntu-20.04/rootfs/home/kong/.julia/artifacts/4851a61c86b2d386d66f06445feac7e9cf8ce7ea/lib
# /libopenblas.so
# -lblas
# -Llib/x64 -lopenblas64_ -L"C:/Program Files/julia/Julia 1.5.1/bin"
# echo $(shell uname)
# lib/libblas_linux.a
${target}:$(OBJS) src/port.o 
	gfortran -shared -static-libgcc -o ${target} $^ -lopenblas
	
src/port.o:src/port2.c
	gcc $(CFLAGS) -c $^ -o $@

$(OBJS): %.o:%.f
	$(FC) $(FFLAGS) -c $^ -o $@ 

# -c $< 
# echo "$(FC) $(FFLAGS) -c $^ #-o $@ "
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
