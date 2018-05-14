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

	//waiting_room->spots = malloc(sizeof(int) * 7);

	// Access the sahred memory Room struct we will call executive_room.
	er_memory_id = shmget (20, sizeof(struct Room), 0777 | IPC_CREAT);
	executive_room = (struct Room *) shmat (er_memory_id, (char *)0, 0);

	sem_id = semget(30, 1, 0777 | IPC_CREAT);

	// We will use sem_op later to make processes wait for the semaphore using
	// sem_op = -1.
	sem_op.sem_num = 0;
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;

	int iterate;
	int childpid;
	int masterpid = getpid();

	if (waiting_room->size > 0 && executive_room->size > 0)
		iterate = 1;

	while (iterate) {
		// Only the master process can fork more child processes.
		if (getpid() == masterpid) {
			sleep(3);

			// Fork a new process, a child from the master process.
			childpid = fork();

			if (childpid == 0) {
				childpid = getpid();

				printf("\n *****");
				printf("\nProcess %d: I fork, therefor I am.", childpid);
				printf("\n *****");

				// If there is enough room to wait...
				if (waiting_room->count < 7) {
					printf("\n ******");
					printf("\nProcess %d: A little waiting never hurt nobody.", childpid);
					printf("\n *****");

					// Enter the waiting_room...
					waiting_room->count++;

					// And wait for the executive to signal us to enter
					// the exectuive_room.
					semop(sem_id, &sem_op, 1);

					printf("\n *****");
					printf("\nProcess %d: Hello, Mr. Executive", childpid);
					printf("\n *****");

					printf("\n\n(served)");

					// Exit the waiting_room
					waiting_room->count--;
					// Enter the executive_room.
					executive_room->count++;

					sleep(1);

					// Exit the buffette.
					executive_room->count--;
				} else {
					printf("\n *****");
					printf("\nProcess %d: There wasn't enough space for me.", childpid);
					printf("\n *****");
				}
			}
		} else {
			printf("\n *****");
			printf("\nProcess %d: Thanks for your service!", getpid());
			printf("\n *****");


			shmdt((char *)waiting_room);
			shmdt((char *)executive_room);

			shmctl(wr_memory_id, IPC_RMID, 0);
			shmctl(er_memory_id, IPC_RMID, 0);

			return;
		}
	}
}
