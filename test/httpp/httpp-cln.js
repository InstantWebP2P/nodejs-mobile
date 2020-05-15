'use strict';

var httpp = require('httpp');

for (var i = 0; i < 256; i ++)
httpp.get('http://localhost:'+(process.argv[2] || 51680), function(res){
   console.log('STATUS: ' + res.statusCode);
   console.log('HEADERS: ' + JSON.stringify(res.headers));
   res.on('data', function (chunk) {
     console.log('BODY: ' + chunk);
  });
});

process.on('uncaughtException', (e) => { console.log('HTTPP Client crashed ...'); });