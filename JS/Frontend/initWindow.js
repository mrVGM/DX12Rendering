const ejsLoader = require('./ejsLoader');
const definitionsLoader = require('./definitionsLoader');

async function init() {
    const ejsDataPr = ejsLoader.loadEJSData();
    const defsDataPr = definitionsLoader.loadDefinitions();

    const ejsData = await ejsDataPr;
    const defsData = await defsDataPr;

    const defsMap = {};

    defsData.forEach(def => {
        defsMap[def.id] = def;
    });

    return {
        ejsData: ejsData,
        defsData: defsData,
        defsMap: defsMap
    };
}

exports.init = init;