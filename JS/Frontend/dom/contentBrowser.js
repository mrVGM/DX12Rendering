const loadContent = require('./loadContent');

const elem = loadContent.LoadContentElement('contentBrowser.ejs');
loadContent.AttachToContentRoot(elem);

async function init() {
    let api = document.serverAPI;
    let types = await api.makeRequest(":get_generated");
    types = JSON.parse(types);

    let filesElem = elem.querySelector('div[app-id="files"]');


    for (let i = 0; i < types.length; ++i) {
        let assetInfo = await api.makeRequest(`:get_asset_info ${types[0]}`);
        assetInfo = JSON.parse(assetInfo);
        let fileEntry = loadContent.LoadContentElement('fileEntry.ejs');
        let nameElem = fileEntry.querySelector('div[app-id="name"]');

        nameElem.innerHTML = assetInfo.name;
        filesElem.appendChild(fileEntry);
    }
}

init();