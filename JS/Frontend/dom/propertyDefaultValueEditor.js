const { LoadContentElement } = require('./loadContent');

function create(propDef) {
    const editor = LoadContentElement('propertyDefaultValueEditor.ejs');

    const {
        single,
        array,
        array_add_button: arrayAddButton,
        array_elements: arrayElements
    } = editor.tagged;

    const { create } = require('./valueEditor');

    if (propDef.structure === 'single') {
        single.style.display = '';

        const valueEditor = create(propDef.type, value => {
            console.log(value, 'set');
        });

        single.appendChild(valueEditor.element);
    }

    if (propDef.structure === 'array') {
        array.style.display = '';

        arrayAddButton.addEventListener('click', event => {
            const holder = LoadContentElement('arrayElementValueHolder.ejs');
            const {
                value,
                remove_button: removeButton
            } = holder.tagged;

            const { create } = require('./propertyDefaultValueEditor');

            const prop = {
                type: propDef.type,
                structure: 'single',
            };
            const item = create(prop, (value) => {
                console.log(value);
            });

            removeButton.addEventListener('click', event => {
                console.log('remove');
            });

            arrayElements.appendChild(holder.element);
            value.appendChild(item.element);
        });
    }

    return editor;
}

exports.create = create;;