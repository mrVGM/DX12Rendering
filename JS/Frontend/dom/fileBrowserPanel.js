const loadContent = require('./loadContent');
const contentBrowserController = require('./contentBrowserController');
const categorizedDataPanel = require('./categorizedDataPanel');

const controller = contentBrowserController.getContentBrowserController();

{
    const contents = controller.contentBrowser;

    const files = contents.tagged.files;
    const buttons = contents.tagged.buttons;
    const defs = document.appData.defsData;

    let selectedFileEntryElement = undefined;
    
    const addStructButton = loadContent.LoadContentElement('button.ejs');
    const addClassButton = loadContent.LoadContentElement('button.ejs');
    {
        addStructButton.tagged.name.innerHTML = 'Add Struct';
        buttons.appendChild(addStructButton.element);

        addClassButton.element.style['margin-left'] = '10px';
        addClassButton.tagged.name.innerHTML = 'Add Class';
        buttons.appendChild(addClassButton.element);
    }

    const filesPanel = categorizedDataPanel.create();
    files.appendChild(filesPanel.element);

    const nativeCategoryName = 'Native';
    const generatedCategoryName = 'Generated';

    {   
        defs.forEach(def => {
            if (!def.isNative) {
                return;
            }

            const slot = filesPanel.data.addSlot(nativeCategoryName + '/' + def.category);
            const fileEntry = loadContent.LoadContentElement('fileEntry.ejs');
            fileEntry.data = {
                slotId: slot.slotId
            };
            fileEntry.tagged.name.innerHTML = def.name;
            filesPanel.data.addItem(fileEntry.element, def.name, slot.slotId);
        });
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

                        const slotId = fileEntry.data.slotId;
                        filesPanel.data.removeSlot(slotId);
                        const newSlot = filesPanel.data.addSlot(generatedCategoryName + '/' + def.category);
                        fileEntry.element.remove();
                        fileEntry.data.slotId = newSlot.slotId;
                        filesPanel.data.addItem(fileEntry.element, def.name, newSlot.slotId);
                    },
                    changeCategory: newCategory => {
                        const slotId = fileEntry.data.slotId;
                        filesPanel.data.removeSlot(slotId);
                        const newSlot = filesPanel.data.addSlot(generatedCategoryName + '/' + newCategory);
                        fileEntry.element.remove();
                        fileEntry.data.slotId = newSlot.slotId;
                        filesPanel.data.addItem(fileEntry.element, def.name, newSlot.slotId);
                        def.category = newCategory;
                    }
                }); 
            });

            fileEntry.element.addEventListener('dblclick', event => {
                controller.openFile(def);
                console.log('open file!');
            });

            return fileEntry;
        }

        
        defs.forEach(def => {
            if (def.isNative) {
                return;
            }

            const slot = filesPanel.data.addSlot(generatedCategoryName + '/' + def.category);
            const fileEntry = createFileEntry(def);
            fileEntry.data = {
                slotId: slot.slotId
            };
            filesPanel.data.addItem(fileEntry.element, def.name, slot.slotId);
        });
        
        addStructButton.element.addEventListener('click', event => {
            const newFile = controller.addStructFile();

            const slot = filesPanel.data.addSlot(generatedCategoryName + '/' + newFile.category);
            const fileEntry = createFileEntry(newFile);
            fileEntry.data = {
                slotId: slot.slotId
            };

            filesPanel.data.addItem(fileEntry.element, newFile.name, slot.slotId);
        });

        addClassButton.element.addEventListener('click', event => {
            const newFile = controller.addClassFile();
            
            const slot = filesPanel.data.addSlot(generatedCategoryName + '/' + newFile.category);
            const fileEntry = createFileEntry(newFile);
            fileEntry.data = {
                slotId: slot.slotId
            };
            filesPanel.data.addItem(fileEntry.element, newFile.name, slot.slotId);
        });
    }
}
