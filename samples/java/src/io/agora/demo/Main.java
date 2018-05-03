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
        System.out.println("ars.createChannel(args) -2");
		ars.unRegister();
        System.out.println("ars.createChannel(args) -3");
        
		//System.exit(0);
	}
}
