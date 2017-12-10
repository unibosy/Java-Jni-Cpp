package javaCode;

public class SubClass {
	private String name = null;
	public SubClass(String name){
		this.name = name;
	}

	public void say(){
		System.out.println("Hello " + this.name);
	}
}
