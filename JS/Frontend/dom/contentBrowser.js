const loadContent = require('./loadContent');
const contentBrowserController = require('./contentBrowserController');
const controller = contentBrowserController.getContentBrowserController();

{
    const button = loadContent.LoadContentElement('tabButton.ejs');
    
    let selected = false;
    function toggleSelected() {
        if (selected) {
            button.element.classList.remove('tab-button-selected');
            button.element.classList.add('tab-button-idle');
        }
        else {
            button.element.classList.remove('tab-button-idle');
            button.element.classList.add('tab-button-selected');
        }

        selected = !selected;
    }

    button.tagged.name.innerHTML = 'Content Browser';
    button.tagged.name.addEventListener('click', event => {
        toggleSelected();
    });
    
    document.getElementById('header').appendChild(button.element);
}

{
    const contents = loadContent.LoadContentElement('contentBrowser.ejs');
    controller.contentBrowser = contents;
    document.getElementById('content').appendChild(contents.element);
    
    require('./fileBrowserPanel');
    require('./fileDetailsPanel');
}
