const ejsLoader = require('./ejsLoader');
const definitionsLoader = require('./definitionsLoader');

async function init() {
    const ejsDataPr = ejsLoader.loadEJSData();
    const defsDataPr = definitionsLoader.loadDefinitions();

    const ejsData = await ejsDataPr;
    const defsData = await defsDataPr;

    return {
        ejsData: ejsData,
        defsData: defsData
    };
}

exports.init = init;