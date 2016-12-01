//
//  connection.cpp
//  webServer
//
//  Created by Amitay Blum on 11/25/16.
//  Copyright © 2016 Amitay Blum. All rights reserved.
//


#include "connection.hpp"
#include <utility>
#include <vector>
#include <iostream>
#include <vector>
#include <utility>
#include "manager.hpp"

//#include "request_handler.hpp"

#define TIMEOUT_INTERVAL 3
using boost::asio::ip::tcp;

namespace http {
    namespace server {

        connection::connection(boost::asio::ip::tcp::socket socket, boost::asio::io_service& io_service,
                               manager& manager,const std::string& path,const std::string& servers)
        : socket_(std::move(socket)),
        resolver_(io_service),
        connection_manager_(manager),
        path_(path),
        servers_(servers),
        deadline_(io_service),
        stopped_(false),
        howmany_(0),
        socketCli_(io_service)
        {
        }

        void connection::start()
        {
            do_read();
        }

        void connection::stop()
        {
            socket_.close();
        }

        void connection::do_read()
        {
            auto self(shared_from_this());
            socket_.async_read_some(boost::asio::buffer(buffer_),
                                    [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
                                    {
                                        if (!ec)
                                        {
                                            request_parser::result_type result;
                                            std::tie(result, std::ignore) = request_parser_.parse(
                                                                                                  request_, buffer_.data(), buffer_.data() + bytes_transferred);

                                            if (result == request_parser::good)
                                            {

                                                // Decode url to path.
                                                std::string request_path;
                                                if (!url_decode(request_.uri, request_path))
                                                {
                                                    reply_ = reply::stock_reply(reply::bad_request);
                                                    do_write();
                                                    //return;
                                                }

                                                // Request path must be absolute and not contain "..".
                                                if (request_path.empty() || request_path[0] != '/'
                                                    || request_path.find("..") != std::string::npos)
                                                {
                                                    reply_ = reply::stock_reply(reply::bad_request);
                                                    do_write();
                                                    //return;
                                                }



                                                if (request_path == path_)
                                                {
                                                    std::cout<<"PATH MATCH!!"<<std::endl;
                                                    std::string delimiter = ",";
                                                    size_t pos = 0;
                                                    std::string token;
                                                    while ((pos = servers_.find(delimiter)) != std::string::npos and
                                                           servers_.length() > 0)
                                                    {
                                                            token = servers_.substr(0, pos);
                                                        if (token.length() > 0 )
                                                        {
                                                            vector_.push_back(token);
                                                        }

                                                            servers_.erase(0, pos + delimiter.length());
                                                    }

                                                    if (!vector_.empty()) {

                                                        it_ = vector_.end() - 1;
                                                        run(*it_);
                                                        std::cout<<*it_<<"\n";
                                                        vector_.pop_back();

                                                    }
                                                    else
                                                    {
                                                        reply_.status = reply::ok;
                                                        reply_.content.append("<h1>Servers Address incorrect</h1>");
                                                        reply_.headers.resize(2);
                                                        reply_.headers[0].name = "Content-Length";
                                                        reply_.headers[0].value = std::to_string(reply_.content.size());
                                                        reply_.headers[1].name = "Content-Type";
                                                        do_write();
                                                    }
//                                                    it_ = vector_.end() - 1;
//
//                                                    run(*it_);
//
//                                                    vector_.pop_back();



                                                }

                                                else
                                                {

                                                    reply_.status = reply::ok;
                                                    reply_.content.append("Path DO NOT Match");
                                                    reply_.headers.resize(2);
                                                    reply_.headers[0].name = "Content-Length";
                                                    reply_.headers[0].value = std::to_string(reply_.content.size());
                                                    reply_.headers[1].name = "Content-Type";
                                                    do_write();
                                                }
                                            }
                                            else if (result == request_parser::bad)
                                            {
                                                reply_ = reply::stock_reply(reply::bad_request);
                                                do_write();
                                            }
                                            else
                                            {
                                                do_read();
                                            }
                                        }
                                        else if (ec != boost::asio::error::operation_aborted)
                                        {
                                            connection_manager_.stop(shared_from_this());
                                        }
                                    });
        }

        void connection::do_write()
        {
            auto self(shared_from_this());
            boost::asio::async_write(socket_, reply_.to_buffers(),
                                     [this, self](boost::system::error_code ec, std::size_t)
                                     {
                                         if (!ec)
                                         {
                                             // Initiate graceful connection closure.
                                             boost::system::error_code ignored_ec;
                                             socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                                                              ignored_ec);
                                         }

                                         if (ec != boost::asio::error::operation_aborted)
                                         {
                                             connection_manager_.stop(shared_from_this());
                                         }
                                     });
        }

