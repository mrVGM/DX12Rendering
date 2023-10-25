const { LoadContentElement } = require('./loadContent');
const { create: createCategorizedDataPanel  } = require('./categorizedDataPanel');

function createStructEditor(def) {
    const editor = LoadContentElement('structEditor.ejs');
    const categorizedPropertiesPanel = createCategorizedDataPanel();

    const properties = editor.tagged.properties;
    const buttons = editor.tagged.buttons;

    function createProp(propDef) {
        const prop = LoadContentElement('property.ejs');
        const defaultValue = prop.tagged.default_value;
        const categoryInput = prop.tagged.category_input;

        const category = LoadContentElement('category.ejs');
        defaultValue.appendChild(category.element);

        category.tagged.name.innerHTML = 'Default Value';
        const expandIcon = category.tagged.expand_icon;

        const propCat = prop.tagged.category;
        category.tagged.nested.appendChild(propCat);

        let expanded = true;
        function toggle() {
            if (expanded) {
                category.tagged.nested_root.style.display = 'none';
                expandIcon.classList.remove('expand-button-expanded');
                expandIcon.classList.add('expand-button-collapsed');
            }
            else {
                category.tagged.nested_root.style.display = '';
                expandIcon.classList.remove('expand-button-collapsed');
                expandIcon.classList.add('expand-button-expanded');
            }
            expanded = !expanded;
        }

        toggle();
        category.tagged.name_row.addEventListener('click', event => { toggle(); });

        categoryInput.addEventListener('change', event => {
            prop.element.remove();
            categorizedPropertiesPanel.data.removeSlot(prop.data.slot.slotId);

            const slot = categorizedPropertiesPanel.data.addSlot('Properties/' + categoryInput.value);
            prop.data.slot = slot;
            categorizedPropertiesPanel.data.addItem(prop.element, propDef.name, slot.slotId);
        });

        return prop;
    }

    {
        const addPropertyButton = LoadContentElement('button.ejs');
        addPropertyButton.tagged.name.innerHTML = 'Add Property';

        addPropertyButton.element.addEventListener('click', event => {
            const propDef = {
                id: crypto.randomUUID(),
                name: 'New Property',
                category: '',
                access: 'private',
                structure: 'single'
            };

            let cat = 'Properties';
            const slot = categorizedPropertiesPanel.data.addSlot(cat);
            const prop = createProp(propDef);

            categorizedPropertiesPanel.data.addItem(prop.element, propDef.name, slot.slotId);
            prop.data = {
                slot: slot
            };
        });
        buttons.appendChild(addPropertyButton.element);
    }

    properties.appendChild(categorizedPropertiesPanel.element);

    if (!def.properties) {
        def.properties = [];
    }

    def.properties.forEach(propDef => {
        const slot = categorizedPropertiesPanel.data.addSlot('');
        const prop = createProp();
        categorizedPropertiesPanel.data.addItem(prop.element, propDef.name, slot.slotId);
        prop.data = {
            slot: slot
        };
    });

    return editor;
}

exports.createStructEditor = createStructEditor;
