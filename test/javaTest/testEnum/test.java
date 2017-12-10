class AudioFrame{
  public final int PcmType = 0;
  public final int AacType =1;
  private int value = PcmType;
  public void setType(int type){
    value = type;
  }
  public int getType(){
    return value;
  }

}
