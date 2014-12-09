package com.study.common;

public class LocalUsb {
	static {
		System.loadLibrary("local-usb");
	}
    public static native int nsetupEnv(int fd, int inEp, int outEp);
    public static native int ngetStatus();
    public static native void nclose();
    public static native int nwrite(byte[] buffer, int length, int timeout);
    public static native int nread(byte[] buffer, int length, int timeout);
}
