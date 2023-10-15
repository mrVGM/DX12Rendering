const loadContent = require('./loadContent');

{
    const button = loadContent.LoadContentElement('button.ejs');
    
    button.tagged.name.innerHTML = 'Content Browser';
    button.tagged.name.addEventListener('click', event => {
        button.element.classList.add('general-button-selected');
    });
    
    document.getElementById('header').appendChild(button.element);
}

{
    const contents = loadContent.LoadContentElement('contentBrowser.ejs');
    const files = contents.tagged.files;

    const defs = document.appData.defsData;

    defs.forEach(def => {
        const fileEntry = loadContent.LoadContentElement('fileEntry.ejs');
        fileEntry.tagged.name.innerHTML = 'asd';
        files.appendChild(fileEntry.element);
    });
    
    document.getElementById('content').appendChild(contents.element);
}


const elem = loadContent.LoadContentElement('contentBrowser.ejs');