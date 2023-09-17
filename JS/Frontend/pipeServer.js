let net = require('net');

let PIPE_NAME = "mynamedpipe";
let PIPE_PATH = "\\\\.\\pipe\\" + PIPE_NAME;

let L = console.log;

var client = net.connect(PIPE_PATH, function() {
    L('Client: on connection');
})

client.on('data', function(data) {
    L('Client: on data:', data.toString());
    client.end('Thanks!');
});

client.on('end', function() {
    L('Client: on end');
})


if (!document.app) {
    document.app = {};
}

document.app.pipeServer = {
    sendMessage: function(message) {
        console.log("No connection!");
    }
};

exports.client = client;