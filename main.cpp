//
//  main.cpp
//  webServer
//
//  Created by Amitay Blum on 11/23/16.
//  Copyright © 2016 Amitay Blum. All rights reserved.
//

#include <iostream>
#include <boost/asio.hpp>
#include "server.hpp"


int main(int argc, const char * argv[]) {
    try
    {
        // Check command line arguments.
        if (argc != 5)
        {
            std::cerr << "Usage: ./server2 <address> <port> <Remote addresses> <Path>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    ./server2 127.0.0.1 80 www.boost.org,10.0.0.1, / .\n";
            return 1;
        }
        
        // Initialise the server.
        http::server::server s(argv[1], argv[2],argv[3],argv[4]);
//        
//        // Run the server until stopped.
        s.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
    return 0;
}
