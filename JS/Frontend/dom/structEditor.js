const { LoadContentElement } = require('./loadContent');
const { create: createCategorizedDataPanel } = require('./categorizedDataPanel');
const { openModal, closeModal } = require('./modalUtils');
function createStructEditor(def) {
    const editor = LoadContentElement('structEditor.ejs');
    const categorizedPropertiesPanel = createCategorizedDataPanel();

    const properties = editor.tagged.properties;
    const buttons = editor.tagged.buttons;

    const { defsData, defsMap } = document.appData;

    function choseType() {
        let onChosen;

        const entries = defsData.map(def => {
            let cat = 'Generated/';
            if (def.isNative) {
                cat = 'Native/';
            }
            
            const entry = {
                category: cat + def.category,
                name: def.name,
                chosen: () => {
                    onChosen(def);
                }
            };

            return entry;
        });

        const prom = new Promise((resolve, reject) => {
            onChosen = def => {
                resolve(def);
            };
        });

        openModal(entries);

        return prom;
    }

    function createProp(propDef) {
        const prop = LoadContentElement('property.ejs');
        const defaultValue = prop.tagged.default_value;
        const type = prop.tagged.type;
        const secondaryType = prop.tagged.secondary_type;
        const arrow = prop.tagged.arrow;
        const access = prop.tagged.access;
        const structure = prop.tagged.structure;

        {
            const propName = prop.tagged.name;
            const changeName = prop.tagged.change_name;

            propName.addEventListener('click', event => {
                propName.style.display = 'none';
                changeName.style.display = '';
                changeName.value = propDef.name;
            });

            changeName.addEventListener('change', event => {
                const newName = changeName.value.trim();

                if (newName.length > 0) {
                    propDef.name = newName;
                    propName.innerHTML = newName;

                    propName.style.display = '';
                    changeName.style.display = 'none';


                    categorizedPropertiesPanel.data.removeSlot(prop.element.data.slotId);
                    const slot = categorizedPropertiesPanel.data.addSlot('Properties/' + propDef.category);
                    prop.data.slot = slot;
                    categorizedPropertiesPanel.data.addItem(prop.element, propDef.name, slot.slotId);
                }
            });
        }

        {
            type.innerHTML = defsMap[propDef.type].name;
            type.addEventListener('click', async event => {
                let chosen = await choseType();
                propDef.type = chosen.id;
                type.innerHTML = defsMap[propDef.type].name;

                closeModal();

                createDefaultValuePanel();
            });
        }

        {
            secondaryType.innerHTML = defsMap[propDef.secondaryType].name;
            secondaryType.addEventListener('click', async event => {
                let chosen = await choseType();
                propDef.secondaryType = chosen.id;
                secondaryType.innerHTML = defsMap[propDef.secondaryType].name;

                closeModal();

                createDefaultValuePanel();
            });
        }

        {
            access.innerHTML = propDef.access;

            function* accessIt(initial) {
                const accessTypes = [
                    'private',
                    'protected',
                    'public'
                ];
                let i = accessTypes.indexOf(initial);

                while (true) {
                    i = (i + 1) % accessTypes.length;
                    yield accessTypes[i];
                }
            }

            const it = accessIt(propDef.access);
            access.addEventListener('click', event => {
                const nextAccessType = it.next().value;
                propDef.access = nextAccessType;
                access.innerHTML = nextAccessType;
            });
        }

        {
            structure.innerHTML = propDef.structure;
            secondaryType.style.display = propDef.structure === 'map' ? '' : 'none';
            arrow.style.display = propDef.structure === 'map' ? '' : 'none';

            function* structureIt(initial) {
                const structureTypes = [
                    'single',
                    'array',
                    'set',
                    'map'
                ];
                let i = structureTypes.indexOf(initial);

                while (true) {
                    i = (i + 1) % structureTypes.length;
                    yield structureTypes[i];
                }
            }

            const it = structureIt(propDef.structure);
            structure.addEventListener('click', event => {
                const nextStructureType = it.next().value;
                propDef.structure = nextStructureType;
                structure.innerHTML = propDef.structure;

                secondaryType.style.display = propDef.structure === 'map' ? '' : 'none';
                arrow.style.display = propDef.structure === 'map' ? '' : 'none';

                createDefaultValuePanel();
            });
        }

        function createDefaultValuePanel() {
            const { create } = require('./propertyDefaultValuePanel');
            const panel = create(propDef);

            defaultValue.innerHTML = '';
            defaultValue.appendChild(panel.element);

            const category = panel.tagged.category;
            category.addEventListener('change', event => {
                prop.element.remove();
                categorizedPropertiesPanel.data.removeSlot(prop.data.slot.slotId);

                propDef.category = category.value;
                const slot = categorizedPropertiesPanel.data.addSlot('Properties/' + category.value);
                prop.data.slot = slot;
                categorizedPropertiesPanel.data.addItem(prop.element, propDef.name, slot.slotId);
            });
        }

        createDefaultValuePanel();

        return prop;
    }

    {
        const addPropertyButton = LoadContentElement('button.ejs');
        addPropertyButton.tagged.name.innerHTML = 'Add Property';

        addPropertyButton.element.addEventListener('click', async event => {
            const type = await choseType();
            closeModal();

            const propDef = {
                id: crypto.randomUUID(),
                name: 'New Property',
                category: '',
                access: 'private',
                type: type.id,
                secondaryType: type.id,
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
        const slot = categorizedPropertiesPanel.data.addSlot('Properties/' + propDef.category);
        const prop = createProp();
        categorizedPropertiesPanel.data.addItem(prop.element, propDef.name, slot.slotId);
        prop.data = {
            slot: slot
        };
    });

    return editor;
}

exports.createStructEditor = createStructEditor;
