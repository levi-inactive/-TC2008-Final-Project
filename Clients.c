#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#include "Rooms.h"

void main() {
	int wr_memory_id, er_memory_id;
	struct Room *waiting_room = NULL;
	struct Room *executive_room = NULL;

	// Access the shared memory Room struct we will call waiting_room.
	wr_memory_id = shmget (10, sizeof(struct Room), 0777 | IPC_CREAT);
	waiting_room = (struct Room *) shmat (wr_memory_id, (char *)0, 0);

	//waiting_room->spots = malloc(sizeof(int) * 7);

	// Access the sahred memory Room struct we will call executive_room.
	er_memory_id = shmget (20, sizeof(struct Room), 0777 | IPC_CREAT);
	executive_room = (struct Room *) shmat (er_memory_id, (char *)0, 0);

	//executive_room->spots = malloc(sizeof(int) * 2);

	int iterate;

	if (waiting_room->size > 0 && executive_room->size > 0)
		iterate = 1;

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
		// Printing to screen.	//
		// -------------------	//
	}

	shmdt((char *)waiting_room);
	shmdt((char *)executive_room);
	struct shmid_ds free;
	shmctl(wr_memory_id, IPC_RMID, &free);
	shmctl(er_memory_id, IPC_RMID, &free);
}
