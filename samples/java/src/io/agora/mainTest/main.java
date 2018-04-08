package mainTest;

import io.agora.record.AgoraJavaRecording;

public class main {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String libraryPath = "sfsdf";
		AgoraJavaRecording agoraJavaRecording = new AgoraJavaRecording(libraryPath);
		
		AgoraJavaRecordingTest  test = new AgoraJavaRecordingTest(agoraJavaRecording);
		test.test(args);

		test.unRegister();
	}

}
