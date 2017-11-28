#!/bin/bash
help() {
echo "------old key start------"
echo "$1 audio com old"
echo "$1 audio live old"
echo "$1 video com old"
echo "$1 video live old"
echo ""

echo "----- new key start------"
echo "$1 audio com new"
echo "$1 audio live new"
echo "$1 video com new"
echo "$1 video live new"
echo ""

echo "----- new dynamic key start------"
echo "$1 audio com newdy"
echo "$1 audio live newdy"
echo "$1 video com newdy"
echo "$1 video live newdy"
echo ""
}

#newdynamic key
#通信
com_new_appid_dykey=f248443b95df480581e06497d620a48e

#通信动态key
com_new_channelkey='005AQAoAEQ0QzY0RjYzMkM0QTE2MkEyMEFFOUE5RENFNkRCRUJDNDFEMDEyQzYQAPJIRDuV30gFgeBkl9YgpI6yETBZAAAAAAAAAAAAAA=='

#直播
live_new_appid_dykey=80e54398fed94ae8a010acf782f569b7

#直播动态key
live_new_channelkey='005AQAoAEY3ODA5QTQ5QjZFQkNERDY0NUM2NzhDQTBBODRDNzA2QjhGQTZFNDMQAIDlQ5j+2UrooBCs94L1abdSGjBZAAAAAAAAAAAAAA=='


#new
com_new_appid=0c0b4b61adf94de1befd7cdd78a50444
live_new_appid=aab8b8f5a8cd4469a63042fcfafe7063


#old
com_key=0c0b4b61adf94de1befd7cdd78a50444
live_key=aab8b8f5a8cd4469a63042fcfafe7063
#com_key=74a0b7bb5d3e47c7abca0533d17b0afa
#live_key=f4637604af81440596a54254d53ade20


case $# in
    3)
        keyType=$3

        if [ "-$keyType" == "-newdy" ]; then
            com_param="--appId $com_new_appid_dykey --channelKey $com_new_channelkey --channelProfile 0"
            live_param="--appId $live_new_appid_dykey --channelKey $live_new_channelkey --channelProfile 1"
        elif [ "-$keyType" == "-new" ]; then 
            com_param="--appId $com_new_appid --channelKey $com_new_appid --channelProfile 0"
            live_param="--appId $live_new_appid --channelKey $live_new_appid --channelProfile 1"
        elif [ "-$keyType" == "-old" ]; then
            com_param="--appId $com_key --channelKey $com_key --channelProfile 0"
            live_param="--appId $live_key --channelKey $live_key --channelProfile 1"
        fi


        if [ $2 == "com" ]; then
            param=$com_param

        elif [ $2 == "live" ]; then
            param=$live_param
        fi

        echo ${param}

        if [ $1 == "audio" ]; then
           ./server_local --recordFileRootDir ./ --channel video  --uid 333 --appliteDir `pwd`/../../../bin --idle 20 --isAudioOnly 1 --isMixingEnabled 1  $param --decryptionMode aes-128-xts --secret 123 

#            ./server_local --recordFileRootDir ./ --channel audio  --uid 333 --appliteDir `pwd`/../../../bin --idle 20 $param --isAudioOnly 1

        elif [ $1 == "video" ]; then 
            ./Recorder_local  --recordFileRootDir ./ --channel video  --uid 333 --appliteDir `pwd`/../bin --idle 20 $param #--getVideoFrame 2 --getAudioFrame 2 # $param 

           # ./server --recordFileRootDir ./ --channel video  --uid 333 --appliteDir `pwd`/../../../bin --idle 20  $param --lowUdpPort 40000 --highUdpPort 40004 #--isMixingEnabled 1 #--decryptionMode aes-128-xts --secret 123 

           
        fi

        ;;
    *)
        help $0
        exit 1
        ;;
esac
help $0
