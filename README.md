Prerequisites:
--------------

Linux, Mac OS:
GCC Compiler version 5 or higher

Windows:
MinGW Compiler (GCC 5 or higher)

Compile:
--------
g++ -std=c++11 -o cex cex.cpp cpp/exchange.cpp cpp/validator.cpp

Run:
----
1) cex < test1.tst   (use a file with requests as input)
2) cex               (use UI and input requests manually)

UI:
---
type in requests according to the format, to exit the app type in "STOP"

Technologies:
-------------
written in C++ (C++11, STL)

Limitations:
------------
Since the algorithm takes O(N3) of calculations and O(N2) of memory where N is the number of pairs(exchange, currency),
the app has a limitation of N=1500. This is just simple protection for the app not to get stuck if it is given too many pairs.

The production solution for that problem could be:
- dynamical memory reallocation for working data
- storing update requests in the queue allowing immediate response to exchange request using previously calculated working data
- updating the exchange graph and recalculating working data in the separate thread by the timer for instance
- once it is done, working data should be switched to the newly recalculated working data
- this has to be done with the respect to synchronization between working threads