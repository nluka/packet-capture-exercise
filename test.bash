valgrind --track-origins=yes --leak-check=full ./testing.elf input.packets
valgrind --track-origins=yes --leak-check=full ./packets.elf input.packets
