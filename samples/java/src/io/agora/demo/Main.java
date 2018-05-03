package demo;
import java.io.*;
import io.agora.record.AgoraJavaRecording;

public class Main {
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String libraryPath = "./librecording.so";
		AgoraJavaRecording agoraJavaRecording = new AgoraJavaRecording(libraryPath);
        System.out.println("ars.createChannel(args)");
		
		AgoraJavaRecordingImp ars = new AgoraJavaRecordingImp(agoraJavaRecording);
        System.out.println("ars.createChannel(args) -1");
		ars.createChannel2(args);
        try{
            System.out.println("ars.createChannel(args) -1.1");
        Thread.sleep(4000);
        }catch(Exception e){
        }
        System.out.println("ars.createChannel(args) -2");
		ars.unRegister();
	}
}
