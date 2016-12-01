//
//  manager.hpp
//  webServer
//
//  Created by Amitay Blum on 11/25/16.
//  Copyright © 2016 Amitay Blum. All rights reserved.
//

#ifndef manager_hpp
#define manager_hpp

#include <stdio.h>
#include <set>
#include "connection.hpp"

namespace http {
    namespace server {
        
        /// Manages open connections so that they may be cleanly stopped when the server
        /// needs to shut down.
        class manager
        {
        public:
            manager(const manager&) = delete;
            manager& operator=(const manager&) = delete;
            
            /// Construct a connection manager.
            manager();
            
            /// Add the specified connection to the manager and start it.
            void start(connection_ptr c);
            
            /// Stop the specified connection.
            void stop(connection_ptr c);
            
            /// Stop all connections.
            void stop_all();
            
        private:
            /// The managed connections.
            std::set<connection_ptr> connections_;
        };
        
    } // namespace server
} // namespace http

#endif /* manager_hpp */
