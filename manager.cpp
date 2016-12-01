//
//  manager.cpp
//  webServer
//
//  Created by Amitay Blum on 11/25/16.
//  Copyright © 2016 Amitay Blum. All rights reserved.
//

#include "manager.hpp"


namespace http {
    namespace server {
        
        manager::manager()
        {
        }
        
        void manager::start(connection_ptr c)
        {
            connections_.insert(c);
            c->start();
        }
        
        void manager::stop(connection_ptr c)
        {
            connections_.erase(c);
            c->stop();
        }
        
        void manager::stop_all()
        {
            for (auto c: connections_)
                c->stop();
            connections_.clear();
        }
        
    } // namespace server
} // namespace http
