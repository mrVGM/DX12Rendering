const controllersAPI = require('./controllersAPI');

function init() {
    const controller = controllersAPI.getControllers().contentBrowser;
    const defs = document.appData.defsData;

    function addStructFile() {
        const newFile = {
            id: crypto.randomUUID(),
            name: 'New Struct',
            valueType: 'struct',
            isNative: false,
            category: ''
        };

        defs.push(newFile);
        
        return newFile;
    }

    function addClassFile() {
        const newFile = {
            id: crypto.randomUUID(),
            name: 'New Class',
            valueType: 'class',
            isNative: false,
            category: ''
        };

        defs.push(newFile);
        
        return newFile;
    }

    controller.addStructFile = addStructFile;
    controller.addClassFile = addClassFile;
}

function getContentBrowserController() {
    const controllers = controllersAPI.getControllers();

    if (!controllers.contentBrowser) {
        controllers.contentBrowser = {};
        init();
    }

    return controllers.contentBrowser;
}

exports.getContentBrowserController = getContentBrowserController;