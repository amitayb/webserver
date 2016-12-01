//
//  request.hpp
//  webServer
//
//  Created by Amitay Blum on 11/25/16.
//  Copyright © 2016 Amitay Blum. All rights reserved.
//

#ifndef request_h
#define request_h

#include <string>
#include <vector>
#include "header.hpp"

namespace http {
    namespace server {
        
        /// A request received from a client.
        struct request
        {
            std::string method;
            std::string uri;
            int http_version_major;
            int http_version_minor;
            std::vector<header> headers;
        };
        
    } // namespace server
} // namespace http


#endif /* request_h */