        void connection::do_append()
        {
            auto self(shared_from_this());
            boost::asio::async_write(socketCli_, responseBuf_,
                                     [this, self](boost::system::error_code ec, std::size_t)
                                     {
                                         if (!ec)
                                         {

                                             if (!vector_.empty()) {

                                                 it_ = vector_.end() - 1;
                                                 run(*it_);
                                                 std::cout<<*it_<<"\n";
                                                 vector_.pop_back();

                                             }
                                             else
                                             {

                                                 reply_.status = reply::ok;
                                                 reply_.headers.resize(2);
                                                 reply_.headers[0].name = "Content-Length";
                                                 reply_.headers[0].value = std::to_string(reply_.content.size());
                                                 reply_.headers[1].name = "Content-Type";
                                                 do_write();
                                             }
                                         }


                                     });
        }

        bool connection::url_decode(const std::string& in, std::string& out)
        {
            out.clear();
            out.reserve(in.size());
            for (std::size_t i = 0; i < in.size(); ++i)
            {
                if (in[i] == '%')
                {
                    if (i + 3 <= in.size())
                    {
                        int value = 0;
                        std::istringstream is(in.substr(i + 1, 2));
                        if (is >> std::hex >> value)
                        {
                            out += static_cast<char>(value);
                            i += 2;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (in[i] == '+')
                {
                    out += ' ';
                }
                else
                {
                    out += in[i];
                }
            }
            return true;
        }

        void connection::run(std::string& server)
        {

            // Start the deadline actor. You will note that we're not setting any
            // particular deadline here. Instead, the connect and input actors will
            // update the deadline prior to each asynchronous operation.
            deadline_.async_wait(boost::bind(&connection::check_deadline, this));


            server_ = server;
            // Form the request. We specify the "Connection: close" header so that the
            // server will close the socket after transmitting the response. This will
            // allow us to treat all data up until the EOF as the content.
            stopped_ = false;

            std::cout<<server_<<std::endl;
            std::cout<<"this is the path: "<<path_<<" and the server "<<server_<<std::endl;
            std::ostream request_stream(&requestBuf_);
            request_stream << "GET " << path_ << " HTTP/1.0\r\n";
            request_stream << "Host: " << server_ << "\r\n";
            request_stream << "Accept: */*\r\n";
            request_stream << "Connection: close\r\n\r\n";



            // Start an asynchronous resolve to translate the server and service names
            // into a list of endpoints.
            tcp::resolver::query query(server_, "http");
            resolver_.async_resolve(query,
                                    boost::bind(&connection::handle_resolve, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::iterator));
        }

        void connection::handle_resolve(const boost::system::error_code& err,
                                    tcp::resolver::iterator endpoint_iterator)
        {
            if (!err)
            {
                // Set a deadline for the connect operation.
                deadline_.expires_from_now(boost::posix_time::seconds(TIMEOUT_INTERVAL));

                // Attempt a connection to each endpoint in the list until we
                // successfully establish a connection.
                boost::asio::async_connect(socketCli_, endpoint_iterator,
                                           boost::bind(&connection::handle_connect, this,
                                                       boost::asio::placeholders::error));
            }
            else
            {
                std::cout << "Error: " << err.message() << "\n";
            }
        }

        void connection::handle_connect(const boost::system::error_code& err)
        {
            if (!err)
            {
                // The connection was successful. Send the request.
                boost::asio::async_write(socketCli_, requestBuf_,
                                         boost::bind(&connection::handle_write_request, this,
                                                     boost::asio::placeholders::error));
            }
            else
            {
                std::cout << "Error: " << err.message() << "\n";
            }
        }

        void connection::handle_write_request(const boost::system::error_code& err)
        {
            if (!err)
            {
                // Read the response status line. The response_ streambuf will
                // automatically grow to accommodate the entire line. The growth may be
                // limited by passing a maximum size to the streambuf constructor.
                boost::asio::async_read_until(socketCli_, responseBuf_, "\r\n",
                                              boost::bind(&connection::handle_read_status_line, this,
                                                          boost::asio::placeholders::error));
            }
            else
            {
                std::cout << "Error: " << err.message() << "\n";
            }
        }

        void connection::handle_read_status_line(const boost::system::error_code& err)
        {
            if (!err)
            {
                // Check that response is OK.
                std::istream response_stream(&responseBuf_);
                std::string http_version;
                response_stream >> http_version;
                unsigned int status_code;
                response_stream >> status_code;
                std::string status_message;
                std::getline(response_stream, status_message);
                if (!response_stream || http_version.substr(0, 5) != "HTTP/")
                {
                    std::cout << "Invalid response\n";
                    return;
                }
                if (status_code != 200)
                {
                    std::cout << "Response returned with status code ";
                    std::cout << status_code << "\n";
                    return;
                }

                // Read the response headers, which are terminated by a blank line.
                boost::asio::async_read_until(socketCli_, responseBuf_, "\r\n\r\n",
                                              boost::bind(&connection::handle_read_headers, this,
                                                          boost::asio::placeholders::error));
            }
            else
            {
                std::cout << "Error: " << err << "\n";
            }
        }

        void connection::handle_read_headers(const boost::system::error_code& err)
        {
            if (!err)
            {
                if (stopped_)
                    return;

                // Process the response headers.
                std::istream response_stream(&responseBuf_);
                std::string header;
                while (std::getline(response_stream, header) && header != "\r")
                    std::cout << header << "\n";
                std::cout << "\n";

                // Write whatever content we already have to output.
                if (responseBuf_.size() > 0)
                {
                    std::ostringstream ss;
                    ss << &responseBuf_;
                    std::string s = ss.str();
                    reply_.content.append(s);
                    // std::cout << &responseBuf_;
                }
                // Start reading remaining data until EOF.
                boost::asio::async_read(socketCli_, responseBuf_,
                                        boost::asio::transfer_at_least(1),
                                        boost::bind(&connection::handle_read_content, this,
                                                    boost::asio::placeholders::error));
            }
            else
            {
                std::cout << "Error: " << err << "\n";
                stopCli();
            }
        }

        void connection::handle_read_content(const boost::system::error_code& err)
        {
            if (!err)
            {
                if (stopped_)
                    return;
                //convert to string to send reply
                // Write all of the data that has been read so far.
                //std::cout << &responseBuf_;
                //reply_<<responseBuf_
                std::ostringstream ss;
                ss << &responseBuf_;
                std::string s = ss.str();
                reply_.content.append(s);

                // Continue reading remaining data until EOF.
                boost::asio::async_read(socketCli_, responseBuf_,
                                        boost::asio::transfer_at_least(1),
                                        boost::bind(&connection::handle_read_content, this,
                                                    boost::asio::placeholders::error));
            }
            else if (err != boost::asio::error::eof)
            {
                std::cout << "Error: " << err << "\n";
                stopCli();
            }

            else if (err == boost::asio::error::eof)
            {

                std::cout << "EOF: " << err << "\n";
                stopCli();
                do_append();
            }
        }

        void connection::stopCli()
        {

            stopped_ = true;
            //socketCli_.close();
            //deadline_.cancel();
        }


        void connection::check_deadline()
        {
            if (stopped_)
                return;

            // Check whether the deadline has passed. We compare the deadline against
            // the current time since a new asynchronous operation may have moved the
            // deadline before this actor had a chance to run.
            if (deadline_.expires_at() <= deadline_timer::traits_type::now())
            {
                std::cout<<"TIMEOUT Passed after " << TIMEOUT_INTERVAL << " seconds"<<std::endl;
                stopCli();
                // There is no longer an active deadline. The expiry is set to positive
                // infinity so that the actor takes no action until a new deadline is set.
                deadline_.expires_at(boost::posix_time::pos_infin);
                do_append();

            }

            // Put the actor back to sleep.
            deadline_.async_wait(boost::bind(&connection::check_deadline, this));
        }

    } // namespace server
} // namespace http
