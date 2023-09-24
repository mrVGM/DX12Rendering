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


if (!document.app) {
    document.app = {};
}

document.app.pipeServer = {
    sendMessage: function(message) {
        console.log("No connection!");
    }
};

exports.client = client;