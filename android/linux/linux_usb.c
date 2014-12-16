#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <linux/usbdevice_fs.h>

#include <linux/lp.h>

#define GET_DEVICE_ID(len) _IOC(_IOC_READ, 'P', 1, len)
#define GET_VID_PID(len) _IOC(_IOC_READ, 'P', 6, len)
#define GET_STATUS  LPGETSTATUS

int getDeviceId(int handle, char *buffer, int length);
int getStatus(int handle);


int usb_device_control_transfer_linux(int fd, int requestType, int request,
	int value, int index, void* buffer, int length, unsigned int timeout);

void main()
{
	int ret = 0;
	int fd = 0;
	int i = 0;

	char cmd_read[512] = {0};
	char cmd_write[512] = {0x1d, 0x53, 0x4c, 0x44};
	char cmd_ctrl[512] = {0};
	char buffer[512] = {0};
	
	fd = open("/dev/usb/lp0", O_RDWR);
	if (ret == -1)
		perror("open");
	else
		printf("open success.\n");	

	ret = write(fd, cmd_write, strlen(cmd_write));
	if (ret == -1)
		perror("write");
	else
		printf("write success.\n");
	

	ret = read(fd, cmd_read, 512);
	if (ret == -1)
		perror("read");
	else
		printf("read success: %X, %s | %s\n", ret, cmd_read, cmd_read+6);


	
	ret = usb_device_control_transfer_linux(fd, 0xa1, 0, 0, 0, cmd_ctrl, 500, 5000);
	if (ret == -1)
		perror("ioctl");
	else
		printf("ioctl success %X: %s", ret, cmd_ctrl);
		
	//device id
	memset(buffer, 0, 512);
	ret = getDeviceId(fd, buffer, 512);
	if (ret == -1)
		perror("ioctl");
	else
		printf("ioctl device id success %X: %s\n", ret, buffer+2);
		
	//get status
	ret = getStatus(fd);
	if (ret == -1)
		perror("ioctl");
	else
		printf("ioctl status success %02X\n", ret);	

}


int usb_device_control_transfer_linux(int fd, int requestType, int request,
	int value, int index, void* buffer, int length, unsigned int timeout)
{
	struct usbdevfs_ctrltransfer ctrl;
	memset(&ctrl, 0, sizeof(ctrl));

	ctrl.bRequestType = requestType;
	ctrl.bRequest = request;
	ctrl.wValue = value;
	ctrl.wIndex = index;
	ctrl.wLength = length;
	ctrl.data = buffer;
	ctrl.timeout = timeout;
	
	return ioctl(fd, USBDEVFS_CONTROL, &ctrl);
}

//for usb printer
int getDeviceId(int handle, char *buffer, int length)
{
	int ret = 0;
	
	ret = ioctl(handle, GET_DEVICE_ID(length), buffer);
	if (ret != 0)
		return -1;
		
	ret = buffer[0]<<8;
	ret += buffer[1];
	return ret;
}

int getStatus(int handle)
{
	int ret = 0;
	
	int status = 0;
	ret = ioctl(handle, GET_STATUS, &status);
	if (ret != 0)
		return -1;

	return status;
}
