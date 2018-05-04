import java.lang.InterruptedException;
import java.nio.ByteBuffer;
import java.io.File;
import java.io.FileWriter;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
class Test2{
  private String storeDir  = "/root/wangshiwei/myGit/JavaJni/samples/java/test/";   
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
  private void WriteBytesToFileClassic(ByteBuffer byteBuffer,String fileDest) {
    if(byteBuffer == null){
      System.out.println("WriteBytesToFileClassic but byte buffer is null!");
      return;
    }
    byte[] data = new byte[byteBuffer.capacity()];
    ((ByteBuffer) byteBuffer.duplicate().clear()).get(data);
    FileOutputStream fileOuputStream = null;
    try {
      fileOuputStream = new FileOutputStream(fileDest, true);
      fileOuputStream.write(data);
    } catch (IOException e) {
      e.printStackTrace();
    } finally {
      if (fileOuputStream != null) {
        try {
          fileOuputStream.close();
        } catch (IOException e) {
          e.printStackTrace();
        }
      }
    }
  }
  public static void main(String[] args) {
    Test2 t = new Test2();
    /*for(long i = 0; i<10000;i++){
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
    }*/
    
    ByteBuffer buf = ByteBuffer.allocateDirect(996);
    for(long i = 0; i < 100000000; ++i){
      String path = t.storeDir + Long.toString(333);
      path+=".yuv";
      try{
        Thread.currentThread().sleep(10);//sleep 300ms
      }catch(InterruptedException ie){
        System.out.println("exception throwed!");
      }

      t.WriteBytesToFileClassic(buf,path);
    }
    System.out.println("test end");
  }

}
