/**
 * @file
 * Stack-internal timers implementation.
 * This file includes timer callbacks for stack-internal timers as well as
 * functions to set up or stop timers and check for expired timers.
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *         Simon Goldschmidt
 *
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip4_frag.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/nd6.h"
#include "lwip/ip6_frag.h"
#include "lwip/mld6.h"
#include "lwip/sys.h"

/** The one and only timeout list */
static struct sys_timeo *next_timeout;

/** The reference_time is a time which is never in the future and
 *  which is never after any timeout on the list. */
static u32_t reference_time;

#if LWIP_TCP
static struct sys_timeo tcpip_tcp_timeo;

/** global variable that shows if the tcp timer is currently scheduled or not */
static int tcpip_tcp_timer_active;

/**
 * Timer callback function that calls tcp_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void tcpip_tcp_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_ASSERT("tcpip_tcp_timer_active", tcpip_tcp_timer_active);

  /* call TCP timer handler */
  tcp_tmr();
  /* timer still needed? */
  if (tcp_active_pcbs || tcp_tw_pcbs) {
    /* restart timer */
    sys_timeout(&tcpip_tcp_timeo, TCP_TMR_INTERVAL, tcpip_tcp_timer, NULL);
  } else {
    /* disable timer */
    tcpip_tcp_timer_active = 0;
  }
}

/**
 * Called from TCP_REG when registering a new PCB:
 * the reason is to have the TCP timer only running when
 * there are active (or time-wait) PCBs.
 */
void tcp_timer_needed(void)
{
  /* timer is off but needed again? */
  if (!tcpip_tcp_timer_active && (tcp_active_pcbs || tcp_tw_pcbs)) {
    /* enable and start timer */
    tcpip_tcp_timer_active = 1;
    sys_timeout(&tcpip_tcp_timeo, TCP_TMR_INTERVAL, tcpip_tcp_timer, NULL);
  }
}
#endif /* LWIP_TCP */

#if LWIP_IPV4
#if IP_REASSEMBLY
static struct sys_timeo ip_reass_timeo;

/**
 * Timer callback function that calls ip_reass_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void ip_reass_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  ip_reass_tmr();
  sys_timeout(&ip_reass_timeo, IP_TMR_INTERVAL, ip_reass_timer, NULL);
}
#endif /* IP_REASSEMBLY */

#if LWIP_ARP
static struct sys_timeo arp_timeo;

/**
 * Timer callback function that calls etharp_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void arp_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  etharp_tmr();
  sys_timeout(&arp_timeo, ARP_TMR_INTERVAL, arp_timer, NULL);
}
#endif /* LWIP_ARP */

#if LWIP_DHCP
static struct sys_timeo dhcp_coarse_timeo;
static struct sys_timeo dhcp_fine_timeo;

/**
 * Timer callback function that calls dhcp_coarse_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void dhcp_timer_coarse(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  dhcp_coarse_tmr();
  sys_timeout(&dhcp_coarse_timeo, DHCP_COARSE_TIMER_MSECS, dhcp_timer_coarse, NULL);
}

/**
 * Timer callback function that calls dhcp_fine_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void dhcp_timer_fine(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  dhcp_fine_tmr();
  sys_timeout(&dhcp_fine_timeo, DHCP_FINE_TIMER_MSECS, dhcp_timer_fine, NULL);
}
#endif /* LWIP_DHCP */

#if LWIP_IGMP
static struct sys_timeo igmp_timeo;

/**
 * Timer callback function that calls igmp_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void igmp_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  igmp_tmr();
  sys_timeout(&igmp_timeo, IGMP_TMR_INTERVAL, igmp_timer, NULL);
}
#endif /* LWIP_IGMP */
#endif /* LWIP_IPV4 */

#if LWIP_DNS
static struct sys_timeo dns_timeo;

/**
 * Timer callback function that calls dns_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void dns_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  dns_tmr();
  sys_timeout(&dns_timeo, DNS_TMR_INTERVAL, dns_timer, NULL);
}
#endif /* LWIP_DNS */

#if LWIP_IPV6
static struct sys_timeo nd6_timeo;

/**
 * Timer callback function that calls nd6_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void nd6_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  nd6_tmr();
  sys_timeout(&nd6_timeo, ND6_TMR_INTERVAL, nd6_timer, NULL);
}

#if LWIP_IPV6_REASS
static struct sys_timeo ip6_reass_timeo;

/**
 * Timer callback function that calls ip6_reass_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void ip6_reass_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  ip6_reass_tmr();
  sys_timeout(&ip6_reass_timeo, IP6_REASS_TMR_INTERVAL, ip6_reass_timer, NULL);
}
#endif /* LWIP_IPV6_REASS */

#if LWIP_IPV6_MLD
static struct sys_timeo mld6_timeo;

/**
 * Timer callback function that calls mld6_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void mld6_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  mld6_tmr();
  sys_timeout(&mld6_timeo, MLD6_TMR_INTERVAL, mld6_timer, NULL);
}
#endif /* LWIP_IPV6_MLD */
#endif /* LWIP_IPV6 */

