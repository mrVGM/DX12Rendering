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

async function loadDefinitions() {
    const prom = new Promise((resolve, reject) => {
        readDefinitions(defs => {
            resolve(defs);
        });
    });

    const defsSrc = await prom;

    const parser = new DOMParser();
    const parsed = defsSrc.map(elem => {
        const doc = parser.parseFromString(elem, 'text/xml');
        const dataDef = doc.getElementsByTagName('data_def')[0];

        const res = {};

        {
            const id = dataDef.getElementsByTagName('id')[0];
            const idString = id.innerHTML.substring(1, id.innerHTML.length - 1);
            res.id = idString;
        }

        {
            const name = dataDef.getElementsByTagName('name')[0];
            const nameString = name.innerHTML.substring(1, name.innerHTML.length - 1);
            res.name = nameString;
        }

        {
            const valueType = dataDef.getElementsByTagName('value_type')[0];
            const valueTypeString = valueType.innerHTML.substring(1, valueType.innerHTML.length - 1);
            res.valueType = valueTypeString;
        }

        {
            res.isNative = false;

            const isNativeList = dataDef.getElementsByTagName('is_native');
            if (isNativeList.length > 0) {
                const isNative = isNativeList[0];
                const isNativeValue = parseInt(isNative.innerHTML);
                res.isNative = !!isNativeValue;
            }
        }

        {
            res.category = '';
            const categoryList = dataDef.getElementsByTagName('category');
            if (categoryList.length > 0) {
                const category = categoryList[0];
                const categoryString = valueType.innerHTML.substring(1, category.innerHTML.length - 1);
                res.category = categoryString;
            }
        }

        return res;
    });

    return parsed;
}

exports.loadDefinitions = loadDefinitions;