const { LoadContentElement } = require('./loadContent');

function create(propDef) {
    const editor = LoadContentElement('propertyDefaultValueEditor.ejs');

    const { single, array } = editor.tagged;

    const { create } = require('./valueEditor');

    if (propDef.structure === 'single') {
        single.style.display = '';

        const valueEditor = create(propDef.type, value => {
            console.log(value, 'set');
        });

        single.appendChild(valueEditor.element);
    }

    return editor;
}

exports.create = create;;