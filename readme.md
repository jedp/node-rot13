node-rot13
==========

Sync and async implementations of rot13 in a c++ extension for node.js.

This is me trying to learn C++ and v8 at the same time.  Please tell me if you
see anything I'm doing wrong or could do better.

And if you happen to have a rot13-intensive node.js application that's been
dying to have the speed of a native rot13 C++ extension working asynchronously
on the v8 thread pool, well then.  Here you are.

Summary
-------

`rot13.js` is a JS implementation.

`src/rot13.cpp` is a C++ extension.

    % node-waf configure build
    % node
    > var rot13 = require("./build/default/rot13")
    > var r = new rot13.Rot13()
    > r.rotate("Attack at dawn!")
    'Nggnpx ng qnja!'

The c++ implementation includes an asynchronous method:

    > r.rotateAsync("Attack at dawn!", function(r) { console.log("rot13: " + r)} )
    rot13: Nggnpx ng qnja!

Run `test/test.js` to verify that the C++ and JS versions do the same thing.

Resources
---------

[Paul Querna's Tutorial](https://www.cloudkick.com/blog/2010/aug/23/writing-nodejs-native-extensions/)

To Do
-----

1. Make this not happen when you forget to use `new`:

        > var rot13 = require("./build/default/rot13");
        > var r = rot13.Rot13();   // <-- didn't say "new rot13.Rot13()"
        Assertion failed: (handle->InternalFieldCount() > 0), function Wrap, file /usr/local/include/node/node_object_wrap.h, line 61.



