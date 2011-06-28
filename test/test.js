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

function test_with_rotator(r) {
  for (var i in tests) {
    sys.puts('   ' + tests[i][0] + ' -> ' + tests[i][1]);
    
    assert.ok(r.rotate(tests[i][0]) === tests[i][1]);
    assert.ok(r.rotate(r.rotate(tests[i][0])) === tests[i][0]);
  } 
}

function test_rot13_js() {
  sys.puts("testing rot13 js");
  var r = new rot13_js.Rot13();
  test_with_rotator(r);
}

function test_rot13_cpp() {
  sys.puts("testing rot13 cpp");
  var r = new rot13_cpp.Rot13();
  test_with_rotator(r);
}

test_rot13_js();
test_rot13_cpp();

