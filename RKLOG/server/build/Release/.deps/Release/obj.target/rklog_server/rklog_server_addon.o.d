cmd_Release/obj.target/rklog_server/rklog_server_addon.o := g++ '-DNODE_GYP_MODULE_NAME=rklog_server' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-DBUILDING_NODE_EXTENSION' -I/usr/include/nodejs/include/node -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs/deps/v8/include -I../../../include  -fPIC -pthread -Wall -Wextra -Wno-unused-parameter -m64 -O3 -fno-omit-frame-pointer -fno-rtti -std=gnu++0x -MMD -MF ./Release/.deps/Release/obj.target/rklog_server/rklog_server_addon.o.d.raw   -c -o Release/obj.target/rklog_server/rklog_server_addon.o ../rklog_server_addon.cc
Release/obj.target/rklog_server/rklog_server_addon.o: \
 ../rklog_server_addon.cc /usr/include/nodejs/deps/uv/include/uv.h \
 /usr/include/nodejs/deps/uv/include/uv-errno.h \
 /usr/include/nodejs/deps/uv/include/uv-version.h \
 /usr/include/nodejs/deps/uv/include/uv-unix.h \
 /usr/include/nodejs/deps/uv/include/uv-threadpool.h \
 /usr/include/nodejs/deps/uv/include/uv-linux.h ../napi.h \
 /usr/include/nodejs/src/node_api.h \
 /usr/include/nodejs/src/node_api_types.h ../napi-inl.h \
 ../napi-inl.deprecated.h ../rklog_server.h ../readerwriterqueue.h \
 ../atomicops.h
../rklog_server_addon.cc:
/usr/include/nodejs/deps/uv/include/uv.h:
/usr/include/nodejs/deps/uv/include/uv-errno.h:
/usr/include/nodejs/deps/uv/include/uv-version.h:
/usr/include/nodejs/deps/uv/include/uv-unix.h:
/usr/include/nodejs/deps/uv/include/uv-threadpool.h:
/usr/include/nodejs/deps/uv/include/uv-linux.h:
../napi.h:
/usr/include/nodejs/src/node_api.h:
/usr/include/nodejs/src/node_api_types.h:
../napi-inl.h:
../napi-inl.deprecated.h:
../rklog_server.h:
../readerwriterqueue.h:
../atomicops.h:
