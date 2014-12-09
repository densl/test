/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>

#include <fcntl.h>
#include <stdio.h>

#include <errno.h>
#include <linux/usbdevice_fs.h>
#include <sys/ioctl.h>
#include <unistd.h>

void setLog(const char *fmt, ...);
void setLogStr(char *buf);
int usb_device_control_transfer_jni(int fd, int requestType, int request, int value, int index, void* buffer, int length, unsigned int timeout);
int usb_device_bulk_transfer_jni(int fd, int endpoint, void* buffer, int length, unsigned int timeout);



jstring
Java_com_example_hellojni_HelloJni_dealwithUsb( JNIEnv* env,
                                                  jobject thiz , jint inHandle, jint inep, jint outep)
{
	int handle = inHandle;
	int inEp = inep;
	int outEp = outep;

	int ret = 0;
	char msg[1024] = {0};
	int len = 4;
	//return str;

	setLog("-----------");
	setLog("dealwithUsb, handle: %X", handle);


	//device id
	memset(msg, 0, 1024);
	ret = usb_device_control_transfer_jni(handle, 0xa1, 0, 0, 0, msg, 1000, 3000);
	setLog("usb ctrl ret: %d, msg0: %X, msg: %s, error: %d", ret, msg[1], msg+1, errno);

	//device id
	memset(msg, 0, 1024);
	ret = usb_device_control_transfer_jni(handle, 0x80, 0, 0, 0, msg, 1000, 3000);
	setLog("usb ctrl ret: %d, msg0: %X, msg: %s, error: %d", ret, msg[1], msg, errno);

	//write data
	memset(msg, 0, 1024);
	ret = usb_device_bulk_transfer_jni(handle, 1, "\x1d\x53\x4c\x44", 4, 3000);
	setLog("usb bulk ret: %d", ret);

	//read data
	memset(msg, 0, 1024);
	ret = usb_device_bulk_transfer_jni(handle, 130, msg, 512, 3000);
	setLog("usb bulk ret: %d, msg5: %X, msg: %s, errno %d", ret, msg[5], msg+6, errno);

	return (*env)->NewStringUTF(env, "test");
}

int usb_device_bulk_transfer_jni(int fd, int endpoint, void* buffer, int length, unsigned int timeout)
{
	struct usbdevfs_bulktransfer ctrl;
	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.ep = endpoint;
	ctrl.len = length;
	ctrl.data = buffer;
	ctrl.timeout = timeout;
	return ioctl(fd, USBDEVFS_BULK, &ctrl);
}


int usb_device_control_transfer_jni(int fd, int requestType, int request, int value, int index, void* buffer, int length, unsigned int timeout)
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
void setLogStr(char *buf)
{
	int fp = open("/sdcard/jnilog.txt", O_RDWR | O_CREAT | O_APPEND);
	int ret = 0;

	if (fp < 0)
		return;

	ret = write(fp, buf, strlen(buf));
	ret = write(fp, "\n", 1);
	close(fp);
	return;
}

void setLog(const char *fmt, ...)
{
	int ret = 0;
	char msg[4096];
	memset(msg, 0, 4096);

	sprintf(msg, "%s: ", __TIME__);

	va_list args; //single var
	va_start(args, fmt);
	ret = vsprintf(msg+strlen(msg), fmt, args);
	va_end(args);

	setLogStr(msg);
}
jstring
Java_com_example_hellojni_HelloJni_stringFromJNIandInt( JNIEnv* env,
                                                  jobject thiz , jint inInt)
{
	int handle = inInt;
	char str[50] = "hello";


	jclass ClassJava;
	ClassJava = (*env)->FindClass(env, "com/example/hellojni/ClassJava");

	jmethodID constructmethod = (*env)->GetMethodID(env, ClassJava, "<init>", "()V");
	jobject mClassJava = (*env)->NewObject(env, ClassJava, constructmethod);

	jmethodID mmethod = (*env)->GetMethodID(env, ClassJava, "SayHello", "()Ljava/lang/String;");

	jstring mtest;
	mtest = (*env)->CallObjectMethod(env, mClassJava, mmethod);

	return mtest;
}

///////////

