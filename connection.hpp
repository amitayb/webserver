//
//  connection.hpp
//  webServer
//
//  Created by Amitay Blum on 11/25/16.
//  Copyright © 2016 Amitay Blum. All rights reserved.
//

#ifndef connection_hpp
#define connection_hpp

#include <stdio.h>
#include <array>
#include <memory>
#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_parser.hpp"

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;


namespace http {
    namespace server {
        
        class manager;
        
        /// Represents a single connection from a client.
        class connection
        : public std::enable_shared_from_this<connection>
        {
        public:
            connection(const connection&) = delete;
            connection& operator=(const connection&) = delete;
            
            /// Construct a connection with the given socket.
            explicit connection(boost::asio::ip::tcp::socket socket, boost::asio::io_service& io_service,
                                manager& manager,const std::string& path,const std::string& servers);
            
            /// Start the first asynchronous operation for the connection.
            void start();
            
            /// Stop all asynchronous operations associated with the connection.
            void stop();
            
            void run(std::string& server);
        private:
            /// Perform an asynchronous read operation.
            void do_read();
            
            /// Perform an asynchronous write operation.
            void do_write();
            
            /// Perform an asynchronous append to reply operation.
            void do_append();
            
            bool url_decode(const std::string& in, std::string& out);
            void handle_resolve(const boost::system::error_code& err,
                                tcp::resolver::iterator endpoint_iterator);
            
            void handle_connect(const boost::system::error_code& err);
            
            void handle_write_request(const boost::system::error_code& err);
            
            void handle_read_status_line(const boost::system::error_code& err);
            
            void handle_read_headers(const boost::system::error_code& err);
            
            void handle_read_content(const boost::system::error_code& err);
            
            void check_deadline();
            
            void stopCli();
            
            tcp::resolver resolver_;
            
            
            std::string server_;
            std::string path_;
            
            

            /// The timer for timeout
            deadline_timer deadline_;
            
            int howmany_;
            
            bool stopped_;
            
            /// Socket for the connection.
            boost::asio::ip::tcp::socket socket_;
            
            /// The next socket to be accepted.
            boost::asio::ip::tcp::socket socketCli_;
            
            ///
            boost::asio::io_service io_service_;
            
            /// The manager for this connection.
            manager& connection_manager_;
            
            /// The handler used to process the incoming request.
            //request_handler& request_handler_;
            
            /// Buffer for incoming data.
            std::array<char, 8192> buffer_;
            
            /// The incoming request.
            request request_;
            
            /// for Get requests
            boost::asio::streambuf requestBuf_;
            
            boost::asio::streambuf responseBuf_;
            
            /// The parser for the incoming request.
            request_parser request_parser_;
            
            /// The reply to be sent back to the client.
            reply reply_;
            
            
            /// The vector to hold to GET requests
            std::vector<std::string> vector_;
            
            std::vector<std::string>::iterator it_;
            
            
            /// The servers to send get request to
            std::string servers_;
        };
        
        typedef std::shared_ptr<connection> connection_ptr;
        
    } // namespace server
} // namespace http
#endif /* connection_hpp */
