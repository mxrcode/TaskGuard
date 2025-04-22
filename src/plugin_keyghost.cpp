// KeyGhost Plugin
// A plugin for sending keystrokes to other applications.
// This file is part of KeyGhost.
// Github: https://github.com/mxrcode/KeyGhost/

#include "plugin_keyghost.h"
#include "ui_plugin_keyghost.h"

PluginKeyGhost::PluginKeyGhost(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::PluginKeyGhost),
    m_parent(parent)
{
    ui->setupUi(this);

    const QString software_label = SOFT_NAME + " " + SOFT_VERSION;

    setWindowTitle(tr("KeyGhost") + " | " + software_label);
    setWindowModality(Qt::NonModal);
}

PluginKeyGhost::~PluginKeyGhost()
{
    delete ui;
}

void PluginKeyGhost::closeEvent(QCloseEvent *event)
{
    event->ignore();
    ui->text_typing_plain_text_edit->clear();
    hide();
}

void PluginKeyGhost::open_widget(QString text) {
    if (this->isHidden()) {
        if (!text.isEmpty()) {
            ui->text_typing_plain_text_edit->clear();
            ui->text_typing_plain_text_edit->setPlainText(text);
            ui->text_typing_plain_text_edit->setFocus();
        }

        this->show();
        this->activateWindow();
        this->raise();
    } else {
        this->hide();
    }
}

void PluginKeyGhost::on_text_typing_clicked()
{
#ifdef Q_OS_WIN
    sendKeystroke();
#else
    QMessageBox::warning(this, "Unsupported OS", "This feature is only available on Windows.");
#endif
}

#ifdef Q_OS_WIN
void PluginKeyGhost::sendKeystroke()
{
    QString textToSend = ui->text_typing_plain_text_edit->toPlainText();

    if (textToSend.isEmpty()) {
        QMessageBox::warning(this, "No Text", "Please enter some text first.");
        return;
    }

    // Ask user for typing target
    QMessageBox msgBox;
    msgBox.setText("Click in the target window where you want to type, then press OK.");
    msgBox.exec();

    // Use QTimer instead of Sleep to avoid blocking UI thread
    QTimer::singleShot(1500, this, [this, textToSend]() {
        // Ensure the target application has focus before typing
        HWND foregroundWindow = GetForegroundWindow();
        if (foregroundWindow == nullptr || foregroundWindow == (HWND)this->winId()) {
            QMessageBox::warning(this, "Error", "Failed to detect target window. Please try again.");
            return;
        }

        sendText(textToSend);
    });
}

void PluginKeyGhost::sendText(const QString &text)
{
    // Improved input sending method
    INPUT input;

    // Get current typing delay from settings
    int currentDelay = 30; // Default delay

    // For each character in the string
    for (QChar c : text) {
        // Get virtual key code and scan code for the character
        SHORT vkScan = VkKeyScanW(c.unicode());
        WORD vkCode = vkScan & 0xFF;
        BOOL needShift = vkScan & 0x100;

        if (vkCode == 0xFF) {
            // Character can't be typed with a normal keystroke
            // Use the KEYEVENTF_UNICODE flag for direct Unicode input
            INPUT unicodeInput[2];
            ZeroMemory(unicodeInput, sizeof(unicodeInput));

            // Key down event
            unicodeInput[0].type = INPUT_KEYBOARD;
            unicodeInput[0].ki.wVk = 0;
            unicodeInput[0].ki.wScan = c.unicode();
            unicodeInput[0].ki.dwFlags = KEYEVENTF_UNICODE;

            // Key up event
            unicodeInput[1].type = INPUT_KEYBOARD;
            unicodeInput[1].ki.wVk = 0;
            unicodeInput[1].ki.wScan = c.unicode();
            unicodeInput[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

            SendInput(2, unicodeInput, sizeof(INPUT));
        } else {
            // Normal character input sequence
            std::vector<INPUT> inputs;

            // Press Shift if needed
            if (needShift) {
                ZeroMemory(&input, sizeof(INPUT));
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = VK_SHIFT;
                inputs.push_back(input);
            }

            // Press the key
            ZeroMemory(&input, sizeof(INPUT));
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = vkCode;
            inputs.push_back(input);

            // Release the key
            input.ki.dwFlags = KEYEVENTF_KEYUP;
            inputs.push_back(input);

            // Release Shift if it was pressed
            if (needShift) {
                ZeroMemory(&input, sizeof(INPUT));
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = VK_SHIFT;
                input.ki.dwFlags = KEYEVENTF_KEYUP;
                inputs.push_back(input);
            }

            SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
        }

        // Non-blocking delay between keystrokes
        QThread::msleep(currentDelay);
    }
}
#endif
