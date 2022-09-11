# Packet Capture Exercise

See the problem [here](/PROBLEM.md).

## Repository Breakdown

- /[packet-capture-exercise](packet-capture-exercise)/
  - Overall Visual Studio solution, and project containing final implementation
- /[src](src)/ directory
  - Contains all source code
- /[testing](testing)/
  - Visual Studio project with more targeted testing to verify functionality
- /[packet-analysis-short.excalidraw](incoming-packets-analysis.excalidraw), /[packet-analysis-long.excalidraw](incoming-packets-analysis.excalidraw)
  - [Excalidraw](https://excalidraw.com/) sketch visualizing the first 15 packets (700 bytes) and 37 packets (2500 bytes) respectively from a hex dump of [OUCHLMM2.incoming.packets](OUCHLMM2.incoming.packets)
- /[OUCHLMM2.incoming.packets](OUCHLMM2.incoming.packets), /[OUCHLMM2.incoming.expected](OUCHLMM2.incoming.expected)
  - The provided packet captures to be parsed and the expected output respectively
