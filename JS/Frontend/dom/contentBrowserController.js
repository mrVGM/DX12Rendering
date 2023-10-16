const controllersAPI = require('./controllersAPI');

function init() {
    const controller = controllersAPI.getControllers().contentBrowser;
    const defs = document.appData.defsData;

    function addFile() {
        const newFile = {
            id: crypto.randomUUID(),
            name: 'New File',
            valueType: 'struct',
            isNative: false
        };

        defs.push(newFile);
        
        return newFile;
    }

    controller.addFile = addFile;
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