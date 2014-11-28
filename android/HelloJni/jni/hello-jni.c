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

#include <stdio.h>
/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
jstring
Java_com_example_hellojni_HelloJni_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{
#if defined(__arm__)
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
   #define ABI "x86"
#elif defined(__x86_64__)
   #define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
   #define ABI "mips64"
#elif defined(__mips__)
   #define ABI "mips"
#elif defined(__aarch64__)
   #define ABI "arm64-v8a"
#else
   #define ABI "unknown"
#endif

    return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI " ABI ".");
}

jstring
Java_com_example_hellojni_HelloJni_stringFromJNIandInt( JNIEnv* env,
                                                  jobject thiz , jint inInt)
{
	int handle = inInt;

	char str[50] = "hello";

	//return str;

	if (handle == 99)
		return (*env)->NewStringUTF(env, "Hello from jni and 99" );
	else
		return (*env)->NewStringUTF(env, "Hello from jni and not 99");


	//
	//jclass ClassJava;
	//ClassJava = (*env)->FindClass(env, "com/example/hellojni/ClassJava");
	jclass ClassJava;
	ClassJava = (*env)->FindClass(env, "com/example/hellojni/ClassJava");



	jmethodID constructmethod = (*env)->GetMethodID(env, ClassJava, "<init>", "()V");
	jobject mClassJava = (*env)->NewObject(env, ClassJava, constructmethod);


	jmethodID mmethod = (*env)->GetMethodID(env, ClassJava, "SayHello", "()Ljava/lang/String;");

	jstring mtest;
	mtest = (*env)->CallObjectMethod(env, mClassJava, mmethod);

	return mtest;


	//return "hello";
}


jstring
Java_com_example_hellojni_HelloJni_testo( JNIEnv* env,
                                                  jobject thiz , jobject mActivity)
{

	jboolean ret = 0;

	jclass UsbPrint;
	UsbPrint = (*env)->FindClass(env, "com/example/hellojni/UsbPrint");


	//android.app.Activity;
	jmethodID constructmethod = (*env)->GetMethodID(env, UsbPrint, "<init>", "()V");
	jobject mUsbPrint = (*env)->NewObject(env, UsbPrint, constructmethod, mActivity);


	jmethodID mact = (*env)->GetMethodID(env, UsbPrint, "SetAct", "(Landroid/app/Activity;)V");

	(*env)->CallVoidMethod(env, mUsbPrint, mact, mActivity);



	jmethodID mopenusb = (*env)->GetMethodID(env, UsbPrint, "OpenUsb", "()V");
	(*env)->CallVoidMethod(env, mUsbPrint, mopenusb);


	char toSend[] = "This is a test from hello-jni.c ";
	int len = strlen(toSend);//30;

	jbyteArray bytearray = (*env)->NewByteArray(env, len);
	(*env)->SetByteArrayRegion(env, bytearray, 0, len, toSend);

	jmethodID usbwrite = (*env)->GetMethodID(env, UsbPrint, "UsbWrite", "([BI)Z");
	ret = (*env)->CallBooleanMethod(env, mUsbPrint, usbwrite, bytearray, len);


	jmethodID mmethod = (*env)->GetMethodID(env, UsbPrint, "SayHello", "()Ljava/lang/String;");

	jstring mtest;
	mtest = (*env)->CallObjectMethod(env, mUsbPrint, mmethod);

	return mtest;


	//




	//


	return (*env)->NewStringUTF(env, "End test.");


	//return "hello";
}

jstring
Java_com_example_hellojni_HelloJni_dealwithUsb( JNIEnv* env,
                                                  jobject thiz , jint inInt)
{
	int handle = inInt;

	char str[50] = "hello";

	//return str;

	setLog(str);
	if (handle == 99)
		return (*env)->NewStringUTF(env, str );
	else
		return (*env)->NewStringUTF(env, str);


	//
	//jclass ClassJava;
	//ClassJava = (*env)->FindClass(env, "com/example/hellojni/ClassJava");
//	jclass ClassJava;
//	ClassJava = (*env)->FindClass(env, "com/example/hellojni/ClassJava");
//
//
//
//	jmethodID constructmethod = (*env)->GetMethodID(env, ClassJava, "<init>", "()V");
//	jobject mClassJava = (*env)->NewObject(env, ClassJava, constructmethod);
//
//
//	jmethodID mmethod = (*env)->GetMethodID(env, ClassJava, "SayHello", "()Ljava/lang/String;");
//
//	jstring mtest;
//	mtest = (*env)->CallObjectMethod(env, mClassJava, mmethod);
//
//	return mtest;


	//return "hello";
}


void setLog(char *buf)
{
	FILE *fp = fopen("/sdcard/jnilog.txt", "w+");

	if (fp != NULL)
	{
		fputs(buf, fp);
		fflush(fp);
		fclose(fp);
	}
	return;
}
