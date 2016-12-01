//
//  header.hpp
//  webServer
//
//  Created by Amitay Blum on 11/25/16.
//  Copyright © 2016 Amitay Blum. All rights reserved.
//

#ifndef header_h
#define header_h

#include <string>

namespace http {
    namespace server {
        
        struct header
        {
            std::string name;
            std::string value;
        };
        
    } // namespace server
} // namespace http


#endif /* header_h */
