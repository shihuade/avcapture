#!/bin/bash
#**************************************************************************************
# brief:
#       Transform NV12 to common YUV420 format,and upload to YUV ftp server
#       for more detail about YUV format, please refer below link
#       https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
#
# Usage:
#      please refer to function runUsage
#
# Others:
#      for how to configure host to ftp server, refer to runConfigureHost2FTPServer()
#
# date: 2016.12.13
#**************************************************************************************

runUsage()
{
    echo -e "\033[32m\n******************************************************************\033[0m"
    echo -e "\033[32m   Usage:  $0   \${YUVProfile}     \${Option}                       \033[0m"
    echo -e "\033[32m                \${TransformIterm} \${Resolution}                   \033[0m"
    echo ""
    echo -e "\033[32m  \${YUVProfile} should be: screen/camera                           \033[0m"
    echo -e "\033[32m  \${Option}     should be: transform/upload/pipeline               \033[0m"
    echo -e "\033[32m                            transform: NV12To420 only               \033[0m"
    echo -e "\033[32m                            upload:    upload to ftp only           \033[0m"
    echo -e "\033[32m                            pipeline:  nv12To420 and upload to ftp  \033[0m"
    echo -e "\033[32m  \${TransformIterm} should be:                                     \033[0m"
    echo -e "\033[32m                            no value:  all .yuv file in ~/Desktop   \033[0m"
    echo -e "\033[32m                            file:      .yuv file                    \033[0m"
    echo -e "\033[32m                            dir:       .yuv files in dir            \033[0m"
    echo -e "\033[32m  \${Resolution}   should be: optional param, no value as default   \033[0m"
    echo -e "\033[32m                               shoule be1080p 720p etc.             \033[0m"
    echo ""
    echo -e "\033[33m  nv12 to 420, yuv file name must have nv12/resolution patern       \033[0m"
    echo -e "\033[33m              TestYUV_nv12_1920x1080.yuv                            \033[0m"

    echo ""
    echo -e "\033[34m Example:                                                           \033[0m"
    echo -e "\033[34m   1. nv12 to 420 only for  ./YUV/TestYUV_nv12_1920x1080.yuv        \033[0m"
    echo -e "\033[34m      $0 camera transform   ./YUV/TestYUV_nv12_1920x1080.yuv        \033[0m"
    echo ""
    echo -e "\033[34m   2. upload to ftp only for  ./YUV/TestYUV_nv12_1920x1080.yuv      \033[0m"
    echo -e "\033[34m      $0 camera upload      ./YUV/TestYUV_nv12_1920x1080.yuv  1080p \033[0m"
    echo ""
    echo -e "\033[34m   3. nv12 to 420 then upload to ftp for TestYUV_nv12_1920x1080.yuv \033[0m"
    echo -e "\033[34m      $0 camera pipeline    ./YUV/TestYUV_nv12_1920x1080.yuv        \033[0m"
    echo ""
    echo -e "\033[34m   4. nv12 to 420 then upload to ftp for all nv12 file in ./YUV dir \033[0m"
    echo -e "\033[34m      $0 camera pipeline    ./YUV                                   \033[0m"
    echo ""
    echo -e "\033[32m******************************************************************\n\033[0m"

    exit 1
}


#config host to FTP server without password
runConfigureHost2FTPServer()
{
    echo "example for host configuration:"
    #Step 1: Create public and private keys using ssh-key-gen on local-host
             #if there is public key file ~/.ssh/id_rsa.pub, skip this step;
             #run command: ssh-keygen
    #Step 2: Copy the public key to FTP server using ssh-copy-id
             #run command: ssh-copy-id -i ~/.ssh/id_rsa.pub FTPServer
    #Step 3: now you can login FTP server without password
}


runInit()
{
    CurrentDir=`pwd`

    #FTP server
    FTPServerIP="10.224.203.70"
    FTPServer="ftp://${FTPServerIP}"
    FTPYUVFolder="YUV"
    FTPYUVHomeDir="/home/Video/${FTPYUVFolder}"
    FTPUserName="Video"
    FTPPass="pass789"
    FTPUploadURL=""
    UploadFile=""

    #YUV info
    #720p 1080p etc
    Resolution=""
    #Screen/Camera
    #YUVProfile=""

    YUVToolSrcDir="${CurrentDir}/YUVTool"
    YUVToolApp="YUVTool.app"

    #input params for YUVToolApp
    NV12File=""
    YUV420File=""
    let "YUVTransMode = 2 "
    let "YUVWidth     = 1920"
    let "YUVHeight    = 1080"
}

