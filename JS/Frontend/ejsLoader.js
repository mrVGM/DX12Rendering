const fs = require('fs');
const path = require('path')

const ejsDir = path.join(__dirname, 'ejs');

function readEJSFiles(onReady) {
    const filesData = {};

    function readFile(filePath, onReady) {
        fs.readFile(filePath, (err, data) => {
            let relativePath = path.relative(ejsDir, filePath);
            relativePath = relativePath.replaceAll('\\', '/');
            filesData[relativePath] = data.toString();
            onReady();
        });
    }

    function processDir(dirToProcess, onReady) {
        fs.readdir(dirToProcess, (err, files) => {
            if (files.length === 0) {
                onReady();
                return;
            }

            let toProcess = files.length;
            function callReady() {
                if (toProcess > 0) {
                    return;
                }
                onReady();
            }

            files.forEach(file => {
                const filePath = path.join(dirToProcess, file);
                fs.lstat(filePath, (err, stats) => {
                    if (stats.isFile()) {
                        readFile(filePath, () => {
                            --toProcess;
                            callReady();
                        });
                        return;
                    }
                    if (stats.isDirectory()) {
                        processDir(filePath, () => {
                            --toProcess;
                            callReady();
                        });
                        return;
                    }

                    --toProcess;
                    callReady();
                });
            });
        });
    }

    processDir(ejsDir, () => {
        onReady(filesData);
    });
}

async function loadEJSData() {
    const prom = new Promise((resolve, reject) => {
        readEJSFiles((data) => {
            resolve(data);
        });
    });

    return prom;
}

exports.loadEJSData = loadEJSData;