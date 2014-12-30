/*
** Copyright 2008, Google Inc.
** Copyright (c) 2009, Code Aurora Forum.All rights reserved.
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
*/

#ifndef RPC_IOCTL_H
#define RPC_IOCTL_H

#include <linux/ioctl.h>

struct rpcrouter_ioctl_server_args {
        uint32_t prog;
        uint32_t vers;
};

#define RPC_ROUTER_IOCTL_MAGIC (0xC1)

#define RPC_ROUTER_IOCTL_GET_VERSION \
        _IOR(RPC_ROUTER_IOCTL_MAGIC, 0, unsigned int)

#define RPC_ROUTER_IOCTL_GET_MTU \
        _IOR(RPC_ROUTER_IOCTL_MAGIC, 1, unsigned int)

#define RPC_ROUTER_IOCTL_REGISTER_SERVER \
        _IOWR(RPC_ROUTER_IOCTL_MAGIC, 2, unsigned int)

#define RPC_ROUTER_IOCTL_UNREGISTER_SERVER \
        _IOWR(RPC_ROUTER_IOCTL_MAGIC, 3, unsigned int)

#define RPC_ROUTER_IOCTL_CLEAR_NETRESET \
  _IOWR(RPC_ROUTER_IOCTL_MAGIC, 4, unsigned int)

#endif /* RPC_IOCTL_H */
