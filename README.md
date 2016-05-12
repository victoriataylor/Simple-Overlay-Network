## Simple Overlay Network
#### Program Description: 
Simple Overlay Network is a program written in C that implements Internet routers on edge machines and then creates a network of overlay nodes based neighboring connections. The overlay and network layers are both implemented as processes on each node in the overlay network

#### Contents
* topology 
 * topology.dat - list of neighbors and edge costs in format (host1 host2 cost) <space>
 * topology.c - helper functions to parse the topology.dat file
 * topology.h - topology header file
* overlay
 * neighbortable.c - API for the neighbortable
 * neighbortable.h - header file for neighbortable
 *	overlay.c - overlay network source file
 *	overlay.h - overlay network header file
* network
 *	network.c - simple network protocol process source file
 *	network.h - simple network protocol header file
* common 
 *	pkt.c - packet source file
 *	pkt.h - packet header file
 *	constants.h -constants used by SRT

**The 4 hosts used to build overlay network are**: 
 * tahoe.cs.dartmouth.edu
 * wildcat.cs.dartmouth.edu
 * stowe.cs.dartmouth.edu
 * flume.cs.dartmouth.edu

The overlay process should be running on these four hosts. If you wish to run on different hosts, replace these hosts in topology/topology.dat

#### Running the program
0. goto overlay directory and run ./overlay on each node
0. Give 1 minute for all overlay processes to start and connect
0. Wait until overlays print "Overlay: waiting for connection to start network layer on a host"
0. goto network directory and run ./network on each node 

