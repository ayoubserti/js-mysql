#!/usr/bin/env sh

if ! type gn > /dev/null; then
    # install depot tool
    git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
    export PATH=`pwd`/depot_tools:"$PATH"
fi

fetch --no-history v8
cd v8
gclient sync
gn gen out.gn/x64.release/ --args='is_debug=false target_cpu="x64" is_component_build=false v8_static_library=true'
ninja -C out.gn/x64.release/ v8_libsampler
ninja -C out.gn/x64.release/ icu
ninja -C out.gn/x64.release/ v8_libplatform
ninja -C out.gn/x64.release/ v8_base
ninja -C out.gn/x64.release/ v8_initializers
ninja -C out.gn/x64.release/ v8_init
ninja -C out.gn/x64.release/ v8_external_snapshot
ninja -C out.gn/x64.release/ v8_hello_world
echo "=======> build finshed"

 cd ..
mkdir v8
mkdir v8/libs
cp v8/out.gn/x64.release/obj/*.a v8/libs 
cp v8/out.gn/x64.release/*.a v8/libs 
cp v8/out.gn/x64.release/third_party/icu/*.a v8/libs 
cp v8/out.gn/x64.release/icudtl.dat v8 
cp v8/out.gn/x64.release/snapshot_blob.bin v8/ 
cp v8/out.gn/x64.release/natives_blob.bin v8/ 
cp v8/out.gn/x64.release/v8_hello_world v8/ 
mkdir v8/include  
cp -R v8/include v8/include

echo "=======> file copyied to v8 folder"


