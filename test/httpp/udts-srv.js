const udts = require('udts');
let   fs   = require('fs');

var options = {
     key: fs.readFileSync('test-key.pem'),
    cert: fs.readFileSync('test-cert.pem')
};

const srv = udts.createServer(function(socket){
    socket.pipe(socket);     
});

srv.listen(51686);
console.log('Listening on UDP port 51686');

