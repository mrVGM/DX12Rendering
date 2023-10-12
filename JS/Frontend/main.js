const { app, BrowserWindow, ipcMain } = require('electron')

const ejsLoader = require('./ejsLoader');
const definitionsLoader = require('./definitionsLoader');

let ejsData = undefined;
let definitions = undefined;

ejsLoader.readEJSFiles(data => {
    ejsData = data;
});

definitionsLoader.readDefinitions(data => {
    definitions = data;
});

async function getEJSData(id) {
    function sendEJSData(data) {
        windowsCreated[id].window.webContents.send('main_channel', {
            subject: 'ejs_data',
            ejsData: data
        });
    }

    if (ejsData) {
        sendEJSData(ejsData);
        return;
    }

    const prom = new Promise((resolve, reject) => {
        function check() {
            if (ejsData) {
                resolve(ejsData);
                return;
            }

            setTimeout(check, 100);
        }

        check();
    });

    const data = await prom;

    sendEJSData(data);
}

async function getDefinitionsData(id) {
    function sendEJSData(data) {
        windowsCreated[id].window.webContents.send('main_channel', {
            subject: 'definitions_data',
            definitions: data
        });
    }

    if (definitions) {
        sendEJSData(definitions);
        return;
    }

    const prom = new Promise((resolve, reject) => {
        function check() {
            if (definitions) {
                resolve(definitions);
                return;
            }

            setTimeout(check, 100);
        }

        check();
    });

    const data = await prom;

    sendEJSData(data);
}

ipcMain.on('renderer_channel', (event, data) => {
    if (data.instruction === 'get_init_data') {
        for (let i in windowsCreated) {
            windowsCreated[i].window.webContents.send('main_channel', {
                subject: 'window_id',
                id: i
            });
        }

        return;
    }

    if (data.instruction === 'get_ejs_data') {
        getEJSData(data.id);
        return;
    }

    if (data.instruction === 'get_definitions') {
        getDefinitionsData(data.id);
        return;
    }

    console.log(data);
});

let windowId = 0;
const windowsCreated = {};

function createWindow () {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            contextIsolation: false,
            nodeIntegration: true
        }
    });


    win.loadFile('index.html');
    let entry = {
        windowId: windowId,
        window: win
    };
    windowsCreated[windowId++] = entry;

    win.on("close", () => {
        console.log("window closed!");
        delete windowsCreated[entry.windowId];
    });
}

app.whenReady().then(() => {
    createWindow();

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow();
        }
    });
})

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});
