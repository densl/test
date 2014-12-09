/*
 * local-usb.c
 *
 *  Created on: 2014-12-8
 *      Author: study
 */

#include <string.h>
#include <jni.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <linux/usbdevice_fs.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int g_fd = 0;
static int g_inEp = 0;
static int g_outEp = 0;
static int g_openFlag = 0;

int usb_control_jni(int fd, int requestType, int request, int value, int index, void* buffer, int length, unsigned int timeout);
int usb_bulk_jni(int fd, int endpoint, void* buffer, int length, unsigned int timeout);

jint Java_com_study_common_LocalUsb_nsetupEnv(JNIEnv* env, jobject thiz, jint fd, jint inEp, jint outEp);
void Java_com_study_common_LocalUsb_nclose(JNIEnv* env, jobject thiz);
jint Java_com_study_common_LocalUsb_nread(JNIEnv* env, jobject thiz, jbyteArray inBuffer, jint length, jint timeout);
jint Java_com_study_common_LocalUsb_nwrite(JNIEnv* env, jobject thiz, jbyteArray outBuffer, jint length, jint timeout);
jint Java_com_study_common_LocalUsb_ngetStatus();

jint Java_com_study_common_LocalUsb_nsetupEnv(JNIEnv* env, jobject thiz, jint fd, jint inEp, jint outEp)
{
	g_fd = fd > 0 ? fd : 0;
	g_inEp = inEp > 0 ? inEp : 0;
	g_outEp = outEp > 0 ? outEp : 0;


	char buffer[1024] = {0};
	memset(buffer, 0, 1024);
	int ret = usb_control_jni(g_fd, 0xa1, 0, 0, 0, buffer, 1024, 2000);
	if (ret <= 0)
		return 0;

	if (strcasestr(buffer+1, "dascom") == NULL)
		return 0;


	if (g_fd && g_inEp && g_outEp)
	{
		g_openFlag = 1;
	}
	else
	{
		g_openFlag = 0;
	}


	return g_openFlag;
}

void Java_com_study_common_LocalUsb_nclose(JNIEnv* env, jobject thiz)
{
	g_fd = 0;
	g_inEp = 0;
	g_outEp = 0;
	g_openFlag = 0;
}

jint Java_com_study_common_LocalUsb_nread(JNIEnv* env, jobject thiz, jbyteArray inBuffer, jint length, jint timeout)
{
	if (g_openFlag == 0)
		return 0;

	int ret = 0;
	char buffer[1024] = {0};
	memset(buffer, 0, 1024);
	ret = usb_bulk_jni(g_fd, g_inEp, buffer, length, timeout);
	if (ret <= 0 )
		return 0;

	jbyte *by = (jbyte*)buffer;
	(*env)->SetByteArrayRegion(env, inBuffer, 0, length, by);

	return ret;
}
jint Java_com_study_common_LocalUsb_nwrite(JNIEnv* env, jobject thiz, jbyteArray outBuffer, jint length, jint timeout)
{
	if (g_openFlag == 0)
		return 0;

	jbyte *temp = (jbyte*)( (*env)->GetByteArrayElements(env, outBuffer, 0) );

	char *buffer = (char*)temp;
	return usb_bulk_jni(g_fd, g_outEp, buffer, length, timeout);
}
jint Java_com_study_common_LocalUsb_ngetStatus()
{
	if (g_openFlag == 0)
		return 0;

	char status = 0;
	int ret = usb_control_jni(g_fd, 0xa1, 0x1, 0, 0, &status, 1, 2000);

	if (ret > 0)
		return status;
	else
		return 0;
}

int usb_bulk_jni(int fd, int endpoint, void* buffer, int length, unsigned int timeout)
{
	struct usbdevfs_bulktransfer ctrl;
	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.ep = endpoint;
	ctrl.len = length;
	ctrl.data = buffer;
	ctrl.timeout = timeout;
	return ioctl(fd, USBDEVFS_BULK, &ctrl);
}


int usb_control_jni(int fd, int requestType, int request, int value, int index, void* buffer, int length, unsigned int timeout)
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

