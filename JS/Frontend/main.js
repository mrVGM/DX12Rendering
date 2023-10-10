const { app, BrowserWindow, ipcMain } = require('electron')
const path = require('path')

let client = require('./pipeServer.js');

async function makeRequestFromWindow(message, id)
{
    let resp = await client.makeRequest(message);
    windowsCreated[id].window.webContents.send('main_channel', {
        subject: 'server_response',
        message: resp
    });
}

ipcMain.on('renderer_channel', (event, data) => {
    if (data.instruction === 'make_request') {
        makeRequestFromWindow(data.message, data.id);
        return;
    }
    console.log(data);
});

let windowId = 0;
let windowsCreated = {};

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

    win.webContents.send('main_channel', {
        subject: 'window_id',
        id: entry.windowId
    });
    setTimeout(() => {
    }, 5000);
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
