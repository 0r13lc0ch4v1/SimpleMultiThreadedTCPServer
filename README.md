# Simple Multi-Treaded TCP server for windows

Tested on windows 10 x64 Build 1903.
Compiled using vs2019.

This is a vary basic multi-threaded tcp server written in c++.

### Example of use
    #include "TCPServer.hpp"
    #include <cstdio>
    int main()
    {
    	TCPServer tcpServer{};
	tcpServer.Run();
	
	std::puts("Press enter to close server");
	std::getc(stdin);

	return 0;
    }
