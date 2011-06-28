#!/usr/bin/env node

var sys = require('sys');
var assert = require('assert');
var rot13_js = require('../rot13');
var rot13_cpp = require('../build/default/rot13');

var tests = [
  ["I like pie", "V yvxr cvr"],
  ["I want 110 pies", "V jnag 110 cvrf"],   // numbers unchanged
  ["Çatal Höyük", "Çngny Uölüx"],           // utf8 ok
  ["☃", "☃"]                                // unicode snowman ok
];

function test_with_rotator_sync(r) {
  for (var i in tests) {
    sys.puts('[sync]    ' + tests[i][0] + ' -> ' + tests[i][1]);
    
    assert.ok(r.rotate(tests[i][0]) === tests[i][1]);
    assert.ok(r.rotate(r.rotate(tests[i][0])) === tests[i][0]);
  } 
}

function test_with_rotator_async(r) {
  for (var i in tests) {
    var source = tests[i][0];
    var expected = tests[i][1];
    var testAsync = function(source, expected) {
      r.rotateAsync(source, function(rotated) {
          sys.puts('[async]   ' + source + ' -> ' + expected);
          assert.ok(rotated === expected);
      });
    }
    testAsync(source, expected);
  }
}

function test_rot13_js() {
  sys.puts("testing rot13 js");
  var r = new rot13_js.Rot13();
  test_with_rotator_sync(r);
}

function test_rot13_cpp() {
  sys.puts("testing rot13 cpp");
  var r = new rot13_cpp.Rot13();
  test_with_rotator_sync(r);

  if (typeof r.rotateAsync !== 'undefined') {
    sys.puts("testing rot13 cpp async");
    test_with_rotator_async(r)
  } else {
    sys.puts("Cannot test rotateAsync.  The c++ extension doesn't provide it");
  }
}

test_rot13_js();
test_rot13_cpp();

