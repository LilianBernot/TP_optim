# Project

The purpose of this project is to discover optimization and parallelization. The goal is to optimize the code as much as we can.

# Commands

To compile the project, run : 
```bash 
make
```

To run the compiled code :
```bash 
time ./transform_image $IMAGES/transfo.txt > ./results/run.md
```

To run the tests and change you have not modified the output : 
```bash 
make test
```

# Optimizations

The optimizations have been done in this order : 
- delete useless scanf : scanf multiple elements in 1 function call.
- delete copy of the image : we don't really need to copy the image data.
- fuse light and curve : both functions do loops on the image which can be done in only one.
- changed double loops to 1 : instead or reading with i being width and j being height, we directly go with k going from 0 to width*height. 
- changed read and write : replace fread/fwrite calls for single bytes with block reads and writes. These reduce the number of system calls. 
- Loop up : use block read instead of byte by byte.
- Btter malloc : improve how they are given.
- Parallelization : the transfo loop is parallelized with OMP.