/** Initialize this module */
void sys_timeouts_init(void)
{
#if LWIP_IPV4
#if IP_REASSEMBLY
  sys_timeout(&ip_reass_timeo, IP_TMR_INTERVAL, ip_reass_timer, NULL);
#endif /* IP_REASSEMBLY */
#if LWIP_ARP
  sys_timeout(&arp_timeo, ARP_TMR_INTERVAL, arp_timer, NULL);
#endif /* LWIP_ARP */
#if LWIP_DHCP
  sys_timeout(&dhcp_coarse_timeo, DHCP_COARSE_TIMER_MSECS, dhcp_timer_coarse, NULL);
  sys_timeout(&dhcp_fine_timeo, DHCP_FINE_TIMER_MSECS, dhcp_timer_fine, NULL);
#endif /* LWIP_DHCP */
#if LWIP_IGMP
  sys_timeout(&igmp_timeo, IGMP_TMR_INTERVAL, igmp_timer, NULL);
#endif /* LWIP_IGMP */
#endif /* LWIP_IPV4 */
#if LWIP_DNS
  sys_timeout(&dns_timeo, DNS_TMR_INTERVAL, dns_timer, NULL);
#endif /* LWIP_DNS */
#if LWIP_IPV6
  sys_timeout(&nd6_timeo, ND6_TMR_INTERVAL, nd6_timer, NULL);
#if LWIP_IPV6_REASS
  sys_timeout(&ip6_reass_timeo, IP6_REASS_TMR_INTERVAL, ip6_reass_timer, NULL);
#endif /* LWIP_IPV6_REASS */
#if LWIP_IPV6_MLD
  sys_timeout(&mld6_timeo, MLD6_TMR_INTERVAL, mld6_timer, NULL);
#endif /* LWIP_IPV6_MLD */
#endif /* LWIP_IPV6 */
}

/**
 * Create a one-shot timer (aka timeout).
 * Timeouts are processed by calling sys_check_timeouts().
 *
 * @param timeout the sys_timeo, must not be already running
 * @param msecs time in milliseconds after that the timer should expire
 * @param handler callback function to call when msecs have elapsed
 * @param arg argument to pass to the callback function
 */
#if LWIP_DEBUG_TIMERNAMES
void sys_timeout_debug(struct sys_timeo *timeout, u32_t msecs, sys_timeout_handler handler, void *arg, const char* handler_name)
#else
void sys_timeout      (struct sys_timeo *timeout, u32_t msecs, sys_timeout_handler handler, void *arg)
#endif
{
  u32_t now;
  struct sys_timeo *prev_t, *t;

  LWIP_ASSERT("timeout != NULL", timeout != NULL);
  LWIP_ASSERT("handler != NULL", handler != NULL);
  
  now = sys_now();

  timeout->handler = handler;
  timeout->arg = arg;
  timeout->time = now + msecs;
  
#if LWIP_DEBUG_TIMERNAMES
  timeout->handler_name = handler_name;
  LWIP_DEBUGF(TIMERS_DEBUG, ("sys_timeout: %p msecs=%"U32_F" handler=%s arg=%p\n",
    (void *)timeout, msecs, handler_name, (void *)arg));
#endif

  /* skip any timers which need to expire before this one */
  prev_t = NULL;
  t = next_timeout;
  while (t != NULL && (u32_t)(t->time - reference_time) < (u32_t)(timeout->time - reference_time)) {
    prev_t = t;
    t = t->next;
  }
  
  /* if this becomes the only timer, set the reference time to now */
  if (next_timeout == NULL) {
    reference_time = now;
  }
  
  /* insert the new timer after any skipped timers */
  if (prev_t == NULL) {
    timeout->next = next_timeout;
    next_timeout = timeout;
  } else {
    timeout->next = t;
    prev_t->next = timeout;
  }
}

/**
 * Handle timeouts.
 * Must be called periodically from your main loop.
 * 
 * @param max_timers_to_handle maximum number of timeouts to handle
 */
void sys_check_timeouts(u8_t max_timeouts_to_handle)
{
  u32_t now;
  struct sys_timeo *timeout;
  
  while (next_timeout != NULL && max_timeouts_to_handle > 0) {
    now = sys_now();
    
    if ((u32_t)(now - reference_time) < (u32_t)(next_timeout->time - reference_time)) {
      reference_time = now;
      break;
    }
    
    timeout = next_timeout;
    
    next_timeout = timeout->next;
    reference_time = timeout->time;
    max_timeouts_to_handle--;
    
#if LWIP_DEBUG_TIMERNAMES
    LWIP_DEBUGF(TIMERS_DEBUG, ("sct calling h=%s arg=%p\n", timeout->handler_name, timeout->arg));
#endif
    
    timeout->handler(timeout->arg);
  }
}

/**
 * Get the time when the next timeout is about to expire.
 * 
 * @param out pointer where to write the time
 * @return 1 on success (*out was written), 0 when there are no timeouts
 */
u8_t sys_timeouts_nextime(u32_t *out)
{
  if (next_timeout == NULL) {
    return 0;
  }
  *out = next_timeout->time;
  return 1;
}
