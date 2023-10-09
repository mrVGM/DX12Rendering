let net = require('net');

let PIPE_NAME = "mynamedpipe";
let PIPE_PATH = "\\\\.\\pipe\\" + PIPE_NAME;

let L = console.log;

var client = net.connect(PIPE_PATH, function() {
    L('Client: on connection');

    function ping()
    {
        client.write(":ping");
        setTimeout(() => {
            ping();
        }, 1000);
    }

    ping();

    setTimeout(() => {
        client.write(":types");
    }, 10000);
})

client.on('data', function (data) {
    let dataStr = data.toString();
    L('Client: on data:', dataStr);

    if (dataStr == ":quit") {
        L("Quitting!")
        client.write(":quit");
    }
});

client.on('end', function() {
    L('Client: on end');
})

exports.client = client;