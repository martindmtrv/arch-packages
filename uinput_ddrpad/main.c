/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>


#define VENDOR 0x1ccf // Konami Digital Entertainment
#define PRODUCT 0x1010 // DDR Mat for PS3

#include <suinput.h>

int main() {
	struct hidraw_devinfo info;
	int devices[2];

	int id = 0;
	char *hidraw = 0;

	char buf[255];
	int fd_hid, res;

	// Search for the device
	DIR *folder = opendir("/dev");
	
	struct dirent *entry;
	while ((entry = readdir(folder)))
	{
		if (!strncmp("hidraw", entry->d_name, 6))
		{
			snprintf(buf, 255, "/dev/%s", entry->d_name);
			printf("Looking at %s\n", buf);
			fd_hid = open(buf, O_RDONLY);
			if (fd_hid < 0)
			{
				printf("Checking hidraw device %s\n", buf);
				perror("Could not open hidraw device");
				continue;
			}
			
			res = ioctl(fd_hid, HIDIOCGRAWINFO, &info);
			if (res < 0)
			{
				close(fd_hid);
				continue;
			}
			
			if (info.vendor == VENDOR && info.product == PRODUCT)
			{
				printf("info matches\n");
				devices[id++] = entry->d_name[6] - '0';
			}
			else
				close(fd_hid);
		}
	}
	closedir(folder);
	
	char command[100];

	if (id == 0)
	{
		printf("Could not determine PAD hidraw device. Make sure it is connected and hidraw.ko is loaded\n");
		return 10;
	}
	else
	{
		int offset;

		offset = snprintf(command, 100, "sudo sh -c \"");

		for (int x=0; x < id; x++) {
			printf("DEVICE %d/%d: Found PAD device at /dev/hidraw%d\n", x+1, id, devices[x]);
			offset += snprintf(command+offset, 100 - offset, "uinput_ddrpad /dev/hidraw%d& ", devices[x]);
		}

		command[offset - 1] = '\"';
		system(command);
	}

}
