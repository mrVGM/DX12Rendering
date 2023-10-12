const { ipcRenderer } = require('electron');

async function init()
{
    let messageReceived = undefined;

    ipcRenderer.on('main_channel', (event, data) => {
        if (data.subject === 'window_id') {
            document.windowId = data.id;
            requestEJSData();
        }
        else if (data.subject === 'server_response') {
            if (messageReceived) {
                messageReceived(data.message);
            }
        }
        else if (data.subject === 'ejs_data') {
            if (messageReceived) {
                messageReceived(data.ejsData);
            }
        }
    });

    async function makeRequest(message) {
        ipcRenderer.send('renderer_channel', {
            id: document.windowId,
            instruction: 'make_request',
            message: message
        });

        const prom = new Promise((resolve, reject) => {
            messageReceived = function (resp) {
                resolve(resp);
            };
        });

        let resp = await prom;

        console.log(resp);
    }

    let ejsDataReceived = undefined;

    async function requestEJSData() {
        ipcRenderer.send('renderer_channel', {
            id: document.windowId,
            instruction: 'get_ejs_data'
        });

        const prom = new Promise((resolve, reject) => {
            messageReceived = function (resp) {
                resolve(resp);
            };
        });

        let ejsData = await prom;
        document.ejsData = ejsData;
        ejsDataReceived();
    }

    let initProm = new Promise((resolve, reject) => {
        ejsDataReceived = () => {
            resolve({
                makeRequest: makeRequest
            });
        };
    });

    const res = await initProm;
    return res;
}

exports.init = init;