/*
 * $Id$
 */


#ifndef CFG_FILE_H
#define CFG_FILE_H

#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <sys/un.h>
#include "pcre.h"

#define COND_CONNECT_FLAG 0x1
#define COND_HELO_FLAG 0x2
#define COND_ENVFROM_FLAG 0x4
#define COND_ENVRCPT_FLAG 0x8
#define COND_HEADER_FLAG 0x10
#define COND_BODY_FLAG 0x20

#define MAX_SPF_DOMAINS 1024
#define MAX_CLAMAV_SERVERS 128

enum { VAL_UNDEF=0, VAL_TRUE, VAL_FALSE };
enum condition_type { 
	COND_CONNECT, 
	COND_HELO, 
	COND_ENVFROM, 
	COND_ENVRCPT,
	COND_HEADER, 
	COND_BODY, 
	COND_MAX 
};

enum action_type { 
	ACTION_REJECT, 
	ACTION_TEMPFAIL, 
	ACTION_QUARANTINE, 
	ACTION_DISCARD, 
	ACTION_ACCEPT 
};

struct action {
	enum action_type type;
	char *message;
};

struct condition {
    struct cond_arg {
    	char    *src;
    	int  empty;
    	int  not;
	    pcre  *re;
    }	args[2];
	enum condition_type type;
	LIST_ENTRY (condition) next;
};

struct rule {
	LIST_HEAD (condl, condition) *conditions;
	struct action *act;
	uint8_t flags;
	LIST_ENTRY (rule) next;
};

struct clamav_server {
	int sock_type;

	union {
		char *unix_path;
		struct {
			const char *addr_str;
			struct in_addr addr;
			uint16_t port;
		} inet;
	} sock;
	
	/* Number of requests to clamav server */
	int failed_attempts;
	/* Time in seconds when it is needed to check this server again if it is marked inactive */
	double next_check;
	/* Flag that specify whether this server active */
	char active;
};

struct config_file {
	char *pid_file;
	char *temp_dir;

	char *sock_cred;
	size_t sizelimit;
	
	struct clamav_server clamav_servers[MAX_CLAMAV_SERVERS];
	size_t clamav_servers_num;
	size_t clamav_servers_alive;

	LIST_HEAD (ruleset, rule) rules;
	
	/* Must be sorted */
	char **spf_domains;
	size_t spf_domains_num;

	char use_dcc;
};

int yylex (void);
int yyparse (void);

#endif /* ifdef CFG_FILE_H */
