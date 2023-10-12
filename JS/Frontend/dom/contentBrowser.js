const loadContent = require('./loadContent');

const elem = loadContent.LoadContentElement('contentBrowser.ejs');
loadContent.AttachToContentRoot(elem);

async function init() {
    let api = document.serverAPI;

    let definitions = await api.requestDefinitionsData();

    const parser = new DOMParser();
    definitions.forEach(def => {
        const xmlDoc = parser.parseFromString(def, "text/xml");
        let defElem = xmlDoc.getElementsByTagName('data_def')[0];
        console.log(defElem);
    });
}

init();