import java.lang.InterruptedException;
class Test2{
    private String storeDir  = "/wangshiwei/ServerSDK-Video/Agora_Recording_SDK_for_Linux_FULL/samples/java/";   
    private StringBuffer path =  new StringBuffer("");
    private String str = "";
    public void test(long uid){
      path.append(storeDir).append(Long.toString(uid));
      path.append(".yuv");
      str = path.toString();
    }
    public void test2(long uid){
      String path =  storeDir + Long.toString(uid);
      path+=".yuv";
      //str = path.toString();
      System.out.println("path:"+path);
    }

public static void main(String[] args) {
  Test2 t = new Test2();
  for(long i = 0; i<10000;i++){
    t.test2(i);
    try{
      Thread.currentThread().sleep(10);//sleep 100ms
    }catch(InterruptedException ie){
      System.out.println("exception throwed!");
    }
  }
  System.out.println("end for!!");
  try{
      Thread.currentThread().sleep(1000);//sleep 100ms
    }catch(InterruptedException ie){
      System.out.println("exception throwed!");
    }

     System.out.println("test end");
  }

}
