const { app, BrowserWindow, ipcMain } = require('electron')
const path = require('path')

let client = require('./pipeServer.js');

ipcMain.on('renderer_channel', (event, data) => {
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
    ++windowId;
    windowsCreated[windowId] = entry;

    win.on("close", () => {
        console.log("window closed!");
        delete windowsCreated[entry.windowId];
    });

    setTimeout(() => {
        win.webContents.send('main_channel', 'Hello from Main!');
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
