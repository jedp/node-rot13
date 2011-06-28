node-rot13
==========

This is me learning how to write native extensions for node.js.

And if you have a rot13-intensive node.js application that's been dying to have
the speed of a native rot13 C++ extension, well then.  Here you are.

    #!zsh
    % node-waf configure build
    % node
    > var rot13 = require("./build/default/rot13")
    > var r = new rot13.Rot13()
    > r.rotate("Attack at dawn!")
    'Nggnpx ng qnja!'

Run `test/test.js` to verify that the C++ and JS versions do the same thing.

Resources
---------

[Paul Querna's Tutorial](https://www.cloudkick.com/blog/2010/aug/23/writing-nodejs-native-extensions/)

