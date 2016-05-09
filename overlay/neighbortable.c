//FILE: overlay/neighbortable.c
//
//Description: this file the API for the neighbor table
//
//Date: May 03, 2016
//Author: Victoria Taylor (skeleton code provided by Prof. Xia Zhou)

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "neighbortable.h"
#include "../topology/topology.h"

//This function first creates a neighbor table dynamically. It then parses the topology/topology.dat file and fill the nodeID and nodeIP fields in all the entries, initialize conn field as -1 .
//return the created neighbor table
nbr_entry_t* nt_create()
{
	int neighborNum;
	nbr_entry_t *neighborTable;
	char line[100];
	char *hostone, *hosttwo; 
	int oneID, twoID, myID;
	int count;

	// Dynamically allocate neighbor table
	neighborNum = topology_getNbrNum();
	myID = topology_getMyNodeID();

	if ((neighborTable = malloc(sizeof(nbr_entry_t) * neighborNum)) == NULL){
		printf("malloc of neighbortable failed\n");
		exit(1);
	}

	// Open topology.dat
	FILE *fd = fopen("../topology/topology.dat", "r");
	if (fd == NULL){
		printf("Can't open topology.dat\n");
		return NULL;
	}

	//Parse topology.dat
	count = 0;
	while (fgets(line, 100, fd) != NULL){
		if (line[0] != '\n'){
			hostone = strtok(line, " ");
			hosttwo = strtok(NULL, " ");

			oneID = topology_getNodeIDfromname(hostone);
			twoID = topology_getNodeIDfromname(hosttwo);

			
			
			//Check if first host is equal to myID
			if (oneID == myID){
				neighborTable[count].nodeID = twoID;
				neighborTable[count].nodeIP = topology_getIPfromname(hosttwo);
				neighborTable[count].conn = -1;
				count++;
			}
			//Check if second host is equal to myID
			else if (twoID == myID){
				neighborTable[count].nodeID = oneID;
				neighborTable[count].nodeIP = topology_getIPfromname(hostone);
				neighborTable[count].conn = -1; 
				count++;
			}
		}
	}

  return neighborTable;
}

//This function destroys a neighbortable. It closes all the connections and frees all the dynamically allocated memory.
void nt_destroy(nbr_entry_t* nt)
{
	int neighborNum = topology_getNbrNum();

	//Close all TCP connections to neighbors
	for (int i = 0; i < neighborNum; i++){
		close(nt[i].conn);
	}

	//Free neighbortable
	free(nt);
  	return;
}

//This function is used to assign a TCP connection to a neighbor table entry for a neighboring node. If the TCP connection is successfully assigned, return 1, otherwise return -1
int nt_addconn(nbr_entry_t* nt, int nodeID, int conn)
{
	int neighborNum = topology_getNbrNum();

	for (int i = 0; i < neighborNum; i++){
		if (nt[i].nodeID == nodeID){
			// Make sure connection has not already been assigned for the neighbor
			if (nt[i].conn != -1){
				printf("Connection already assigned for this neighbor\n");
				return -1;
			}

			// If no connection has been established, assign the passed connection
			nt[i].conn = conn; 
			return 1; 
		}
	}
  return -1; 
}
