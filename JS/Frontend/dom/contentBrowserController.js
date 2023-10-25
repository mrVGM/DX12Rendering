const controllersAPI = require('./controllersAPI');
const tabsController = require('./tabsController').getTabsController();
const loadContent = require('./loadContent');

function init() {
    const controller = controllersAPI.getControllers().contentBrowser;
    const { defsData, defsMap } = document.appData;

    function addStructFile() {
        const newFile = {
            id: crypto.randomUUID(),
            name: 'New Struct',
            valueType: 'struct',
            isNative: false,
            category: ''
        };

        defsData.push(newFile);
        defsMap[newFile.id] = newFile;
        
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

        defsData.push(newFile);


        
        return newFile;
    }

    function openFile(def) {
        const tabName = def.name;
        const tabKey = {
            fileId: def.id
        };

        if (tabsController.hasTab(tabKey)) {
            tabsController.activate(tabKey);
            return;
        }

        const { createStructEditor } = require('./structEditor');
        const editor = createStructEditor(def);

        const button = loadContent.LoadContentElement('tabButton.ejs');
        tabsController.registerTab(tabKey, button);
        button.data = {
            activate: () => {
                button.element.classList.remove('tab-button-idle');
                button.element.classList.add('tab-button-selected');

                document.getElementById('content').appendChild(editor.element);
            },
            deactivate: () => {
                button.element.classList.remove('tab-button-selected');
                button.element.classList.add('tab-button-idle');

                document.getElementById('content').innerHTML = '';
            }
        };

        button.tagged.name.innerHTML = tabName;
        button.tagged.name.addEventListener('click', event => {
            tabsController.activate(tabKey);
        });

        document.getElementById('header').appendChild(button.element);
    }

    controller.addStructFile = addStructFile;
    controller.addClassFile = addClassFile;
    controller.openFile = openFile;
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