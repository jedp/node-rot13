#!/usr/bin/env node

var fs = require('fs');
var util = require('util');
var assert = require('assert');
var rot13_js = require('../lib/rot13_javascript');
var rot13_cpp = require('../build/default/rot13');

var tests = [
  ["I like pie", "V yvxr cvr"],
  ["I want 110 pies", "V jnag 110 cvrf"],   // numbers unchanged
  ["Çatal Höyük", "Çngny Uölüx"],           // utf8 ok
  ["☃", "☃"]                                // unicode snowman ok
];

function test_with_rotator_sync(r) {
  for (var i in tests) {
    util.puts('[sync]    ' + tests[i][0] + ' -> ' + tests[i][1]);
    
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
          util.puts('[async]   ' + source + ' -> ' + expected);
          assert.ok(rotated === expected);
      });
    }
    testAsync(source, expected);
  }
}

function test_rot13_js() {
  util.puts("testing rot13 js");
  var r = new rot13_js.Rot13();
  test_with_rotator_sync(r);
}

function test_rot13_cpp() {
  util.puts("testing rot13 cpp");
  var r = new rot13_cpp.Rot13();
  test_with_rotator_sync(r);

  if (typeof r.rotateAsync !== 'undefined') {
    util.puts("testing rot13 cpp async");
    test_with_rotator_async(r)
  } else {
    util.puts("Cannot test rotateAsync.  The c++ extension doesn't provide it");
  }
}

function time_them () {
  var r_js = new rot13_js.Rot13();
  var r_cpp = new rot13_cpp.Rot13();
  var aeneid = fs.readFileSync('test/aeneid.I.txt', 'ascii');

  // give v8 a chance to make r_js fast
  r_js.rotate("I like pie");

  var js_time = 0;
  var cpp_time = 0;
  var cpp_async_time = 0;

  for (var i=0; i<100; i++) {
    var then = new Date();
    r_js.rotate(aeneid);
    js_time += new Date() - then;
  }
  util.puts("Average js run: " + (js_time/100));

  for (var i=0; i<100; i++) {
    var then = new Date();
    r_cpp.rotate(aeneid);
    cpp_time += new Date() - then;
  }
  util.puts("Average cpp run: " + (cpp_time/100));

  function do_nothing() {};
  for (var i=0; i<100; i++) {
    var then = new Date();
    r_cpp.rotateAsync(aeneid, do_nothing);
    cpp_async_time += new Date() - then;
  }
  util.puts("Average cpp async run: " + (cpp_async_time/100));
}

util.puts("\n** Timing comparison (milliseconds)");
util.puts("rot13 encoding Aeneid, Book I, 100 times");
time_them();

util.puts("\n** Unit Tests");

test_rot13_js();
test_rot13_cpp();
