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
#define MAX_BUF 64

int keepgoing = 1;
int MotorValues[4];

void signal_handler (int sig);

void signal_handler (int sig) {
	printf("The car stops!\n");
	printf("Exit the programming!\n");
	keepgoing = 0;
}

int analogIn (char *ain);

int analogIn (char *ain) {
	FILE *fp;
	char ainPath[MAX_BUF];
	char ainVal[MAX_BUF];
	
	snprintf(ainPath, sizeof ainPath, "/sys/devices/ocp.2/helper.14/%s", ain);

	if ((fp = fopen(ainPath, "r")) == NULL) {
		printf("Can't open this pin, %s\n", ain);
		return 1;
	}

	fgets(ainVal, MAX_BUF, fp);

	fclose(fp);
	return atoi(ainVal);		
}

void delay (unsigned int loops) {
	int i, j;
#ifdef HACK
	for (i = 0; i < loops; i++)
		for (j = 0; j < 50000; j++);
#else
    usleep(1000*loops);
#endif
}

int main (int argc, char **argv) {
	printf("The car start!!!\n");
	
	signal(SIGINT, signal_handler);

	struct pollfd fdset[4];

	int MotorCotrolPins[] = MOTOR_CONTROL_PINS;
	int gpio_fd[4];
	
	char Fornt_Sensor[] = "AIN6";
	int Fornt_Sensor_Value;

	int i;
	for (i = 0; i < 4; i++) {
		gpio_export(MotorCotrolPins[i]);
		gpio_set_dir(MotorCotrolPins[i], "out");
		gpio_fd[i] = gpio_fd_open(MotorCotrolPins[i], O_RDONLY);
		MotorValues[i] = 0;
	}
	
	int timeout = POLL_TIMEOUT;
	
	printf("The car is going forword!\n");

	while (keepgoing) {
		Fornt_Sensor_Value = analogIn(Fornt_Sensor);
		delay(100);
		printf("Fornt Sensor Value: %d\n", Fornt_Sensor_Value);
		if (Fornt_Sensor_Value > 400 && Fornt_Sensor_Value < 1000) {
			for (i = 0; i < 4; i++) {
				MotorValues[i] = 0;
			}
		} else if (Fornt_Sensor_Value < 400) {
			MotorValues[0] = 1;
			MotorValues[1] = 0;
			MotorValues[2] = 1;
			MotorValues[3] = 0;
		} else if (Fornt_Sensor_Value > 1000) {
			MotorValues[0] = 0;
			MotorValues[1] = 1;
			MotorValues[2] = 0;
			MotorValues[3] = 1;
		}
		delay(100);
		
		for (i = 0; i < 4; i++) {
			gpio_set_value(MotorCotrolPins[i], MotorValues[i]);
		}
	}

	for (i = 0; i < 4; i++) {
		gpio_set_value(MotorCotrolPins[i], 0);
		gpio_fd_close(gpio_fd[i]);
	}
	return 0;
}
