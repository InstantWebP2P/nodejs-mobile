const udts = require('udts');
const fs = require('fs');

let options = {
    ca: [fs.readFileSync('test-cert.pem')],

    port: 51686,
    host: "localhost",
};

const cln = udts.connect(options, function () {
    console.log('you can type char here, then server send it back:\n');
    process.stdin.resume();
    process.stdin.pipe(cln);
    cln.pipe(process.stdout);
});
