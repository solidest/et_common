cmd_Release/rkdb_client.node := ln -f "Release/obj.target/rkdb_client.node" "Release/rkdb_client.node" 2>/dev/null || (rm -rf "Release/rkdb_client.node" && cp -af "Release/obj.target/rkdb_client.node" "Release/rkdb_client.node")
