const net = require('net');

const PIPE_NAME = "mynamedpipe";
const PIPE_PATH = "\\\\.\\pipe\\" + PIPE_NAME;

const L = console.log;

const requests = [];

let responseReceived = undefined;
let requestReceived = undefined;

const client = net.connect(PIPE_PATH, function () {
    L('Client: on connection');

    async function ping() {
        let resp = await makeRequest(":ping");
        L(resp);
        setTimeout(ping, 1000);
    }

    ping();
});

client.on('data', function (data) {
    let dataStr = data.toString();

    if (dataStr == ":quit") {
        L("Quitting!")
        client.write(":quit");
    }

    if (responseReceived) {
        responseReceived(dataStr);
    }
});

client.on('end', function () {
    L('Client: on end');
});

async function sendRequestAndWaitForResponse(request)
{
    const sendPromise = new Promise((resolve, reject) => {
        responseReceived = response => {
            resolve(response);
        };
        client.write(request.message);
    });

    let response = await sendPromise;
    request.onResponse(response);
}

function getRequest()
{
    const prom = new Promise((resolve, reject) => {
        if (requests.length > 0) {
            const req = requests.shift();
            resolve(req);
            return;
        }

        requestReceived = () => {
            requestReceived = undefined;
            const req = requests.shift();
            resolve(req);
        };
    });

    return prom;
}

function makeRequest(request)
{
    const prom = new Promise((resolve, reject) => {
        requests.push({
            message: request,
            onResponse: (response) => {
                resolve(response);
            }
        });

        if (requestReceived) {
            requestReceived();
        }
    });

    return prom;
}

function startProcessing()
{
    async function process()
    {
        while (true) {
            const req = await getRequest();
            sendRequestAndWaitForResponse(req);
        }
    }
    setTimeout(process, 0);
}

startProcessing();

exports.makeRequest = makeRequest;