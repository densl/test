package com.example.hellojni;

public class ClassJava {
	public ClassJava(){
		
		System.out.println("hello");
	}
	
	public static String StaticSayHello()
	{
		return "static hello from ClassJava";
	}
	
	public String SayHello()
	{
		String test  = "hello from ClassJava.";
		return test;
	}
	
	public int SayInt()
	{
		return 88;
	}

	public int Trans(int k)
	{
		return k;
	}
}
