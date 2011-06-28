Rot13 = function() {
};

Rot13.prototype.rotate = function(s) {
  var i;
  var rotated = '';

  for (i=0; i<s.length; i++) {
    var ch = s.charCodeAt(i);
    // a-z -> n-m
    if (97 <= ch && ch <= 122) {
      rotated += String.fromCharCode( (ch-97 + 13) % 26 + 97);
    // A-Z -> N-M
    } else if (65 <= ch && ch <= 90) {
      rotated += String.fromCharCode( (ch-65 + 13) % 26 + 65);
    } else {
      rotated += s[i];
    } 
  }

  return rotated;
};

if (typeof exports !== 'undefined' && exports !== null) {
  exports.Rot13 = Rot13;
}
