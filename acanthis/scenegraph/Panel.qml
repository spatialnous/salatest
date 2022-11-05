// Copyright (C) 2021 - 2022 Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Templates as T

import "." as Ui

Page {
    id: panelRoot
    objectName: "panel"
    topPadding: 0
    bottomPadding: 0

    property bool expanded: true
    property T.Popup settingsPopup: null
    property alias settingsPopupToolButton: settingsPopupToolButton

    header: RowLayout {
        objectName: panelRoot.objectName + "Header"
        spacing: 0

        Label {
            objectName: parent.objectName + "TitleLabel"
            text: panelRoot.title
            font.bold: true
            color: Theme.panelTextColour

            Layout.leftMargin: 16
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            id: settingsPopupToolButton
            objectName: panelRoot.objectName + "SettingsToolButton"

            contentItem: Text {
                text: "⚙"
                horizontalAlignment: Text.AlignHCenter
                color: Theme.toolbarButtonTextColour
            }
            focusPolicy: Qt.NoFocus
            visible: settingsPopup

            Layout.preferredWidth: implicitHeight

            onClicked: settingsPopup.open()
            background: Rectangle {
                Layout.fillHeight: true
                implicitWidth: parent.height
                radius: Theme.tabButtonHoverRadius
                color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
            }
        }

        ToolButton {
            objectName: panelRoot.objectName + "HideShowToolButton"
            contentItem: Text {
                text: expanded ? "⯇" : "⯆"
                horizontalAlignment: Text.AlignHCenter
                color: Theme.toolbarButtonTextColour
            }
            focusPolicy: Qt.NoFocus

            Layout.leftMargin: -8
            Layout.preferredWidth: implicitHeight

            onClicked: expanded = !expanded
            background: Rectangle {
                Layout.fillHeight: true
                implicitWidth: parent.height
                radius: Theme.tabButtonHoverRadius
                color: parent.hovered ? Theme.toolbarButtonHoverColour : Theme.toolbarButtonColour
            }
        }
    }
}