runParamValidate()
{
    let "Flag = 0"
    [ ! "$YUVProfile" = "screen" ] && [ ! "$YUVProfile" = "camera" ] && let "Flag = 1"
    [ ! ${Flag} -eq 0 ] && echo "Param error----YUVProfile should be sreen or camera" && runUsage

    let "Flag = 0"
    [ ! "$Option" = "transform" ] && [ ! "$Option" = "upload" ] && [ ! "$Option" = "pipeline" ] && let "Flag = 1"
    [ ! ${Flag} -eq 0 ]           && echo "Param error----Option should be transform,camera or pipeline" && runUsage

    let "Flag = 0"
    if [ -z "${TransformIterm}" ]
    then
       cd ~/Desktop &&  TransformIterm=`pwd` && cd -
    fi

    echo "TransformIterm is ${TransformIterm}"
    NV12Pattern="nv12"
    [ ! -f "${TransformIterm}" ] && [ ! -d "${TransformIterm}" ] && let "Flag = 1"
    [ ! ${Flag} -eq 0 ]          && echo "Param error----TransformIterm is not a file or dir,please double check" && runUsage
    [ -f "${TransformIterm}" ]   && NV12Pattern=`echo ${TransformIterm} | grep "nv12"`
    [ -z "${NV12Pattern}" ]      && echo "Param error----NV12 file's name should contain pattern nv12" && runUsage
}


runGenerateYUVToolApp()
{
    let "Flag = 0"
    cd ${YUVToolSrcDir}
    make clean
    make
    [  $? -eq 0 ] && [ -e ${YUVToolApp} ] && let "Flag = 1"
    cd -

    [ ! ${Flag} -eq 1 ] && echo "YUV tool build faild, please double check!" && exit 1

    cp -f ${YUVToolSrcDir}/${YUVToolApp}  ./
    echo "    YUV tool app----${YUVToolApp} build succeed!    "
}

runTransformNV12ToYUV420()
{
    [ ! -e ${NV12File} ] && echo "NV12File--${NV12File}, no such file, please double check" && exit 1
    [ -e ${YUV420File} ] && rm ${YUV420File}

    echo -e "\033[32m\n**********************************************************\n\033[0m"
    echo -e "\033[34m\n  Transforming NV12 to YUV420 format....                  \n\033[0m"
    echo -e "\033[34m\n  NV12File   is: ${NV12File}                              \n\033[0m"
    echo -e "\033[34m\n  YUV420File is: ${YUV420File}                            \n\033[0m"
    echo -e "\033[32m\n**********************************************************\n\033[0m"

    TransformCommand="./${YUVToolApp} ${YUVTransMode} ${NV12File} ${YUV420File} ${YUVWidth} ${YUVHeight}"
    echo "TransformCommand is ${TransformCommand}"
    ${TransformCommand}
    [ ! $? -eq 0 ] && echo -e "\033[31m\n Transform failed! please double check! \n\033[0m" && exit 1

    echo -e "\033[32m**********************************************************\033[0m"
    echo -e "\033[34m  Transforming NV12 to YUV420 succeed!                    \033[0m"
    echo -e "\033[32m**********************************************************\033[0m"
}

