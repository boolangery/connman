/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2014  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef NTP_H_
#define NTP_H_

#define CONNMAN_NTP_RESPONSE_SIGNAL "ResponseReceived"

struct ntp_time {
	uint32_t seconds;
	uint32_t fraction;
} __attribute__ ((packed));


struct ntp_short {
	uint16_t seconds;
	uint16_t fraction;
} __attribute__ ((packed));


struct ntp_msg {
	uint8_t flags;			/* Mode, version and leap indicator */
	uint8_t stratum;		/* Stratum details */
	int8_t poll;			/* Maximum interval in log2 seconds */
	int8_t precision;		/* Clock precision in log2 seconds */
	struct ntp_short rootdelay;	/* Root delay */
	struct ntp_short rootdisp;	/* Root dispersion */
	uint32_t refid;			/* Reference ID */
	struct ntp_time reftime;	/* Reference timestamp */
	struct ntp_time orgtime;	/* Origin timestamp */
	struct ntp_time rectime;	/* Receive timestamp */
	struct ntp_time xmttime;	/* Transmit timestamp */
} __attribute__ ((packed));


struct ntp_query_status {
  char *timeserver;
	bool success;       /* If false, status_msg explain why */
	char *status_msg;   /* Explain the last failure */
	struct ntp_msg msg;
};

#endif  /* !NTP_H_ */
