const loadContent = require('./loadContent');
const contentBrowserController = require('./contentBrowserController');
const controller = contentBrowserController.getContentBrowserController();

{
    const content = controller.contentBrowser;

    const detailsPanel = loadContent.LoadContentElement('fileDetails.ejs');
    content.tagged.details.appendChild(detailsPanel.element);

    const id = detailsPanel.tagged.id;
    const name = detailsPanel.tagged.name;
    const changeName = detailsPanel.tagged.change_name;
    const valueType = detailsPanel.tagged.value_type;
    const isNative = detailsPanel.tagged.is_native;

    function hideDetails() {

    }

    function showDetails(def, api) {
        console.log(api);

        id.innerHTML = def.id;
        name.innerHTML = def.name;
        valueType.innerHTML = def.valueType;
        isNative.innerHTML = def.isNative;

        name.addEventListener('click', event => {
            changeName.value = def.name;
            changeName.style.display = '';
            name.style.display = 'none';
        });

        changeName.addEventListener('change', event => {
            if (changeName.value === '') {
                return;
            }

            api.renameFile(changeName.value);
            
            changeName.style.display = 'none';
            name.style.display = '';
            controller.showDetails(def, api);
        });
    }

    controller.hideDetails = hideDetails;
    controller.showDetails = showDetails;
}