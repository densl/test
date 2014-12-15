package com.study.common;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.os.Environment;
import android.util.Log;


/**
 * 
 * common methods
 *
 */
public class M {
	private String TAG = "";
	private String SDPATH = "";
	
	public M(String mTag){
		TAG = mTag;				
		SDPATH = Environment.getExternalStorageDirectory().getPath();
	}// end M(String mTag)
	public final String directory(String dirPath){
		SDPATH = SDPATH + "/"+dirPath;
		
		File tempFile = new File(SDPATH);
		tempFile.mkdirs();
		return SDPATH;
	}// end directory
	public final void setLog(String msg){
		Log.i(TAG, "-----" + msg);

		SimpleDateFormat gStamp = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.ENGLISH);
		
		String str = gStamp.format(new Date()) + " "+TAG+": " + msg + "\n";
		try {
			FileOutputStream fos =  new FileOutputStream(SDPATH + "/zlog.txt", true);;
			fos.write( str.getBytes() );
			fos.close();
			
		} catch (FileNotFoundException e) {
			Log.i(TAG, "FileNotFoundException.");
			e.printStackTrace();			
		} catch (IOException e) {
			Log.i(TAG, "IOException.");
			e.printStackTrace();
		}
	}// end setLog
	
	
	@Override
	public String toString() {
		return getClass().getName()+"@"+SDPATH;
	}
}
