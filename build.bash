rm -r src
mkdir src
echo "copying files"
cp /mnt/c/code/packet-capture-exercise/src/* src -r
cp /mnt/c/code/packet-capture-exercise/OUCHLMM2.incoming.packets input.packets -r
cp /mnt/c/code/packet-capture-exercise/testing/*.{expected,packets} . -r
echo "compiling testing"
g++-11 -o testing.elf -g -Wall -Wextra -Wpedantic -Wconversion -std=c++2a src/{packet,process,test,testing,util}.cpp
echo "compiling main"
g++-11 -o packets.elf -g -Wall -Wextra -Wpedantic -Wconversion -std=c++2a src/{main,packet,process,util}.cpp
