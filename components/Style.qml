pragma Singleton

import QtQuick

QtObject {
    property bool blackTheme: true
    property QtObject fontMedium: FontLoader { id: _fontMedium; source: "qrc:/fonts/Roboto-Medium.ttf"; }
    property QtObject fontBold: FontLoader { id: _fontBold; source: "qrc:/fonts/Roboto-Bold.ttf"; }
    property QtObject fontLight: FontLoader { id: _fontLight; source: "qrc:/fonts/Roboto-Light.ttf"; }
    property QtObject fontRegular: FontLoader { id: _fontRegular; source: "qrc:/fonts/Roboto-Regular.ttf"; }

    property QtObject fontMonoMedium: FontLoader { id: _fontMonoMedium; source: "qrc:/fonts/RobotoMono-Medium.ttf"; }
    property QtObject fontMonoBold: FontLoader { id: _fontMonoBold; source: "qrc:/fonts/RobotoMono-Bold.ttf"; }
    property QtObject fontMonoLight: FontLoader { id: _fontMonoLight; source: "qrc:/fonts/RobotoMono-Light.ttf"; }
    property QtObject fontMonoRegular: FontLoader { id: _fontMonoRegular; source: "qrc:/fonts/RobotoMono-Regular.ttf"; }

    property string grey: "#3A2766"
    property string orange: "#FF8A3D"
    property string white: "#FFFFFF"
    property string green: "#3CD89F"
    property string moneroGrey: "#2C1D4F"
    property string warningColor: "#FFC46B"

    property string defaultFontColor: blackTheme ? _b_defaultFontColor : _w_defaultFontColor
    property string dimmedFontColor: blackTheme ? _b_dimmedFontColor : _w_dimmedFontColor
    property string lightGreyFontColor: blackTheme ? _b_lightGreyFontColor : _w_lightGreyFontColor
    property string errorColor: blackTheme ? _b_errorColor : _w_errorColor
    property string textSelectionColor: blackTheme ? _b_textSelectionColor : _w_textSelectionColor
    property string textSelectedColor: blackTheme ? _b_textSelectedColor : _w_textSelectedColor

    property string inputBoxBackground: blackTheme ? _b_inputBoxBackground : _w_inputBoxBackground
    property string inputBoxBackgroundDisabled: blackTheme ? _b_inputBoxBackgroundDisabled : _w_inputBoxBackgroundDisabled
    property string inputBoxBackgroundError: blackTheme ? _b_inputBoxBackgroundError : _w_inputBoxBackgroundError
    property string inputBoxColor: blackTheme ? _b_inputBoxColor : _w_inputBoxColor
    property string legacy_placeholderFontColor: blackTheme ? _b_legacy_placeholderFontColor : _w_legacy_placeholderFontColor
    property string inputBorderColorActive: blackTheme ? _b_inputBorderColorActive : _w_inputBorderColorActive
    property string inputBorderColorInActive: blackTheme ? _b_inputBorderColorInActive : _w_inputBorderColorInActive
    property string inputBorderColorInvalid: blackTheme ? _b_inputBorderColorInvalid : _w_inputBorderColorInvalid

    property string buttonBackgroundColor: blackTheme ? _b_buttonBackgroundColor : _w_buttonBackgroundColor
    property string buttonBackgroundColorHover: blackTheme ? _b_buttonBackgroundColorHover : _w_buttonBackgroundColorHover
    property string buttonBackgroundColorDisabled: blackTheme ? _b_buttonBackgroundColorDisabled : _w_buttonBackgroundColorDisabled
    property string buttonBackgroundColorDisabledHover: blackTheme ? _b_buttonBackgroundColorDisabledHover : _w_buttonBackgroundColorDisabledHover
    property string buttonInlineBackgroundColor: blackTheme ? _b_buttonInlineBackgroundColor : _w_buttonInlineBackgroundColor
    property string buttonInlineBackgroundColorHover: blackTheme ? _b_buttonInlineBackgroundColorHover : _w_buttonInlineBackgroundColorHover
    property string buttonTextColor: blackTheme ? _b_buttonTextColor : _w_buttonTextColor
    property string buttonTextColorDisabled: blackTheme ? _b_buttonTextColorDisabled : _w_buttonTextColorDisabled
    property string buttonSecondaryBackgroundColor: blackTheme ? _b_buttonSecondaryBackgroundColor : _w_buttonSecondaryBackgroundColor
    property string buttonSecondaryBackgroundColorHover: blackTheme ? _b_buttonSecondaryBackgroundColorHover : _w_buttonSecondaryBackgroundColorHover
    property string buttonSecondaryTextColor: blackTheme ? _b_buttonSecondaryTextColor : _w_buttonSecondaryTextColor
    property string dividerColor: blackTheme ? _b_dividerColor : _w_dividerColor
    property real dividerOpacity: blackTheme ? _b_dividerOpacity : _w_dividerOpacity

    property string titleBarBackgroundGradientStart: blackTheme ? _b_titleBarBackgroundGradientStart : _w_titleBarBackgroundGradientStart
    property string titleBarBackgroundGradientStop: blackTheme ? _b_titleBarBackgroundGradientStop : _w_titleBarBackgroundGradientStop
    property string titleBarBackgroundBorderColor: blackTheme ? _b_titleBarBackgroundBorderColor : _w_titleBarBackgroundBorderColor
    property string titleBarLogoSource: blackTheme ? _b_titleBarLogoSource : _w_titleBarLogoSource
    property string titleBarMinimizeSource: blackTheme ? _b_titleBarMinimizeSource : _w_titleBarMinimizeSource
    property string titleBarFullscreenSource: blackTheme ? _b_titleBarFullscreenSource : _w_titleBarFullscreenSource
    property string titleBarCloseSource: blackTheme ? _b_titleBarCloseSource : _w_titleBarCloseSource
    property string titleBarButtonHoverColor: blackTheme ? _b_titleBarButtonHoverColor : _w_titleBarButtonHoverColor

    property string wizardBackgroundGradientStart: blackTheme ? _b_wizardBackgroundGradientStart : _w_wizardBackgroundGradientStart
    property string middlePanelBackgroundGradientStart: blackTheme ? _b_middlePanelBackgroundGradientStart : _w_middlePanelBackgroundGradientStart
    property string middlePanelBackgroundGradientStop: blackTheme ? _b_middlePanelBackgroundGradientStop : _w_middlePanelBackgroundGradientStop
    property string middlePanelBackgroundColor: blackTheme ? _b_middlePanelBackgroundColor : _w_middlePanelBackgroundColor
    property string menuButtonFallbackBackgroundColor: blackTheme ? _b_menuButtonFallbackBackgroundColor : _w_menuButtonFallbackBackgroundColor
    property string menuButtonGradientStart: blackTheme ? _b_menuButtonGradientStart : _w_menuButtonGradientStart
    property string menuButtonGradientStop: blackTheme ? _b_menuButtonGradientStop : _w_menuButtonGradientStop
    property string menuButtonTextColor: blackTheme ? _b_menuButtonTextColor : _w_menuButtonTextColor
    property string menuButtonImageRightColorActive: blackTheme ? _b_menuButtonImageRightColorActive : _w_menuButtonImageRightColorActive
    property string menuButtonImageRightColor: blackTheme ? _b_menuButtonImageRightColor : _w_menuButtonImageRightColor
    property string menuButtonImageRightSource: blackTheme ? _b_menuButtonImageRightSource : _w_menuButtonImageRightSource
    property string menuButtonImageDotArrowSource: blackTheme ? _b_menuButtonImageDotArrowSource : _w_menuButtonImageDotArrowSource
    property string inlineButtonTextColor: blackTheme ? _b_inlineButtonTextColor : _w_inlineButtonTextColor
    property string inlineButtonBorderColor: blackTheme ? _b_inlineButtonBorderColor : _w_inlineButtonBorderColor
    property string appWindowBackgroundColor: blackTheme ? _b_appWindowBackgroundColor : _w_appWindowBackgroundColor
    property string appWindowBorderColor: blackTheme ? _b_appWindowBorderColor : _w_appWindowBorderColor
    property bool progressBarProgressTextBold: blackTheme ? _b_progressBarProgressTextBold : _w_progressBarProgressTextBold
    property string progressBarBackgroundColor: blackTheme ? _b_progressBarBackgroundColor : _w_progressBarBackgroundColor
    property string leftPanelBackgroundGradientStart: blackTheme ? _b_leftPanelBackgroundGradientStart : _w_leftPanelBackgroundGradientStart
    property string leftPanelBackgroundGradientStop: blackTheme ? _b_leftPanelBackgroundGradientStop : _w_leftPanelBackgroundGradientStop
    property string historyHeaderTextColor: blackTheme ? _b_historyHeaderTextColor : _w_historyHeaderTextColor
    property var accountColors: blackTheme ? _b_accountColors : _w_accountColors

    property string _b_defaultFontColor: "#FBF6FF"
    property string _b_dimmedFontColor: "#B9A9D6"
    property string _b_lightGreyFontColor: "#D8CCEF"
    property string _b_errorColor: "#FF6B8B"
    property string _b_textSelectionColor: "#3A2766"
    property string _b_textSelectedColor: "#FBF6FF"

    property string _b_inputBoxBackground: "#130B24"
    property string _b_inputBoxBackgroundDisabled: Qt.rgba(255, 255, 255, 0.08)
    property string _b_inputBoxBackgroundError: "#3D182C"
    property string _b_inputBoxColor: "#FBF6FF"
    property string _b_legacy_placeholderFontColor: "#8A7BB0"
    property string _b_inputBorderColorActive: "#99FF8A3D"
    property string _b_inputBorderColorInActive: "#38B9A9D6"
    property string _b_inputBorderColorInvalid: "#99FF6B8B"

    property string _b_buttonBackgroundColor: "#E66A1D"
    property string _b_buttonBackgroundColorHover: "#FF8A3D"
    property string _b_buttonBackgroundColorDisabled: "#3A2C59"
    property string _b_buttonBackgroundColorDisabledHover: "#46356B"
    property string _b_buttonInlineBackgroundColor: "#35235E"
    property string _b_buttonInlineBackgroundColorHover: "#452F78"
    property string _b_buttonTextColor: "#FFFFFF"
    property string _b_buttonTextColorDisabled: "#8A7BB0"
    property string _b_buttonSecondaryBackgroundColor: "#35235E"
    property string _b_buttonSecondaryBackgroundColorHover: "#452F78"
    property string _b_buttonSecondaryTextColor: "#FBF6FF"
    property string _b_dividerColor: "#B9A9D6"
    property real _b_dividerOpacity: 0.16

    property string _b_titleBarBackgroundGradientStart: "#231640"
    property string _b_titleBarBackgroundGradientStop: "#1A1030"
    property string _b_titleBarBackgroundBorderColor: "#34225C"
    property string _b_titleBarLogoSource: "qrc:///images/titlebarLogo.png"
    property string _b_titleBarMinimizeSource: "qrc:///images/minimize.svg"
    property string _b_titleBarFullscreenSource: "qrc:///images/fullscreen.svg"
    property string _b_titleBarCloseSource: "qrc:///images/close.svg"
    property string _b_titleBarButtonHoverColor: "#1AFF8A3D"

    property string _b_wizardBackgroundGradientStart: "#1A1030"
    property string _b_middlePanelBackgroundGradientStart: "#22153D"
    property string _b_middlePanelBackgroundGradientStop: "#150D27"
    property string _b_middlePanelBackgroundColor: "#1A1030"
    property string _b_menuButtonFallbackBackgroundColor: "#12B9A9D6"
    property string _b_menuButtonGradientStart: "#18B9A9D6"
    property string _b_menuButtonGradientStop: "#001A1030"
    property string _b_menuButtonTextColor: "#FBF6FF"
    property string _b_menuButtonImageRightColorActive: "#FF8A3D"
    property string _b_menuButtonImageRightColor: "#B9A9D6"
    property string _b_menuButtonImageRightSource: "qrc:///images/right.svg"
    property string _b_menuButtonImageDotArrowSource: "qrc:///images/arrow-right-medium-white.png"
    property string _b_inlineButtonTextColor: "#FBF6FF"
    property string _b_inlineButtonBorderColor: "#34225C"
    property string _b_appWindowBackgroundColor: "#1A1030"
    property string _b_appWindowBorderColor: "#34225C"
    property bool _b_progressBarProgressTextBold: true
    property string _b_progressBarBackgroundColor: "#2E2052"
    property string _b_leftPanelBackgroundGradientStart: "#20143A"
    property string _b_leftPanelBackgroundGradientStop: "#150D27"
    property string _b_historyHeaderTextColor: "#B9A9D6"
    property var _b_accountColors: ["#3C2272", "#6B2248", "#1E5A4A", "#2A2F7A", "#7A3B1E", "#5E2570", "#1C5268", "#352E4A"]

    property string _w_defaultFontColor: "#1A1030"
    property string _w_dimmedFontColor: "#554773"
    property string _w_lightGreyFontColor: "#6E5F8E"
    property string _w_errorColor: "#D9365E"
    property string _w_textSelectionColor: "#DFD5F2"
    property string _w_textSelectedColor: "#1A1030"

    property string _w_inputBoxBackground: "#FFFFFF"
    property string _w_inputBoxBackgroundDisabled: Qt.rgba(26, 16, 48, 0.08)
    property string _w_inputBoxBackgroundError: "#FFE4EC"
    property string _w_inputBoxColor: "#1A1030"
    property string _w_legacy_placeholderFontColor: "#8A7BB0"
    property string _w_inputBorderColorActive: "#B3E66A1D"
    property string _w_inputBorderColorInActive: "#381A1030"
    property string _w_inputBorderColorInvalid: "#99D9365E"

    property string _w_buttonBackgroundColor: "#E66A1D"
    property string _w_buttonBackgroundColorHover: "#FF8A3D"
    property string _w_buttonBackgroundColorDisabled: "#D6CCE8"
    property string _w_buttonBackgroundColorDisabledHover: "#C7BAE0"
    property string _w_buttonInlineBackgroundColor: "#EAE3F7"
    property string _w_buttonInlineBackgroundColorHover: "#DDD3F0"
    property string _w_buttonTextColor: "#FFFFFF"
    property string _w_buttonTextColorDisabled: "#6E5F8E"
    property string _w_buttonSecondaryBackgroundColor: "#EAE3F7"
    property string _w_buttonSecondaryBackgroundColorHover: "#DDD3F0"
    property string _w_buttonSecondaryTextColor: "#2C1D4F"
    property string _w_dividerColor: "#1A1030"
    property real _w_dividerOpacity: 0.14

    property string _w_titleBarBackgroundGradientStart: "#FAF8FF"
    property string _w_titleBarBackgroundGradientStop: "#F2ECFC"
    property string _w_titleBarBackgroundBorderColor: "#E0D6F2"
    property string _w_titleBarLogoSource: "qrc:///images/themes/white/titlebarLogo.png"
    property string _w_titleBarMinimizeSource: "qrc:///images/themes/white/minimize.svg"
    property string _w_titleBarFullscreenSource: "qrc:///images/themes/white/fullscreen.svg"
    property string _w_titleBarCloseSource: "qrc:///images/themes/white/close.svg"
    property string _w_titleBarButtonHoverColor: "#142C1D4F"

    property string _w_wizardBackgroundGradientStart: "#FAF8FF"
    property string _w_middlePanelBackgroundGradientStart: "#FAF8FF"
    property string _w_middlePanelBackgroundGradientStop: "#F0E9FA"
    property string _w_middlePanelBackgroundColor: "#F5F0FC"
    property string _w_menuButtonFallbackBackgroundColor: "#0D1A1030"
    property string _w_menuButtonGradientStart: "#0A1A1030"
    property string _w_menuButtonGradientStop: "#10FFFFFF"
    property string _w_menuButtonTextColor: "#554773"
    property string _w_menuButtonImageRightSource: "qrc:///images/right.svg"
    property string _w_menuButtonImageRightColorActive: "#E66A1D"
    property string _w_menuButtonImageRightColor: "#8A7BB0"
    property string _w_menuButtonImageDotArrowSource: "qrc:///images/arrow-right-medium-white.png"
    property string _w_inlineButtonTextColor: "#2C1D4F"
    property string _w_inlineButtonBorderColor: "#DFD5F2"
    property string _w_appWindowBackgroundColor: "#FAF8FF"
    property string _w_appWindowBorderColor: "#DFD5F2"
    property bool _w_progressBarProgressTextBold: false
    property string _w_progressBarBackgroundColor: "#241A1030"
    property string _w_leftPanelBackgroundGradientStart: "#FAF8FF"
    property string _w_leftPanelBackgroundGradientStop: "#EFE8FA"
    property string _w_historyHeaderTextColor: "#554773"
    property var _w_accountColors: ["#3C2272", "#3C2272", "#6B2248", "#1E5A4A", "#5E2570", "#2A2F7A", "#7A3B1E", "#1C5268", "#352E4A"]
}
