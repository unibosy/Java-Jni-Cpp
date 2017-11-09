class Hello{
  
  static {  
        System.load("/home/wangshiwei/ars/code_ars/javaDemo/libmain.so");   
  }

  public native static int add(int x,int y);

  public static void main(String[] args) {
    System.out.println("hello");
    Hello hello = new Hello();
    int sum = hello.add(1,1);
    System.out.println("sum="+sum);
  }


}