runParseAndGenerateYUVFileInfo()
{
    #TestYUV_nv12_1920x1080.yuv
    FileName=`echo ${NV12File} | awk 'BEGIN {FS="/"} {print $NF}'`
    FileDir=`echo ${NV12File}  | awk 'BEGIN {FS="/"} {for (i=1;i<NF;i++) printf("%s/",$i)}'`
    [ -d ${FileDir} ] && cd ${FileDir} &&  FileDir=`pwd` && cd -

    aYUVInfo=(`echo ${FileName} | awk 'BEGIN {FS="[_.]"} {for(i=1;i<=NF;i++) printf("%s  ",$i)}'`)
    Pattern_01="[xX]"
    Pattern_02="^[1-9][0-9]"
    Pattern_03="[0-9][0-9]$"

    #get PicW PicH info
    for  Iterm in ${aYUVInfo[@]}
    do
        if [[ $Iterm =~ $Pattern_01 ]] && [[ $Iterm =~ $Pattern_02 ]] && [[ $Iterm =~ $Pattern_03 ]]
        then
            PicWidth=`echo $Iterm  | awk 'BEGIN {FS="[xX]"} {print $1}'`
            PicHeight=`echo $Iterm | awk 'BEGIN {FS="[xX]"} {print $2}'`
        fi
    done

    #generate YUV420 file's path
    OutFileName=""
    for((i=0; i< ${#aYUVInfo[@]} - 1; i++))
    do
        Iterm="${aYUVInfo[$i]}"
        [ "${Iterm}" = "nv12" ] && Iterm="YUV420"
        [ $i -eq 0 ]   && OutFileName="${Iterm}"
        [ ! $i -eq 0 ] && OutFileName="${OutFileName}_${Iterm}"
   done

   YUV420File="${FileDir}/${OutFileName}.yuv"
}

runUploadYUV420FileToFTPServer()
{
    if [ ! -e ${UploadFile} ]
    then
        echo -e "\033[31m\n Upload file ${UploadFile} not found! please double check! \n\033[0m"
        exit 1
    fi

    FTPUploadURL="${FTPUserName}@${FTPServerIP}:${FTPYUVHomeDir}/${YUVProfile}/${Resolution}"
    FileURL="${FTPServer}/${FTPYUVFolder}/${YUVProfile}/${Resolution}"

    #upload to FTP server
    FTPUploadCommand="scp  ${UploadFile}  ${FTPUploadURL}"
    echo "FTPUploadCommand is ${FTPUploadCommand}"
    ${FTPUploadCommand}
    [ ! $? -eq 0 ] &&  echo -e "\033[32m\n   upload failed! please double check！ \n\033[0m" && exit 1

    echo -e "\033[32m\n************************************************\n\033[0m"
    echo -e "\033[32m\n   Upload file ${UploadFile} succeed！          \n\033[0m"
    echo -e "\033[34m\n   FTP url     is: ${FileURL}                   \n\033[0m"
    echo -e "\033[34m\n   FTP account is: ${FTPUserName}/${FTPPass}    \n\033[0m"
    echo -e "\033[32m\n************************************************\n\033[0m"
}

runProcessOneNV12File()
{
    runParseAndGenerateYUVFileInfo
    UploadFile=${YUV420File}

    [ "$Option" = "transform" ] && runTransformNV12ToYUV420
    [ "$Option" = "upload" ]    && runUploadYUV420FileToFTPServer
    [ "$Option" = "pipeline" ]  && runTransformNV12ToYUV420 && runUploadYUV420FileToFTPServer
}

runProcessAllNV12FileInGivenDir()
{
    for file in ${TransformIterm}/*.yuv
    do
        echo "file is ${file}"
        NV12Match=`echo ${file} | awk 'BEGIN {FS="/"} {print $NF}' | grep nv12`
        [ ! "${NV12Match}X" = "X" ] && NV12File="$file" && runProcessOneNV12File
    done
}

runProcess()
{
    echo -e "\033[32m\n************************************************\033[0m"
    echo -e "\033[32m\n Start processing....                           \033[0m"
    echo -e "\033[32m\n TransformIterm is ${TransformIterm}            \033[0m"
    echo -e "\033[32m\n************************************************\033[0m"

    [ -d ${TransformIterm} ] && runProcessAllNV12FileInGivenDir
    [ -f ${TransformIterm} ] && NV12File="$TransformIterm" && runProcessOneNV12File
}

runMain()
{
    runInit
    runParamValidate

    runGenerateYUVToolApp
    runProcess
}


#*******************************************************
#  Main Entry
#*******************************************************
#
YUVProfile=$1
Option=$2
TransformIterm=$3
Resolution=$4

if [ $# -lt 2 ]
then
    runUsage
fi

runMain
#*******************************************************



