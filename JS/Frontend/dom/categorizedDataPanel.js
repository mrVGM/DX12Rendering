const loadContent = require('./loadContent');

function create() {
    const panel = loadContent.LoadContentElement('categorizedDataPanel.ejs');

    let itemId = 0;
    const itemCategory = {};

    const dummyCategory = loadContent.LoadContentElement('dummyCategory.ejs');

    dummyCategory.data = {
        name: '',
        subcategories: {},
        subelements: 0
    };
    panel.element.appendChild(dummyCategory.element);

    function createCategory(name) {
        let expanded = true;
        const category = loadContent.LoadContentElement('category.ejs');
        const expandIcon = category.tagged.expand_icon;

        const initialDisplay = category.tagged.nested_root.style.display;

        category.tagged.name.innerHTML = name;

        category.tagged.name_row.addEventListener('click', event => {
            if (expanded) {
                category.tagged.nested_root.style.display = 'none';
                expandIcon.classList.remove('expand-button-expanded');
                expandIcon.classList.add('expand-button-collapsed');
            }
            else {
                category.tagged.nested_root.style.display = initialDisplay;
                expandIcon.classList.remove('expand-button-collapsed');
                expandIcon.classList.add('expand-button-expanded');
            }

            expanded = !expanded;
        });

        category.data = {
            name: name,
            subcategories: {},
            subelements: 0,
        };

        return category;
    }

    function addSlot(categoryName) {
        let categoriesList = categoryName.split('/').filter(str => str !== '');

        let parentCat = dummyCategory;
        categoriesList.forEach(cat => {
            let curCat = parentCat.data.subcategories[cat];
            if (!curCat) {
                curCat = createCategory(cat);
                const elem = parentCat.tagged.nested_categories;
                if (elem.childElementCount === 0) {
                    elem.appendChild(curCat.element);
                }
                else {
                    let nextElem = undefined;
                    for (let i = 0; i < elem.childElementCount; ++i) {
                        const curCat = elem.childNodes[i].ejsData.tagged.name.innerHTML;
                        if (cat.toUpperCase() < curCat.toUpperCase()) {
                            nextElem = elem.childNodes[i];
                            break;
                        }
                    }

                    if (!nextElem) {
                        elem.appendChild(curCat.element);
                    }
                    else {
                        elem.insertBefore(curCat.element, nextElem);
                    }
                }

                parentCat.data.subcategories[cat] = curCat;
                ++parentCat.data.subelements;
                curCat.data.parentCat = parentCat;
            }

            parentCat = curCat;
        });

        ++parentCat.data.subelements;
        itemCategory[itemId] = parentCat;

        return {
            slotId: itemId++,
            parentCat: parentCat,
        };
    }

    function removeCategory(category) {
        let parentCat = category.data.parentCat;
        if (!parentCat) {
            return;
        }

        category.element.remove();
        delete parentCat.data.subcategories[category.data.name];
        parentCat.data.subelements--;

        if (parentCat.data.subelements === 0) {
            removeCategory(parentCat);
        }
    }

    function removeSlot(itemId) {
        let category = itemCategory[itemId];
        delete itemCategory[itemId];
        --category.data.subelements;

        if (category.data.subelements === 0) {
            removeCategory(category);
        }
    }

    function addItem(item, name, slotId) {
        if (!item.data) {
            item.data = {};
        }

        item.data.slotId = slotId;
        item.data.itemName = name;

        const cat = itemCategory[slotId];

        const elem = cat.tagged.nested;
        if (elem.childElementCount === 0) {
            elem.appendChild(item);
        }
        else {
            let nextElem = undefined;
            for (let i = 0; i < elem.childElementCount; ++i) {
                const curElem = elem.childNodes[i].data.itemName;
                if (name.toUpperCase() < curElem.toUpperCase()) {
                    nextElem = elem.childNodes[i];
                    break;
                }
            }

            if (!nextElem) {
                elem.appendChild(item);
            }
            else {
                elem.insertBefore(item, nextElem);
            }
        }
    }

    panel.data = {
        addSlot: addSlot,
        removeSlot: removeSlot,
        addItem: addItem
    };

    return panel;
}

exports.create = create;