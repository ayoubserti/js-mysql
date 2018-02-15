#!/usr/bin/env sh

mkdir deps
cd deps

case "$(uname -s)" in

   Darwin)
     echo 'Mac OS X'
	curl https://s3.us-east-2.amazonaws.com/v8-build/v8-build-osx-x64-release.tar.gz > v8-prebuilt.tar.gz
     ;;

   Linux)
     echo 'Linux'
     curl https://s3.us-east-2.amazonaws.com/v8-build/v8-build-linux-x64-release.tar.gz > v8-prebuilt.tar.gz
     ;;
   *)
     echo 'other OS' 
     ;;
esac

tar -xzf v8-prebuilt.tar.gz
mv build/ v8
cd ../
