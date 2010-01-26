/* HostsGatherer
 *
 * Copyright (C) 1999 Renaud Deraison
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string.h> /* for strdup */

#include "hosts_gatherer.h"
#include "hg_utils.h"
#include "hg_add_hosts.h"
#include "hg_subnet.h"


void
hg_gather_subnet_hosts (struct hg_globals * globals, struct hg_host * host)
{
 struct in_addr start;
 struct in_addr end;
 char hostname[1024];
 struct in6_addr in6addr;

 hg_add_subnet(globals, host->addr, host->cidr_netmask);
 start = cidr_get_first_ip(host->addr, host->cidr_netmask);
 end   = cidr_get_last_ip (start, host->cidr_netmask);
 hg_get_name_from_ip(&in6addr, hostname, sizeof(hostname));
 /** @TODO This works only for ipv4 as of now */
 start.s_addr = in6addr.s6_addr32[3];
 hg_add_host_with_options (globals, strdup(hostname), start, 1, 32, 1, &end);
}


struct in_addr
cidr_get_first_ip (struct in_addr addr, int netmask)
{
#if DANGEROUS
 struct in_addr ret;
 /* Netmask is the integer that the
  * user entered after the slash (hostname/netmask)
  *
  * 2^(32-netmask) gives us the real netmask.
  * Shifting (32-netmask) bits on the right
  * then on the left will just give us the
  * first IP. */
 ret.s_addr = ntohl(addr.s_addr);
 ret.s_addr = (ret.s_addr >> (32 - netmask));
 ret.s_addr <<= (32 - netmask);
 ret.s_addr = htonl(ret.s_addr);
 return(ret);
#else /* start at the IP provided by the user */
 return addr;
#endif
}


struct in_addr
cidr_get_last_ip (struct in_addr start, int netmask)
{
 struct in_addr ret;
 /* The last IP is the first IP plus
  * 2 ^ (32 - netmask ) - 1 */
 ret.s_addr = ntohl(start.s_addr);
 ret.s_addr >>= (32 - netmask);
 ret.s_addr++;
 ret.s_addr <<= (32 - netmask);

 if(netmask != 31)
  ret.s_addr -= 2; /* skip the broadcast */
 else
  ret.s_addr -= 1; /* skip the broadcast */

 ret.s_addr = htonl(ret.s_addr);
 return(ret);
}
