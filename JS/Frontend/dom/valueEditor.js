const { LoadContentElement } = require('./loadContent');

function create(type, setter) {
    const editor = LoadContentElement('valueEditor.ejs');

    const { bool, int, float, string } = editor.tagged;

    const { defsMap } = document.appData;

    const def = defsMap[type];

    if (def.name === 'bool') {
        bool.style.display = '';
        bool.addEventListener('change', event => {
            setter(bool.checked);
        });
    }

    if (def.name === 'int') {
        int.style.display = '';

        int.addEventListener('change', event => {
            setter(parseInt(int.value));
        });
    }

    if (def.name === 'float') {
        float.style.display = '';
        float.addEventListener('change', event => {
            setter(parseFloat(float.value));
        });
    }

    if (def.name === 'string') {
        string.style.display = '';
        string.addEventListener('change', event => {
            setter(string.value);
        });
    }

    return editor;
}

exports.create = create;