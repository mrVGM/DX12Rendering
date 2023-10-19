const loadContent = require('./loadContent');
const contentBrowserController = require('./contentBrowserController');
const controller = contentBrowserController.getContentBrowserController();
const tabsController = require('./tabsController').getTabsController();

{
    const contents = loadContent.LoadContentElement('contentBrowser.ejs');
    controller.contentBrowser = contents;

    const button = loadContent.LoadContentElement('tabButton.ejs');
    const contentBrowserButtonName = 'Content Browser';
    tabsController.registerTab(contentBrowserButtonName, button);
    button.data = {
        activate: () => {
            button.element.classList.remove('tab-button-idle');
            button.element.classList.add('tab-button-selected');

            document.getElementById('content').appendChild(contents.element);
        },
        deactivate: () => {
            button.element.classList.remove('tab-button-selected');
            button.element.classList.add('tab-button-idle');

            document.getElementById('content').innerHTML = '';
        }
    };

    button.tagged.name.innerHTML = contentBrowserButtonName;
    button.tagged.name.addEventListener('click', event => {
        tabsController.activate(contentBrowserButtonName);
    });
    
    document.getElementById('header').appendChild(button.element);
}

{
    require('./fileBrowserPanel');
    require('./fileDetailsPanel');
}
