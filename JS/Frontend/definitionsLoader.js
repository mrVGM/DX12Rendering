const fs = require('fs');
const path = require('path')

const dirPath = path.join(__dirname, '../../Data/scripting');

function readDefinitions(onReady) {
    fs.readdir(dirPath, (err, files) => {
        if (files.length === 0) {
            onReady([]);
            return;
        }

        let toProcess = files.length;
        let res = [];

        function callReady() {
            if (toProcess > 0) {
                return;
            }

            onReady(res);
        }

        files.forEach(file => {
            const filePath = path.join(dirPath, file);

            fs.readFile(filePath, (err, data) => {
                const str = data.toString();
                res.push(str);

                --toProcess;
                callReady();
            });
        });
    });
}

exports.readDefinitions = readDefinitions;