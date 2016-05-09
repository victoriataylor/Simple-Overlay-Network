//FILE: topology/topology.c
//
//Description: this file implements some helper functions used to parse 
//the topology file 
//
//Date: May 3, 2016
//Author: Victoria Taylor (skeleton code provided by Prof. Xia Zhou)

#define _DEFAULT_SOURCE

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "topology.h"
#include "../common/constants.h"

//this function returns node ID of the given hostname
//the node ID is an integer of the last 8 digit of the node's IP address
//for example, a node with IP address 202.120.92.3 will have node ID 3
//if the node ID can't be retrieved, return -1
int topology_getNodeIDfromname(char* hostname) 
{
	int nodeID;
	struct sockaddr_in addr; 

	struct hostent *host = gethostbyname(hostname);
	if (host == NULL){
		return -1;
	}
	memcpy(&(addr.sin_addr), host->h_addr_list[0], host->h_length);
	nodeID = topology_getNodeIDfromip(&addr.sin_addr);

	return nodeID;
}

in_addr_t topology_getIPfromname(char* hostname){
	struct hostent *host;
	struct sockaddr_in addr; 

	host = gethostbyname(hostname);

	memcpy(&(addr.sin_addr), host->h_addr_list[0], host->h_length);
	return addr.sin_addr.s_addr;

}

//this function returns node ID from the given IP address
//if the node ID can't be retrieved, return -1
int topology_getNodeIDfromip(struct in_addr* addr)
{
	strtok(inet_ntoa(*addr), ".");
	strtok(NULL, ".");
	strtok(NULL, ".");
	char *ip = strtok(NULL, ".");
	if (ip != NULL) {
		return atoi(ip);
	}
	else {
		return -1; 
	}
	
}

//this function returns my node ID
//if my node ID can't be retrieved, return -1
int topology_getMyNodeID()
{
	int myID;
	char hostname[50];
	if (gethostname(hostname, 50) < 0 ){
		return -1;
	}
	myID = topology_getNodeIDfromname(hostname);

	return myID; 
}

//this functions parses the topology information stored in topology.dat
//returns the number of neighbors
int topology_getNbrNum()
{
	char line[100];
	char *hostone, *hosttwo;
	int neighbors = 0; 
	int myID = topology_getMyNodeID();

	// Open topology.dat
	FILE *fd = fopen("../topology/topology.dat", "r");
	if (fd == NULL){
		printf("File can't be opened\n");
		return -1;
	}

	// Count neighbors
	while (fgets(line, 100, fd) != NULL){
		if (line[0] != '\n'){
			hostone = strtok(line, " ");
			hosttwo = strtok(NULL, " ");
			if ((topology_getNodeIDfromname(hostone) == myID) || (topology_getNodeIDfromname(hosttwo)) == myID){
				neighbors++;
			}
		}
	}

	//Close file and return number of neighbors
	fclose(fd);
	return neighbors;
}

//this functions parses the topology information stored in topology.dat
//returns the number of total nodes in the overlay 
int topology_getNodeNum()
{
	int nodes[50] = {0};
	int nodecount;
	int oneInTable, twoInTable;
	char *hostone, *hosttwo;
	int oneID, twoID;
	FILE *fp; 
	char line[100];

	//Open topology.dat
	fp = fopen("../topology/topology.dat", "r");
	if (fp == NULL){
		printf("Could not open topology.dat\n");
		return -1; 
	}

	// parse each line and get the nodeIDs
	nodecount = 0;
	while (fgets(line, 100, fp) != NULL){
		if (line[0] != '\n'){
			
			//Pull node IDs from topology.dat
			hostone = strtok(line, " ");
			hosttwo = strtok(NULL, " ");
			oneID = topology_getNodeIDfromname(hostone); 
			twoID = topology_getNodeIDfromname(hosttwo);
			
			//Reset flags
			oneInTable = 0; 
			twoInTable = 0;

			// Check if nodes are already in the list
			int i = 0;
			while (nodes[i] != 0){
				if (nodes[i] == oneID){
					oneInTable = 1;
				}
				if (nodes[i] == twoID){
					twoInTable = 1;
				}
				i++;
			}

			//If nodes aren't in list- add them and update count
			if (!oneInTable){
				nodes[nodecount] = oneID;
				nodecount++;
			}
			if (!twoInTable){
				nodes[nodecount] = twoID;
				nodecount++;
			}
		}
	}

	//return total number of nodes
	fclose(fp);
	return nodecount;
}



//this functions parses the topology information stored in topology.dat
//returns the cost of the direct link between the two given nodes 
//if no direct link between the two given nodes, INFINITE_COST is returned
unsigned int topology_getCost(int fromNodeID, int toNodeID)
{
	char *hostone, *hosttwo,  *cost;
	int oneID, twoID;
	char line[100];

	FILE *fp = fopen("../topology/topology.dat", "r");
	if (fp == NULL){
		printf("Could not open topology.dat\n");
		return -1; 
	}

	while (fgets(line, 100, fp) != NULL){
		if (line[0] != '\n'){
			hostone = strtok(line, " ");
			hosttwo = strtok(NULL, " ");
			cost = strtok(NULL, " ");

			printf("hostone = %s, hosttwo= %s\n", hostone, hosttwo);
			oneID = topology_getNodeIDfromname(hostone);
			twoID = topology_getNodeIDfromname(hosttwo);

			//Return the cost of a direct link
			if (((oneID == fromNodeID) && (twoID == toNodeID)) || ((oneID == toNodeID) && (twoID == fromNodeID))){
				return atoi(cost); 
			}
		}
	}

	//No match found
	return INFINITE_COST;
}
