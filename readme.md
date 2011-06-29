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

`lib/rot13_javascript.js` is a JS implementation.

`src/rot13.cpp` is a C++ extension.

Build the node extension:

    % node-waf configure build

Use like so:

    % node
    > var rot13 = require("./rot13")
    > var r = new rot13.Rot13()
    > r.rotate("Attack at dawn!")
    'Nggnpx ng qnja!'

The c++ implementation includes an asynchronous method:

    > r.rotateAsync("Attack at dawn!", function(r) { console.log("rot13: " + r)} )
    rot13: Nggnpx ng qnja!

Run `test/test.js` to verify that the C++ and JS versions do the same thing.

Timing
------

On my MacBook, the first part of the test suite reports:

    ** Timing comparison (milliseconds)
    rot13 encoding Aeneid, Book I, 100 times
    Average js run: 2.77
    Average cpp run: 2.59
    Average cpp async run: 1.23

Note that the timing test runs the pure js encoder once in advance to give v8 a
chance to compile it.  

Rot13 is obviously a silly example, but it demonstrates the speed of node.js
and js on v8.  The synchronous js method is just about as fast as native C++.

It also shows the value of making your function async if possible.  Here, the
C++ `rotateAsync()` only spends about 40% of the time on the main node.js
thread as the other two `rotate()` methods.

Resources
---------

[Paul Querna's Tutorial](https://www.cloudkick.com/blog/2010/aug/23/writing-nodejs-native-extensions/)

To Do
-----

1. Make this not happen when you forget to use `new`:

        > var rot13 = require("./rot13");
        > var r = rot13.Rot13();   // <-- didn't say "new rot13.Rot13()"
        Assertion failed: (handle->InternalFieldCount() > 0), function Wrap, file /usr/local/include/node/node_object_wrap.h, line 61.



