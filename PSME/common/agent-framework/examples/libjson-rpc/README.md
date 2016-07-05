README
======

Example
-------

libjson-rpc example

1. Run json-rpc server:

    bin/examples/jsonrpc_example_server 8888

2. Run json-rpc client:

    bin/examples/jsonrpc_example_client localhost:8888

Generating JSON-RPC stubs
-------------------------

1. Create *.json schema file like example.json
2. Run jsonrpcstub to generate C++ stubs:


    jsonrpcstub example.json --cpp-server=AbstractStubServer --cpp-client=StubClient


3. Change throw() keywords to noexcept(false)
4. Create stub destructros to remove weak virtual table error

Workarounds
-----------

Run client site with root privileges, there is problem with network access
using curl.
