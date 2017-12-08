import javaCode.SubClass;

public class Demo {

public static void main(String[] args) {
		new Demo("Xiaofeng").myCall();
	}


public Demo(){
		//System.out.println("in default constructor");
		this.name = "default";
		//System.out.println("name is " + this.name);
	}

private String name = null;
public Demo(String name){
		//System.out.println("in special constructor");
		this.name = name;
		//System.out.println("name is " + this.name);
	}	

public void myCall(){
	SubClass s = new SubClass(this.name);
	s.say();
	}
}
