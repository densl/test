
#include <android/log.h>
#include <string.h>
#include <jni.h>
#include <fcntl.h>
#include <stdio.h>

#include <errno.h>
#include <linux/usbdevice_fs.h>
#include <sys/ioctl.h>
#include <unistd.h>



// declare functions
void setLog(const char *fmt, ...);
void setLogStr(char *buf);

#define JNI_CLASS_STR "com/example.hellojni.HelloJni"
#define TAG "zeng"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

JNIEXPORT jint JNICALL native_test(JNIEnv* env, jobject thiz);


static JNINativeMethod methods[] = {
		{"native_test", "()I",	(void*)native_test},

};

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;

	//get env
	if ( (*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK )
		return JNI_ERR;

	LOGE("test");
//	jclass classz;
//	classz = (*env)->GetSuperclass()


	registerNatives(env);

	return JNI_VERSION_1_4;

}



JNIEXPORT jint JNICALL native_test(JNIEnv* env, jobject thiz)
{

	setLog("It is in native test");


	return 99;
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
