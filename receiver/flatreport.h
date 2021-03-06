#ifndef FLATREPORT_H
#define FLATREPORT_H 1

#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include "hashmap.h"
#include "triggertable2.h"
#include "flowentry.h"
#include "ddostable2.h"
#include "client.h"

#define ANYMATCH_MAX 32

struct flatreport_pkt{
	struct flow f;
	uint32_t ts;
	uint32_t seq;
	uint32_t ack;
	uint32_t hash;
	uint16_t length;
	bool sameaslast;
};

struct flatreport{
//per packet
	uint32_t stat_pktnum;
	uint32_t step; //epoch number
	uint8_t pkt_q; //how many packets are in the queue for cache prefetching
	bool lastpktisddos;
	struct ddostable2 * dt;
	struct flowentry * last_flowentry;
	struct hashmap * ft1;
	struct summary_table * st;
	uint64_t epoch_ts; //the base timestamp for whole packets in a batch
	struct flatreport_pkt pkts [FLATREPORT_PKT_BURST];
//per match
	struct summary * pktnum_summary; //the summary that keeps track of # packets per flow
	struct triggertable * tt;
	uint64_t stat_matchdelay;
	uint32_t stat_flownum;
	uint8_t anymatchfilternum;
	struct flow anymatchfilter[ANYMATCH_MAX];  
	struct flow anymatchfiltermask[ANYMATCH_MAX];  
////////////////////////////////sweep	
	bool sweepfinished;
	bool sweep_removedentry; //if last sweep step over flowtable, removed an entry
	uint32_t ticksperentrysweep; //if we sweep flowtable, how long it took to sweep each entry in average
	uint32_t minsweepticks;
	struct hashmap_iterator sweep_iterator;
//rare
// report	
	struct client * c;
//init	
	struct lossfinder * lf;

};

/*
* prefetches the corresponding entry in the flow table into cache
*/
void flatreport_readpacket_prefetch(struct flatreport * fr, struct flatreport_pkt * pkt);

/*
* processes a batch of packets same as flatreport_process
*/
void flatreport_batchprocess(struct flatreport * fr);
void flatreport_process(struct flatreport * fr, struct flatreport_pkt * pkt);

/*
* Start the periodic phase
*/
void flatreport_startsweep(struct flatreport * fr);

/*
* Run a step for the periodic step
*/
void flatreport_sweep(struct flatreport * fr, uint64_t sweeptime, uint64_t start);

/*
* Are we done with the periodic phase?
*/
bool flatreport_issweepfinished(struct flatreport * fr);

/*
* The default function to add a set of triggers at the server without the controller
*/
void flatreport_addtriggers(struct flatreport * fr, uint16_t trigger_num, uint16_t trigger_perpkt, uint16_t patterns, struct triggertype ** types, int types_num);

/*
* Add the default types 
*/
void flatreport_addtypes(struct flatreport * fr, struct triggertype ** types, int num);

/*
* Get the flow entry in the flow table that matches the flow or return null
*/
struct flowentry * flatreport_getflowentry(struct flatreport * fr, struct flow * f);
void flatreport_setminsweepticks(struct flatreport * fr, uint64_t minsweepticks);

/*
* Remove the entry from the flow table and reset its memory
*/
bool flatreport_flowentry_finish(void * data, void * aux);

/*
* Send trigger satisfaction for a trigger right now
*/
void flatreport_report(struct flatreport * fr, struct trigger * t);

/*
* Find all flow table entries that match a trigger!
*/
void flatreport_matchforatrigger(struct flatreport * fr, struct trigger * t);

/*
* Just the naive sweep assuming that trigger stats are updated (for packet history strawman)
*/
void flatreport_naivesweep(struct flatreport * fr);

/*
* process packet history as a strawman
*/
void flatreport_historyprocess(struct flatreport * fr);

struct summary_table * flatreport_getsummarytable(struct flatreport * fr);

void flatreport_profilematching(struct flatreport * fr);

// a bunch of different ways to define triggers (use to profile matching)
void flatreport_makenotmatchingtriggers(struct flatreport * fr, uint32_t triggernum, uint32_t patterns, struct triggertype * type);
void flatreport_makeallpatternsmatchingtriggers(struct flatreport * fr, uint32_t triggernum, uint32_t patterns, struct triggertype * type);
void flatreport_makeperpktmatchingtriggers(struct flatreport * fr, uint32_t triggernum, uint32_t patterns, struct triggertype * type);
void flatreport_makeperpktpatterntriggers(struct flatreport * fr, uint32_t triggernum, uint32_t patterns, struct triggertype * type);
void flatreport_makeallmatchingtriggers(struct flatreport * fr, uint32_t triggernum, struct triggertype * type);

/*
* Client c can be null
*/
struct flatreport * flatreport_init(struct ddostable2 * dt, struct client * c);
void flatreport_finish(struct flatreport * fr);

#endif /* flatreport.h */
