#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#include "Rooms.h"

void serve_client();

void main() {
	int wr_memory_id, er_memory_id;
	struct Room *waiting_room = NULL;
	struct Room *executive_room = NULL;

	// Access the shared memory Room struct we will call waiting_room.
	wr_memory_id = shmget (10, sizeof(struct Room), 0777 | IPC_CREAT);
	waiting_room = (struct Room *) shmat (wr_memory_id, (char *)0, 0);

	// Initialize waiting room with a permanent max size of 7,
	// empty and with an int array of size 7.
	waiting_room->size = 7;
	waiting_room->count = 0;
	int *ptr_wrspots = malloc(sizeof(int) * 7);
	waiting_room->spots = (int *)ptr_wrspots;

	// Access the shared memory Room struct we will call executive_room.
	er_memory_id = shmget (20, sizeof(struct Room), 0777 | IPC_CREAT);
	executive_room = (struct Room *) shmat (er_memory_id, (char *)0, 0);

	// Initialize executive room with a permanent max size of 2,
	// empty and with an int array of size 2.
	executive_room->size = 2;
	executive_room->count = 0;
	int *ptr_erspots = malloc(sizeof(int) * 2);
	executive_room->spots = (int *)ptr_erspots;

	// First, the executive arrives to her office and always
	// sits in the [0] spot.
	int executive_id = getpid();
	executive_room->spots[0] = executive_id;
	executive_room->count++;

	int wr_isnotfull = 0;
	int wr_isempty = 0;
	int client_pending = 0;
	int resting = 0;

	int iterate = 1;

	while (iterate) {
		// -------------------	//
		// Printing to screen.	//
		printf("\n\nWaiting room count: %d\n", waiting_room->count);
		printf("Executive room count: %d\n", executive_room->count);

		printf("\nPeople in the waiting room: ");
		for (int i = 0; i < waiting_room->size; i++) {
			printf("\n\t%d", waiting_room->spots[i]);
		}

		printf("\n\nPeople in the executive room: ");
		for (int i = 0; i < executive_room->size; i++) {
			printf("\n\t%d", executive_room->spots[i]);
		}

		sleep(3);

		printf("\n------------------------------");

		// printing to screen. 	//
		// ------------------- 	//

		// Is the waiting room full?
		if (waiting_room->count < 7) 	wr_isnotfull = 1;
		else				wr_isnotfull = 0;

		// Is it empty?
		if (waiting_room->count == 0)	wr_isempty = 1;

		// Is there any client pending to serve?
		if ((!resting && waiting_room->count > 0) || (resting && waiting_room->count > 1))
			client_pending = 1;
		else	client_pending = 0;


		if (client_pending) {
			if (resting) {
				printf("\n\n *****");
				printf("\nExecutive: Woah! Okay! Let's get back to work then.");
				printf("\n *****");

				// If I was resting, don't.
				for (int i = 0; i < waiting_room->size; i++) {
					if (waiting_room->spots[i] == executive_id) {
						executive_room->spots[0] = executive_id;
						executive_room->count++;

						waiting_room->spots[i] = 0;
						waiting_room->count--;
					}
				}

				resting = 0;
			}

			serve_client();
		}

		// If there are no clients and I'm not already resting,
		// change rooms and have a coffee.
		if (wr_isempty && !resting) {
			printf("\n\n *****");
			printf("\nExecutive: Phew! What a day! I'm having a delicious cup of coffee now...");
			printf("\n *****");

			resting = 1;

			waiting_room->spots[waiting_room->count] = executive_room->spots[0];
			waiting_room->count++;

			executive_room->spots[0] = 0;
			executive_room->count--;
		}
	}

	shmdt((char *)waiting_room);
	shmdt((char *)executive_room);
	struct shmid_ds free;
	free.shm_nattch = 0;
	shmctl(wr_memory_id, IPC_RMID, &free);
	shmctl(er_memory_id, IPC_RMID, &free);
}

void serve_client() {
	printf("\n\n *****");
	printf("\nExecutive: Sir/Miss, you might come in...");
	printf("\n *****");

	// Tell client to come in
	// wait without sleeping

	printf("\n\n *****");
	printf("\nExecutive: It was a pleasure serving you!");
	printf("\n *****");
}
