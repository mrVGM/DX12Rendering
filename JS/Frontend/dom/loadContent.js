function LoadContentElement(ejsFile) {
    let tmp = document.createElement('template');

    tmp.innerHTML = document.ejsData[ejsFile];
    const res = tmp.content.firstChild;

    return res;
}

function AttachToContentRoot(element) {
    const contentRoot = document.getElementById('content');

    contentRoot.appendChild(element);
}

exports.LoadContentElement = LoadContentElement;
exports.AttachToContentRoot = AttachToContentRoot;