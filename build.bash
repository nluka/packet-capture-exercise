rm -r src *.packets
mkdir src
echo "copying files"
cp /mnt/c/code/packet-capture-exercise/src/* src -r
cp /mnt/c/code/packet-capture-exercise/testing/*.{packets,expected} . -r
cp /mnt/c/code/packet-capture-exercise/OUCHLMM2.incoming.{packets,expected} .
echo "compiling 'testing'"
g++-11 -o testing.elf -g -O2 -Wall -Wextra -Wpedantic -Wconversion -std=c++2a src/{decode,test,testing,util}.cpp
echo "compiling 'main'"
g++-11 -o packets.elf -g -O2 -Wall -Wextra -Wpedantic -Wconversion -std=c++2a src/{decode,main,util}.cpp
