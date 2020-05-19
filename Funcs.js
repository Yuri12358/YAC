function createWindow(qmlPath, qmlParent)
{
    var component = Qt.createComponent(qmlPath)
    if (component.status !== Component.Ready) {
        if (component.status === Component.Error)
            console.debug("Error:" + component.errorString())
        return null
    }
    return component.createObject(qmlParent);
}
