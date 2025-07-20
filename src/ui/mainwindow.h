#pragma once

#include <QtGui/QIcon>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);

  private slots:
    void showDecoder();
    void showUnpacker();
    void showCurlBuilder();
    void goHome();

    // Decoder slots
    void performDecode();
    void clearDecoder();
    void copyDecoderOutput();

    // Unpacker slots
    void performUnpack();
    void clearUnpacker();
    void copyUnpackerOutput();

    // Curl builder slots
    void addHeader();
    void updateCurlCommand();
    void copyCurlCommand();
    void formatJsonBody();

  private:
    void setupUI();
    void setupHomeScreen();
    void setupDecoderScreen();
    void setupUnpackerScreen();
    void setupCurlBuilderScreen();

    QIcon createSquareIcon(const QString &text, const QColor &bgColor);
    bool hasIncompleteHeader();
    void updateAddHeaderButton();
    void updateHeaderValueDropdown(QWidget *headerRow, const QString &headerName);

    // UI Components
    QStackedWidget *stackedWidget;

    // Decoder components
    QComboBox *algorithmCombo;
    QSpinBox *rotSpinBox;
    QTextEdit *decoderInputEdit;
    QTextEdit *decoderOutputEdit;

    // Unpacker components
    QTextEdit *unpackerInputEdit;
    QTextEdit *unpackerOutputEdit;

    // Curl builder components
    QLineEdit *urlLineEdit;
    QComboBox *methodCombo;
    QComboBox *verboseCombo;
    QCheckBox *followRedirectsCheck;
    QCheckBox *insecureCheck;
    QCheckBox *includeHeadersCheck;
    QWidget *headersWidget;
    QVBoxLayout *headersWidgetLayout;
    QPushButton *addHeaderButton;
    QTextEdit *bodyTextEdit;
    QTextEdit *curlCommandEdit;
};