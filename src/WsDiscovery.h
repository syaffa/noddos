/*
   Copyright 2017 Steven Hessing

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 * WsDiscovery.h
 *
 *  Created on: September 9, 2017
 *      Author: Steven Hessing (steven.hessing@gmail.com)
 */

#ifndef WSDISCOVERY_H_
#define WSDISCOVERY_H_

#include "iDeviceInfoSource.h"
#include "HostCache.h"
#include "noddos.h"

#include <regex>
#include <string>
#include <ctime>
#include <memory>
#include <sys/epoll.h>

struct WsDiscoveryHost {
    std::string IpAddress;

    //! SsdpHost comparison only compares info from the WsDiscovery messages, not the host info.
    bool operator == (const SsdpHost &rhs) const {
        return IpAddress == rhs.IpAddress;
    }
};

class WsDiscovery : public iDeviceInfoSource {
private:
    int socket_fd;
    std::string IpAddress;
    time_t Expiration;
    HostCache &hCache;
    bool Debug;
    std::regex wsdiscovery_rx;
    time_t nextProbe = 0;

public:
    WsDiscovery(HostCache &inhCache, const time_t inExpiration, const std::string inIpAddress = "", const bool inDebug = false):
            hCache{inhCache}, Expiration{inExpiration}, IpAddress{inIpAddress}, Debug{inDebug} {
        socket_fd = -1;
        wsdiscovery_rx = std::regex(R"delim(^(SERVER|LOCATION|NT|USN|USER-AGENT): (.*)$)delim",
                std::regex_constants::ECMAScript | std::regex_constants::icase | std::regex_constants::optimize);

        Open (IpAddress);

    }
    ~WsDiscovery() {
        // TODO Auto-generated destructor stub
    }
    int Open(std::string input, uint32_t inExpiration = 0);
    bool Close();
    bool processEvent(struct epoll_event &event);
    int getFileHandle();
    bool Probe ();

    bool ParseWsDiscoveryMessage (std::shared_ptr<WsDiscoveryHost> host, const char * msgbuf, const int nbytes);

};

#endif /* WSDISCOVERY_H_ */
