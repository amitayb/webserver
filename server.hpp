//
//  server.hpp
//  webServer
//
//  Created by Amitay Blum on 11/23/16.
//  Copyright © 2016 Amitay Blum. All rights reserved.
//

#ifndef server_h
#define server_h

#include <boost/asio.hpp>
#include <string>
#include "connection.hpp"
#include "manager.hpp"



namespace http {
    namespace server {
        
        /// The top-level class of the HTTP server.
        class server
        {
        public:
            server(const server&) = delete;
            server& operator=(const server&) = delete;
            
            /// Construct the server to listen on the specified TCP address and port, and
            /// serve up files from the given directory.
            explicit server(const std::string& address, const std::string& port, const std::string& remoteAddress ,
                            const std::string& path);
            
            /// Run the server's io_service loop.
            void run();
            
        private:
            /// Perform an asynchronous accept operation.
            void do_accept();
            
            /// Wait for a request to stop the server.
            void do_await_stop();
            
            /// The io_service used to perform asynchronous operations.
            boost::asio::io_service io_service_;
            
            /// The signal_set is used to register for process termination notifications.
            boost::asio::signal_set signals_;
            
            /// Acceptor used to listen for incoming connections.
            boost::asio::ip::tcp::acceptor acceptor_;
            
            /// The connection manager which owns all live connections.
            manager connection_manager_;
            
            /// The next socket to be accepted.
            boost::asio::ip::tcp::socket socket_;
            
            
            /// The path to listen
            std::string path_;
            
            /// The servers to send get request to
            std::string servers_;
            
//            /// The handler for all incoming requests.
//            request_handler request_handler_;
        };
        
    } // namespace server
} // namespace http

#endif /* server_h */
