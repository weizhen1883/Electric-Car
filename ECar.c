#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include "gpio-utils.h"

#define POLL_TIMEOUT 10000
#define MOTOR_CONTROL_PINS {30, 60, 31, 50}

int keepgoing = 1;
int MotorValues[4];

void signal_handler (int sig);

void signal_handler (int sig) {	
	printf("The car stops!\n");
	printf("Exit the programming!\n");
	keepgoing = 0;
}

int main (int argc, char **argv) {
	printf("The car start!!!\n");

	signal(SIGINT, signal_handler);
	
	struct pollfd fdset[4];

	int MotorCotrolPins[] = MOTOR_CONTROL_PINS;
	int gpio_fd[4];

	int i;
	for (i = 0; i < 4; i++) {
		gpio_export(MotorCotrolPins[i]);
		gpio_set_dir(MotorCotrolPins[i], "out");
		gpio_fd[i] = gpio_fd_open(MotorCotrolPins[i], O_RDONLY);
		MotorValues[i] = 0;
	}
	
	int timeout = POLL_TIMEOUT;
	
	while (keepgoing) {
		char dir;
		scanf("%c", &dir);
		switch (dir) {
			case 'w':
				//go forword
				MotorValues[0] = 1;
				MotorValues[1] = 0;
				MotorValues[2] = 1;
				MotorValues[3] = 0;
				printf("The car is going forword!\n");
				break;
			case 'x':
				//go back
				MotorValues[0] = 0;
				MotorValues[1] = 1;
				MotorValues[2] = 0;
				MotorValues[3] = 1;
				printf("The car is going back!\n");
				break;
			case 'a':
				//turn left
				MotorValues[0] = 1;
				MotorValues[1] = 0;
				MotorValues[2] = 0;
				MotorValues[3] = 1;
				printf("The car is turing left!\n");
				break;
			case 'd':
				//turn right
				MotorValues[0] = 0;
				MotorValues[1] = 1;
				MotorValues[2] = 1;
				MotorValues[3] = 0;
				printf("The car is turing right!\n");
				break;
			case 's':
				//stop
				for (i = 0; i < 4; i++) {
					MotorValues[i] = 0;
				}
				printf("The car stops!\n");
				break;
		}

		for (i = 0; i < 4; i++) {
			gpio_set_value(MotorCotrolPins[i], MotorValues[i]);
		}

		fflush(stdout);
	}

	for (i = 0; i < 4; i++) {
		gpio_set_value(MotorCotrolPins[i], 0);
		gpio_fd_close(gpio_fd[i]);
	}

	return 0;
}
