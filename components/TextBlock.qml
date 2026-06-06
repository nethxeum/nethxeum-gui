import QtQuick 2.9

import "../components" as NethxeumComponents

TextEdit {
    color: NethxeumComponents.Style.defaultFontColor
    font.family: NethxeumComponents.Style.fontRegular.name
    selectionColor: NethxeumComponents.Style.textSelectionColor
    wrapMode: Text.Wrap
    readOnly: true
    selectByMouse: true
    // Workaround for https://bugreports.qt.io/browse/QTBUG-50587
    onFocusChanged: {
        if(focus === false)
            deselect()
    }
}
