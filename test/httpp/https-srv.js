'use strict';

var https = require('https');
var fs = require('fs');

var options = {
     key: fs.readFileSync('test-key.pem'),
    cert: fs.readFileSync('test-cert.pem')
};

var srv = https.createServer(options, function(req, res){
    setTimeout(function(){
        res.end('Hi, just say hi to you over secure TCP ...\n');
    }, 6000);
});
srv.listen(51680);
console.log('HTTPS server listing on TCP port 51680');

