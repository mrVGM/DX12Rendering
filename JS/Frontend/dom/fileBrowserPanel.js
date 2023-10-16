const loadContent = require('./loadContent');
const contentBrowserController = require('./contentBrowserController');
const controller = contentBrowserController.getContentBrowserController();

{
    const contents = controller.contentBrowser;

    const files = contents.tagged.files;
    const buttons = contents.tagged.buttons;
    const defs = document.appData.defsData;

    let selectedFileEntryElement = undefined;
    
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
        function createFileEntry(def) {
            const fileEntry = loadContent.LoadContentElement('fileEntry.ejs');
            fileEntry.tagged.name.innerHTML = def.name;

            fileEntry.element.addEventListener('click', event => {
                if (selectedFileEntryElement) {
                    selectedFileEntryElement.classList.remove('general-row-selected');
                    selectedFileEntryElement.classList.add('general-row');
                }

                selectedFileEntryElement = fileEntry.element;
                selectedFileEntryElement.classList.remove('general-row');
                selectedFileEntryElement.classList.add('general-row-selected');

                controller.showDetails(def, {
                    renameFile: newName => {
                        def.name = newName;
                        fileEntry.tagged.name.innerHTML = newName;
                    }
                });
            });

            return fileEntry;
        }

        const generatedCategory = createCategory('Generated');
        
        defs.forEach(def => {
            if (def.isNative) {
                return;
            }

            const newFileEntry = createFileEntry(def);
            generatedCategory.tagged.nested.appendChild(newFileEntry.element);
        });

        files.appendChild(generatedCategory.element);
    }

    document.getElementById('content').appendChild(contents.element);
}
