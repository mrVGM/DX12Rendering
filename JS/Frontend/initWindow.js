const { ipcRenderer } = require('electron');

async function init()
{
    let messageReceived = undefined;

    ipcRenderer.on('main_channel', (event, data) => {
        if (messageReceived) {
            messageReceived(data);
            return;
        }

        console.log(data);
    });

    async function getMyId() {
        const prom = new Promise((resolve, reject) => {
            messageReceived = function (resp) {
                messageReceived = undefined;
                resolve(resp);
            };
        });

        ipcRenderer.send('renderer_channel', {
            instruction: 'get_init_data'
        });

        let data = await prom;
        document.windowId = data.id;
    }

    async function requestEJSData() {
        const prom = new Promise((resolve, reject) => {
            messageReceived = function (resp) {
                messageReceived = undefined;
                resolve(resp);
            };
        });

        ipcRenderer.send('renderer_channel', {
            id: document.windowId,
            instruction: 'get_ejs_data'
        });


        let data = await prom;
        document.ejsData = data.ejsData;
    }

    async function requestDefinitionsData() {
        const prom = new Promise((resolve, reject) => {
            messageReceived = function (resp) {
                messageReceived = undefined;
                resolve(resp);
            };
        });

        ipcRenderer.send('renderer_channel', {
            id: document.windowId,
            instruction: 'get_definitions'
        });


        let data = await prom;
        return data.definitions;
    }

    await getMyId();
    await requestEJSData();

    return {
        requestDefinitionsData: requestDefinitionsData
    };
}

exports.init = init;