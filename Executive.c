#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "Rooms.h"

void main() {
	int wr_memory_id, er_memory_id, sem_id;
	struct Room *waiting_room = NULL;
	struct Room *executive_room = NULL;

	struct sembuf sem_op;

	// Access the shared memory Room struct we will call waiting_room.
	wr_memory_id = shmget (10, sizeof(struct Room), 0777 | IPC_CREAT);
	waiting_room = (struct Room *) shmat (wr_memory_id, (char *)0, 0);

	// Initialize waiting room with a permanent max size of 7,
	// empty and with an int array of size 7.
	waiting_room->size = 7;
	waiting_room->count = 0;
	printf("\nWaiting room initialization complete");

	// Access the shared memory Room struct we will call executive_room.
	er_memory_id = shmget (20, sizeof(struct Room), 0777 | IPC_CREAT);
	executive_room = (struct Room *) shmat (er_memory_id, (char *)0, 0);

	// Initialize executive room with a permanent max size of 2,
	// empty and with an int array of size 2.
	executive_room->size = 2;
	executive_room->count = 0;

	// Access the shared memory sempahore and turn it off.
	sem_id = semget(30, 1, 0777 | IPC_CREAT);
	semctl(sem_id, 0, SETVAL, 0);

	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;

	// First, the executive arrives to her office and always
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

		sleep(3);

		printf("\n------------------------------");

		// printing to screen. 	//
		// ------------------- 	//

		// Is the waiting room full?
		if (waiting_room->count < 7) 	wr_isnotfull = 1;
		else				wr_isnotfull = 0;

		// Is it empty?
		if (waiting_room->count == 0)	wr_isempty = 1;
		else 				wr_isempty = 0;

		// Is there any client pending to serve?
		if ((!resting && waiting_room->count > 0) || (resting && waiting_room->count > 1))
			client_pending = 1;
		else	client_pending = 0;


		if (client_pending) {
			if (resting) {
				printf("\n\n *****");
				printf("\nExecutive: Woah! Okay! Let's get back to work then.");
				printf("\n *****");

				// Exit the waiting_room.
				waiting_room->count--;

				// Enter the executive room.
				executive_room->count++;

				resting = 0;
			}

			printf("\n\n *****");
			printf("\nExecutive: Mr., you might come in...");
			printf("\n *****");

			semop(sem_id, &sem_op, 1);

			printf("\n\n *****");
			printf("\nExecutive: It was a pleasure serving you!");
			printf("\n *****");
		}

		// If there are no clients and I'm not already resting,
		// change rooms and have a coffee.
		if (wr_isempty && !resting) {
			printf("\n\n *****");
			printf("\nExecutive: Phew! What a day! I'm having a delicious cup of coffee now...");
			printf("\n *****");

			resting = 1;

			// Exit the executive_room
			executive_room->count--;

			// Enter the waiting_room
			waiting_room->count++;
		}
	}

	shmdt((char *)waiting_room);
	shmdt((char *)executive_room);

	shmctl(wr_memory_id, IPC_RMID, 0);
	shmctl(er_memory_id, IPC_RMID, 0);
}
