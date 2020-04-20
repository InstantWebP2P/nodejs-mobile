require('httpp').createServer((req, res) => {
    res.end('Hi, just say hi to you over UDP ...\n');
}).listen(
    51680, 
    256, 
    () => 
    console.log('HTTPP server listing on UDP port 51680')); // backlog as 256

process.on('uncaughtException', (e) => { console.log('HTTPP Server crashed ...'); });