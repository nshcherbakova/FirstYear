#pragma once
#ifndef FIRSTYEAR_STYLES_H
#define FIRSTYEAR_STYLES_H

namespace FirstYear::UI {

/// Main window styles
static const char *c_background_str = "QMainWindow{"
                                      "background-color: #CFD1BF;"
                                      "}";

static const char *c_white_button_style_str =
    "QPushButton{"
    "background-color: rgba(255, 255, 255, 0);"
    "color:white; "
    "font-size: 23px; "
    "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-radius: 0;"
    "border-color: white;"
    "border-width: 3;"
    "}"

    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";

// Tune Widget styles
static const char *c_dark_button_style_str =
    "QPushButton{"
    "background-color: rgba(255, 255, 255, 150);"
    "color:#555C24; "
    "font-size: 23px; "
    "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-radius: 0;"
    "border-color: #555C24;"
    "border-width: 3;"
    "}"

    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";

static const char *c_select_button_style_str =
    "QPushButton{"
    "background-color: rgba(255, 255, 255, 0);"
    "color:#83368A; "
    "font-size: 23px; "
    "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-radius: 30;"
    "border-color: #83368A;"
    "border-width: 3;"
    "}"

    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";

// Preview Widget styles
static const char *c_preview_background_color_str = "#DBD5EB";

// Edit window styles

// General
static const char *c_share_image_str = ":/images/icons/share";
static const char *c_share_button_style_template_str =
    "QPushButton{"
    "background-color: #4C5220;"
    "color: #CFBED1; "
    "font-size: 23px; "
    "font-family: Typo Round Regular Demo;"
    "border-radius: %1;"
    "border:none;"
    "}"

    //  "QPushButton:pressed {border-color: #1C2247);}"
    "QPushButton:pressed {background-color:#2D2052;}";

} // namespace FirstYear::UI

#endif // FIRSTYEAR_STYLES_H
