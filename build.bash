rm -r src *.packets
mkdir src
echo "copying files"
cp /mnt/c/code/packet-capture-exercise/src/* src -r
cp /mnt/c/code/packet-capture-exercise/testing/*.{packets,expected} . -r
cp /mnt/c/code/packet-capture-exercise/{OUCHLMM2.incoming,100x}.{packets,expected} .
echo "compiling 'testing'"
g++-11 -o testing.elf -g -Wall -Wextra -Wpedantic -Wconversion -std=c++2a src/{packet,parse,stream,test,testing,util}.cpp
echo "compiling 'main'"
g++-11 -o packets.elf -g -Wall -Wextra -Wpedantic -Wconversion -std=c++2a src/{main,packet,parse,stream,util}.cpp
