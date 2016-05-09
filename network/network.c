//FILE: network/network.c
//
//Description: this file implements SNP process  
//
//Date: April 29, 2016
//Author: Victoria Taylor (skeleton code and main() provided by Prof. Xia Zhou)

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#include <assert.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <pthread.h>
#include <unistd.h>

#include "../common/constants.h"
#include "../common/pkt.h"
#include "../topology/topology.h"
#include "network.h"

/**************************************************************/
//declare global variables
/**************************************************************/
int overlay_conn; 		//connection to the overlay


/**************************************************************/
//implementation network layer functions
/**************************************************************/


//this function is used to for the SNP process to connect to the local ON process on port OVERLAY_PORT
//connection descriptor is returned if success, otherwise return -1
int connectToOverlay() {
	int overlaysd; 
	char hostname[50];
	struct sockaddr_in overlay_addr;

	//Find local overlay host
	if (gethostname(hostname, 50) < 0){
		return -1;
	}

	//Set up address connecting to connect to
	memset(&overlay_addr, 0, sizeof(overlay_addr));
	overlay_addr.sin_port = htons(OVERLAY_PORT);
	overlay_addr.sin_addr.s_addr = topology_getIPfromname(hostname);
	overlay_addr.sin_family = AF_INET;

	overlaysd = socket(AF_INET, SOCK_STREAM, 0);
	if (overlaysd < 0){
		printf("err: socket creation failed\n");
		return -1;
	}
	if (connect(overlaysd, (struct sockaddr*)&overlay_addr, sizeof(overlay_addr))<0){
		printf("err: connect failed\n");
		return -1;
	}

	printf("Connected to overlay network\n");
	return overlaysd; 
}

//This thread sends out route update packets every ROUTEUPDATE_INTERVAL time
//In this lab this thread only broadcasts empty route update packets to all the neighbors,
// broadcasting is done by set the dest_nodeID in packet header as BROADCAST_NODEID
void* routeupdate_daemon(void* arg) {
	snp_pkt_t pkt;
	int myID = topology_getMyNodeID();
	pkt.header.src_nodeID = myID; 

	while (overlay_sendpkt(BROADCAST_NODEID, &pkt, overlay_conn) > 0){
		printf("Sent route update packet to overlay\n");
		sleep(ROUTEUPDATE_INTERVAL);
	}
	close(overlay_conn);
	overlay_conn = -100;
	pthread_exit(NULL);
}

//this thread handles incoming packets from the ON process
//It receives packets from the ON process by calling overlay_recvpkt()
//In this lab, after receiving a packet, this thread just outputs the packet received information without handling the packet 
void* pkthandler(void* arg) {
	snp_pkt_t pkt;

	while(overlay_recvpkt(&pkt,overlay_conn)>0) {
		printf("Routing: received a packet from neighbor %d\n",pkt.header.src_nodeID);
	}
	close(overlay_conn);
	overlay_conn = -100;
	pthread_exit(NULL);
}

//this function stops the SNP process 
//it closes all the connections and frees all the dynamically allocated memory
//it is called when the SNP process receives a signal SIGINT
void network_stop() {
	close(overlay_conn);
	overlay_conn = -1;
	printf("Received SIGINT\n");
	exit(0);
}


int main(int argc, char *argv[]) {
	printf("network layer is starting, pls wait...\n");

	//initialize global variables
	overlay_conn = -1;

	//register a signal handler which will kill the process
	signal(SIGINT, network_stop);

	//connect to overlay
	overlay_conn = connectToOverlay();
	if(overlay_conn<0) {
		printf("can't connect to ON process\n");
		exit(1);		
	}
	
	//start a thread that handles incoming packets from overlay
	pthread_t pkt_handler_thread; 
	pthread_create(&pkt_handler_thread,NULL,pkthandler,(void*)0);

	//start a route update thread 
	pthread_t routeupdate_thread;
	pthread_create(&routeupdate_thread,NULL,routeupdate_daemon,(void*)0);	

	printf("network layer is started...\n");

	//sleep forever
	while(1) {
		sleep(30);
		if (overlay_conn == -100){
			printf("Overlay connection failed\n");
			exit(0);
		}
	}
}


