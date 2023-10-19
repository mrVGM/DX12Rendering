const controllersAPI = require('./controllersAPI');

function init() {
    const controller = controllersAPI.getControllers().tabsController;

    let active;
    const tabs = {};

    controller.registerTab = (id, button) => {
        tabs[id] = button;
    };

    controller.unregisterTab = id => {
        delete tabs[id];
    };

    controller.activate = id => {
        if (active === id) {
            return;
        }

        if (active) {
            tabs[active].data.deactivate();
        }

        tabs[id].data.activate();
        active = id;
    };

    controller.deactivate = () => {
        if (!active) {
            return;
        }

        tabs[active].data.deactivate();
        active = undefined;
    };
}

function getTabsController() {
    const controllers = controllersAPI.getControllers();

    if (!controllers.tabsController) {
        controllers.tabsController = {};
        init();
    }

    return controllers.tabsController;
}

exports.getTabsController = getTabsController;