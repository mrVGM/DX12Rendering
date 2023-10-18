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

        category.data = {
            name: name,
            subcategories: {},
            subelements: 0,
        };

        if (!document.cats) {
            document.cats = [];
        }
        document.cats.push(category);

        return category;
    }

    function addSlot(categoryName) {
        let categoriesList = categoryName.split('/').filter(str => str !== '');

        let parentCat = dummyCategory;
        categoriesList.forEach(cat => {
            let curCat = parentCat.data.subcategories[cat];
            if (!curCat) {
                curCat = createCategory(cat);
                parentCat.tagged.nested.appendChild(curCat.element);
                parentCat.data.subcategories[cat] = curCat;
                ++parentCat.data.subelements;
                curCat.data.parentCat = parentCat;
            }

            parentCat = curCat;
        });

        ++parentCat.data.subelements;
        itemCategory[itemId] = parentCat;

        return {
            itemId: itemId++,
            parentCat: parentCat,
        };
    }

    function removeCategory(category) {
        let parentCat = category.data.parentCat;
        if (!parentCat) {
            return;
        }

        category.element.remove();
        delete parentCat[category.data.name];
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

    panel.data = {
        addSlot: addSlot,
        removeSlot: removeSlot
    };

    return panel;
}

exports.create = create;