'use strict';

const tls = require('tls');
let   fs   = require('fs');

var options = {
     key: fs.readFileSync('test-key.pem'),
    cert: fs.readFileSync('test-cert.pem')
};

const srv = tls.createServer(function(socket){
    socket.pipe(socket);     
});

srv.listen(51686);
console.log('Listening on TCP port 51686');

