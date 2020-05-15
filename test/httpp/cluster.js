'use strict';

var cluster = require('cluster');
var http = require('http');
var numCPUs = require('os').cpus().length;

var i;
if (cluster.isMaster) {
  // Fork workers.
  for (i = 0; i < numCPUs; i++) {
    cluster.fork();
  }

  cluster.on('exit', function(worker, code, signal) {
    console.log('worker ' + worker.process.pid + ' died');
  });
} else {
  console.log('I am worker #' + cluster.worker.id);

  // Workers can share any TCP connection
  // In this case its a HTTP server
  http.createServer(function(req, res) {
    res.writeHead(200);
    res.end("hello world\n");
  }).listen(51680);
}
