import QtQuick 2.9

import "." as NethxeumComponents
import "effects/" as NethxeumEffects

Rectangle {
    color: NethxeumComponents.Style.appWindowBorderColor
    height: 1

    NethxeumEffects.ColorTransition {
        targetObj: parent
        blackColor: NethxeumComponents.Style._b_appWindowBorderColor
        whiteColor: NethxeumComponents.Style._w_appWindowBorderColor
    }
}
