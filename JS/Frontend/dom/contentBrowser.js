const loadContent = require('./loadContent');
const contentBrowserController = require('./contentBrowserController');

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
    const files = contents.tagged.files;
    const buttons = contents.tagged.buttons;

    const defs = document.appData.defsData;

    {
        const button = loadContent.LoadContentElement('button.ejs');
        button.tagged.name.innerHTML = 'Add';
        buttons.appendChild(button.element);

        const controller = contentBrowserController.getContentBrowserController();
        button.element.addEventListener('click', event => {
            const newFile = controller.addFile();

            const fileEntry = loadContent.LoadContentElement('fileEntry.ejs');
            fileEntry.tagged.name.innerHTML = newFile.name;
            files.appendChild(fileEntry.element);
        });
    }

    function createCategory(name) {
        let expanded = true;
        const category = loadContent.LoadContentElement('fileCategory.ejs');
        const expandIcon = category.tagged.expand_icon;

        const initialDisplay = category.tagged.nested.style.display;

        category.tagged.name.innerHTML = name;
        
        category.tagged.name_row.addEventListener('click', event => {
            if (expanded) {
                category.tagged.nested.style.display = 'none';
                expandIcon.classList.remove('expand-button-expanded');
                expandIcon.classList.add('expand-button-collapsed');
            }
            else {
                category.tagged.nested.style.display = initialDisplay;
                expandIcon.classList.remove('expand-button-collapsed');
                expandIcon.classList.add('expand-button-expanded');
            }

            expanded = !expanded;
        });

        return category;
    }

    {
        const nativeCategory = createCategory('Native');
        
        defs.forEach(def => {
            if (!def.isNative) {
                return;
            }

            const fileEntry = loadContent.LoadContentElement('fileEntry.ejs');
            fileEntry.tagged.name.innerHTML = def.name;
            nativeCategory.tagged.nested.appendChild(fileEntry.element);
        });

        files.appendChild(nativeCategory.element);
    }

    {
        const generatedCategory = createCategory('Generated');
        
        defs.forEach(def => {
            if (def.isNative) {
                return;
            }

            const fileEntry = loadContent.LoadContentElement('fileEntry.ejs');
            fileEntry.tagged.name.innerHTML = def.name;
            generatedCategory.tagged.nested.appendChild(fileEntry.element);
        });

        files.appendChild(generatedCategory.element);
    }

    document.getElementById('content').appendChild(contents.element);
}


const elem = loadContent.LoadContentElement('contentBrowser.ejs');