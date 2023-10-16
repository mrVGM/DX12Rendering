function getControllers() {
    if (!document.controllers) {
        document.controllers = {};
    }

    const controllers = document.controllers;
    return controllers;
}

exports.getControllers = getControllers;