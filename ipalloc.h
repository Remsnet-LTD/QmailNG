#ifndef IPALLOC_H
#define IPALLOC_H

#include "ip.h"

#if defined(TLS_REMOTE) || defined(TLS_SMTPD)
struct ip_mx { struct ip_address ip; int pref; char *fqdn; } ;
#else
struct ip_mx { struct ip_address ip; int pref; } ;
#endif

#include "gen_alloc.h"

GEN_ALLOC_typedef(ipalloc,struct ip_mx,ix,len,a)
extern int ipalloc_readyplus(ipalloc *, unsigned int);
extern int ipalloc_append(ipalloc *, struct ip_mx *);

#endif
