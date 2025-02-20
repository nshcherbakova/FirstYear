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
    "background-color: rgba(207, 209, 191, 200);"
    "color:white; "
    "font-size: 23px; "
    //   "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-radius: 0;"
    "border-color: white;"
    "border-width: 3;"
    "}"

    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";

static const char *c_progress_dialog_style_str =
    "QProgressDialog{"
    "background-color: rgba(207, 209, 191, 200);"
    "color:#83368A; "
    "font-size: 23px; "
    //  "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-color: #83368A;"
    "border-width: 3;"
    "}";

static const char *c_message_box_style_str =
    "QMessageBox{"
    "background-color: rgba(207, 209, 191, 200);"
    "color:#83368A; "
    "font-size: 23px; "
    //    "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-color: #83368A;"
    "border-width: 3;"
    "}";

static const char *c_drag_and_drop_style_str =
    "QLabel{"
    "color:#BC9F76; "
    "font-size: 23px; "
    //    "font-family: Typo Round Regular Demo;"
    "}";

// Tune Widget styles
static const char *c_dark_button_style_str =
    "QPushButton{"
    "background-color: rgba(255, 255, 255, 200);"
    "color:#555C24; "
    "font-size: 23px; "
    //    "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-radius: 0;"
    "border-color: #555C24;"
    "border-width: 3;"
    "}"

    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";

static const char *c_open_button_style_str =
    "QPushButton{"
    "background-color: rgba(255, 255, 255, 200);"
    "color:#83368A; "
    "font-size: 23px; "
    //  "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-radius: 30;"
    "border-color: #83368A;"
    "border-width: 3;"
    "}"

    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";

static const char *c_open_image_button_style_str =
    "QPushButton{"
    "background-color: rgba(255, 255, 255, 200);"
    "border-style: solid;"
    "border-radius: 30;"
    "border-color: #BC9F76;"
    "border-width: 3;"
    "}"

    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";

static const char *c_select_button_style_str =
    "QPushButton{"
    "background-color: rgba(207, 209, 191, 200);"
    "color:#83368A; "
    "font-size: 23px; "
    //   "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-radius: 30;"
    "border-color: #83368A;"
    "border-width: 3;"
    "}"
    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";

static const char *c_tune_frame_light_color_str = "#E9D5EB";
static const char *c_tune_frame_dark_color_str = "#555C24";
static const char *c_tune_frame_text_color_str = "#53798C";

// Preview Widget styles
static const char *c_preview_background_color_str = "#DBD5EB";
static const char *c_close_button_style_str =
    "QPushButton{"
    "background-color: rgba(255, 255, 255, 200);"
    "color:#BC9F76; "
    "font-size: 23px; "
    //    "font-family: Typo Round Regular Demo;"
    "border-style: solid;"
    "border-radius: 30;"
    "border-color: #BC9F76;"
    "border-width: 3;"
    "}"

    "QPushButton:pressed {border-color: #C3BED1;}"
    "QPushButton:pressed{color: #C3BED1;}";
// Edit window styles
static const char
    *c_line_edit_style_str = "QLineEdit{"
                             "background-color: #F5F5DF;"
                             "color: #83368A;"
                             //" font-family: Typo Round Regular Demo;"
                             "border: none;"
                             "border-radius: 30;"
                             "}";

// Edit window styles
static const char *c_clickable_label_style_str =
    "QLabel{"
    "background-color: rgba(255, 255, 255, 200);"
    "border: none;"
    "border-radius: 20;"
    "}";
static const QColor c_line_edit_background_color = QColor(164, 166, 151, 200);

// Frame
static const char *c_remove_image_str = ":/images/icons/remove";

// General
static const char *c_share_image_str = ":/images/icons/share";
static const char *c_share_button_style_template_str =
    "QPushButton{"
    "background-color: #4C5220;"
    "border-radius: %1;"
    "border:none;"
    "}"

    //  "QPushButton:pressed {border-color: #1C2247);}"
    "QPushButton:pressed {background-color:#2D2052;}";

// Photo widget
static const char *c_photo_widget_background_color = "#FFFBF2";
} // namespace FirstYear::UI

#endif // FIRSTYEAR_STYLES_H
