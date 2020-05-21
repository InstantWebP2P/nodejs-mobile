'use strict';

var https = require('https');

for (var i = 0; i < 200; i ++)
https.get('https://localhost:51680', function(res){
   console.log('STATUS: ' + res.statusCode);
   console.log('HEADERS: ' + JSON.stringify(res.headers));
   res.on('data', function (chunk) {
       console.log('BODY: ' + chunk);
  });
});
