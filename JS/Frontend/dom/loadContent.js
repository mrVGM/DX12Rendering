const ejs = require('ejs');

function LoadContentElement(ejsFile, data) {
    const src = document.appData.ejsData[ejsFile];
    let rendered = ejs.render(src, data);

    let tmp = document.createElement('template');
    tmp.innerHTML = rendered;

    const elem = tmp.content.firstChild;

    function* query(elem) {
        if (elem.getAttribute) {
            const appId = elem.getAttribute('app-id');
            if (appId !== null) {
                yield {
                    appId: appId,
                    elem: elem
                }
            }
        }

        for (let i = 0; i < elem.childNodes.length; ++i) {
            const child = elem.childNodes[i];
            const it = query(child);

            let cur = it.next();
            while (!cur.done) {
                yield cur.value;
                cur = it.next();
            }
        }
    }

    let res = {
        element: elem,
        tagged: {}
    };

    const it = query(elem);
    let cur = it.next();
    while (!cur.done) {
        let val = cur.value;
        res.tagged[val.appId] = val.elem;

        cur = it.next();
    }

    res.element.ejsData = res;

    return res;
}

exports.LoadContentElement = LoadContentElement;