#!/usr/bin/env sh

mkdir deps
cd deps

if [ "$(uname)" == "Darwin" ]; then
    curl https://s3.us-east-2.amazonaws.com/v8-build/v8-build-osx-x64-release.tar.gz > v8-prebuilt.tar.gz
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    curl https://s3.us-east-2.amazonaws.com/v8-build/v8-build-linux-x64-release.tar.gz > v8-prebuilt.tar.gz
fi
tar -xzf v8-prebuilt.tar.gz
mv build/ v8
cd ../