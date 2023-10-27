const { LoadContentElement } = require('./loadContent');

function create(propDef) {
    const panel = LoadContentElement('propertyDefaultValuePanel.ejs');
    const { expand_icon: expandIcon, default_value: defaultValue } = panel.tagged;

    let expanded = true;

    function toggle() {
        if (expanded) {
            panel.tagged.nested_root.style.display = 'none';
            expandIcon.classList.remove('expand-button-expanded');
            expandIcon.classList.add('expand-button-collapsed');
        }
        else {
            panel.tagged.nested_root.style.display = '';
            expandIcon.classList.remove('expand-button-collapsed');
            expandIcon.classList.add('expand-button-expanded');
        }
        expanded = !expanded;
    }

    panel.tagged.name_row.addEventListener('click', event => { toggle(); });

    toggle();

    const { create } = require('./propertyDefaultValueEditor');
    const editor = create(propDef);

    defaultValue.appendChild(editor.element);

    return panel;
}

exports.create = create;