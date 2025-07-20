#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtGui/QClipboard>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QScrollArea>
#include <QtCore/QTimer>

#include "decoder.h"
#include "unpacker.h"
#include "curl_builder.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        setWindowTitle("dave.");
        resize(900, 700);
    }

private slots:
    void showDecoder()
    {
        stackedWidget->setCurrentIndex(1);
    }
    
    void showUnpacker()
    {
        stackedWidget->setCurrentIndex(2);
    }
    
    void showCurlBuilder()
    {
        stackedWidget->setCurrentIndex(3);
    }
    
    void goHome()
    {
        stackedWidget->setCurrentIndex(0);
    }

    // Decoder slots
    void performDecode()
    {
        QString input = decoderInputEdit->toPlainText().trimmed();
        if (input.isEmpty()) {
            decoderOutputEdit->clear();
            return;
        }
        
        QString result;
        int algorithm = algorithmCombo->currentIndex();
        
        switch (algorithm) {
            case 0: // Base64
                result = Decoder::decode(input, Decoder::Base64);
                break;
            case 1: // Hex
                result = Decoder::decode(input, Decoder::Hex);
                break;
            case 2: // ROT/Caesar
                result = Decoder::decode(input, Decoder::ROT, rotSpinBox->value());
                break;
        }
        
        decoderOutputEdit->setPlainText(result);
    }
    
    void clearDecoder()
    {
        decoderInputEdit->clear();
        decoderOutputEdit->clear();
    }
    
    void copyDecoderOutput()
    {
        QString output = decoderOutputEdit->toPlainText();
        if (!output.isEmpty()) {
            QApplication::clipboard()->setText(output);
            QMessageBox::information(this, "Copied", "Output copied to clipboard!");
        }
    }

    // Unpacker slots
    void performUnpack()
    {
        QString input = unpackerInputEdit->toPlainText().trimmed();
        if (input.isEmpty()) {
            unpackerOutputEdit->clear();
            return;
        }
        
        QString result = Unpacker::deobfuscateJavaScript(input);
        QString formatted = Unpacker::beautifyJavaScript(result);
        unpackerOutputEdit->setPlainText(formatted);
    }
    
    void clearUnpacker()
    {
        unpackerInputEdit->clear();
        unpackerOutputEdit->clear();
    }
    
    void copyUnpackerOutput()
    {
        QString output = unpackerOutputEdit->toPlainText();
        if (!output.isEmpty()) {
            QApplication::clipboard()->setText(output);
            QMessageBox::information(this, "Copied", "Output copied to clipboard!");
        }
    }

    // Curl builder slots
    void addHeader()
    {
        // Don't add new header if there's an incomplete one
        if (hasIncompleteHeader()) {
            QMessageBox::information(this, "Complete Headers", "Please fill in all header fields before adding a new one.");
            return;
        }
        
        QWidget *headerRow = new QWidget();
        QHBoxLayout *headerLayout = new QHBoxLayout(headerRow);
        
        QComboBox *headerKey = new QComboBox();
        headerKey->setEditable(true);
        headerKey->addItems({
            "Content-Type",
            "Authorization", 
            "Accept",
            "User-Agent",
            "Accept-Encoding",
            "Accept-Language",
            "Cache-Control",
            "Connection",
            "Cookie",
            "Host",
            "Referer",
            "X-Requested-With",
            "X-API-Key",
            "X-Auth-Token",
            "If-Modified-Since",
            "If-None-Match"
        });
        headerKey->setCurrentText("");
        headerKey->lineEdit()->setPlaceholderText("Header name");
        connect(headerKey, &QComboBox::currentTextChanged, [this, headerRow](const QString &text) {
            updateHeaderValueDropdown(headerRow, text);
            updateCurlCommand();
            updateAddHeaderButton();
        });
        
        QComboBox *headerValue = new QComboBox();
        headerValue->setEditable(true);
        headerValue->lineEdit()->setPlaceholderText("Header value");
        connect(headerValue, &QComboBox::currentTextChanged, [this]() {
            updateCurlCommand();
            updateAddHeaderButton();
        });
        
        QPushButton *removeButton = new QPushButton("✕");
        removeButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 4px 8px; border: none; border-radius: 4px; font-weight: bold; min-width: 20px; max-width: 20px; }");
        connect(removeButton, &QPushButton::clicked, [this, headerRow]() {
            headerRow->setParent(nullptr);
            headerRow->deleteLater();
            QTimer::singleShot(0, [this]() {
                updateCurlCommand();
                updateAddHeaderButton();
            });
        });
        
        headerLayout->addWidget(headerKey, 2);
        headerLayout->addWidget(headerValue, 3);
        headerLayout->addWidget(removeButton, 0);
        
        // Insert before the stretch
        headersWidgetLayout->insertWidget(headersWidgetLayout->count() - 1, headerRow);
        
        updateCurlCommand();
        updateAddHeaderButton();
    }

    void updateCurlCommand()
    {
        CurlBuilder::CurlOptions options;
        
        // Get URL
        if (urlLineEdit) {
            options.url = urlLineEdit->text();
        }
        
        // Get method
        if (methodCombo) {
            options.method = static_cast<CurlBuilder::HttpMethod>(methodCombo->currentIndex());
        }
        
        // Get verbose level
        if (verboseCombo) {
            options.verbose = static_cast<CurlBuilder::VerboseLevel>(verboseCombo->currentIndex());
        }
        
        // Get flags
        if (followRedirectsCheck) {
            options.followRedirects = followRedirectsCheck->isChecked();
        }
        if (insecureCheck) {
            options.insecure = insecureCheck->isChecked();
        }
        if (includeHeadersCheck) {
            options.includeResponseHeaders = includeHeadersCheck->isChecked();
        }
        
        // Get headers
        if (headersWidget) {
            for (int i = 0; i < headersWidgetLayout->count() - 1; i++) {
                QWidget *widget = headersWidgetLayout->itemAt(i)->widget();
                if (widget) {
                    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(widget->layout());
                    if (layout && layout->count() >= 3) {
                        QComboBox *keyCombo = qobject_cast<QComboBox*>(layout->itemAt(0)->widget());
                        QComboBox *valueCombo = qobject_cast<QComboBox*>(layout->itemAt(1)->widget());
                        if (keyCombo && valueCombo && !keyCombo->currentText().isEmpty() && !valueCombo->currentText().isEmpty()) {
                            options.headers.append({keyCombo->currentText(), valueCombo->currentText()});
                        }
                    }
                }
            }
        }
        
        // Get body
        if (bodyTextEdit) {
            options.body = bodyTextEdit->toPlainText();
        }
        
        QString command = CurlBuilder::buildCurlCommand(options);
        
        if (curlCommandEdit) {
            curlCommandEdit->setPlainText(command);
        }
    }
    
    void copyCurlCommand()
    {
        QString command = curlCommandEdit->toPlainText();
        if (!command.isEmpty()) {
            QApplication::clipboard()->setText(command);
            QMessageBox::information(this, "Copied", "Curl command copied to clipboard!");
        }
    }

    void formatJsonBody()
    {
        QString text = bodyTextEdit->toPlainText();
        if (text.isEmpty()) return;
        
        QString formatted = Unpacker::formatJson(text);
        if (!formatted.isEmpty()) {
            bodyTextEdit->setPlainText(formatted);
        } else {
            QMessageBox::warning(this, "JSON Format", "Could not format JSON. Please check syntax.");
        }
    }

private:
    QIcon createSquareIcon(const QString &text, const QColor &bgColor)
    {
        QPixmap pixmap(128, 128);
        pixmap.fill(bgColor);
        
        QPainter painter(&pixmap);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(pixmap.rect(), Qt::AlignCenter | Qt::TextWordWrap, text);
        
        return QIcon(pixmap);
    }
    
    void setupUI()
    {
        stackedWidget = new QStackedWidget(this);
        setCentralWidget(stackedWidget);
        
        setupHomeScreen();
        setupDecoderScreen();
        setupUnpackerScreen();
        setupCurlBuilderScreen();
    }

    void setupHomeScreen()
    {
        QWidget *homeWidget = new QWidget();
        QVBoxLayout *homeLayout = new QVBoxLayout(homeWidget);
        
        QLabel *titleLabel = new QLabel("dave.");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 48px; font-weight: bold; margin: 30px; color: #d32f2f; font-family: 'Georgia', serif;");
        homeLayout->addWidget(titleLabel);
        
        QHBoxLayout *toolsLayout = new QHBoxLayout();
        
        QPushButton *decoderButton = new QPushButton();
        decoderButton->setIcon(createSquareIcon("DECODER\n\nBase64\nHex\nROT/Caesar", QColor("#4CAF50")));
        decoderButton->setIconSize(QSize(128, 128));
        decoderButton->setFixedSize(150, 150);
        decoderButton->setStyleSheet("QPushButton { border: 2px solid #ddd; border-radius: 8px; } QPushButton:hover { border-color: #4CAF50; }");
        connect(decoderButton, &QPushButton::clicked, this, &MainWindow::showDecoder);
        
        QPushButton *unpackerButton = new QPushButton();
        unpackerButton->setIcon(createSquareIcon("UNPACKER\n\nJavaScript\nDeobfuscation", QColor("#FF9800")));
        unpackerButton->setIconSize(QSize(128, 128));
        unpackerButton->setFixedSize(150, 150);
        unpackerButton->setStyleSheet("QPushButton { border: 2px solid #ddd; border-radius: 8px; } QPushButton:hover { border-color: #FF9800; }");
        connect(unpackerButton, &QPushButton::clicked, this, &MainWindow::showUnpacker);
        
        QPushButton *curlButton = new QPushButton();
        curlButton->setIcon(createSquareIcon("CURL\nBUILDER\n\nHTTP\nRequests", QColor("#9C27B0")));
        curlButton->setIconSize(QSize(128, 128));
        curlButton->setFixedSize(150, 150);
        curlButton->setStyleSheet("QPushButton { border: 2px solid #ddd; border-radius: 8px; } QPushButton:hover { border-color: #9C27B0; }");
        connect(curlButton, &QPushButton::clicked, this, &MainWindow::showCurlBuilder);
        
        toolsLayout->addStretch();
        toolsLayout->addWidget(decoderButton);
        toolsLayout->addSpacing(30);
        toolsLayout->addWidget(unpackerButton);
        toolsLayout->addSpacing(30);
        toolsLayout->addWidget(curlButton);
        toolsLayout->addStretch();
        
        homeLayout->addLayout(toolsLayout);
        homeLayout->addStretch();
        
        stackedWidget->addWidget(homeWidget);
    }
    
    void setupDecoderScreen()
    {
        QWidget *decoderWidget = new QWidget();
        QVBoxLayout *decoderLayout = new QVBoxLayout(decoderWidget);
        
        QPushButton *backButton = new QPushButton("← Back to Home");
        backButton->setStyleSheet("QPushButton { background-color: #666; color: white; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #555; }");
        connect(backButton, &QPushButton::clicked, this, &MainWindow::goHome);
        decoderLayout->addWidget(backButton);
        
        QLabel *titleLabel = new QLabel("Decoder Tool");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin: 10px;");
        decoderLayout->addWidget(titleLabel);
        
        QHBoxLayout *algorithmLayout = new QHBoxLayout();
        QLabel *algorithmLabel = new QLabel("Algorithm:");
        algorithmLabel->setStyleSheet("font-weight: bold;");
        algorithmCombo = new QComboBox();
        algorithmCombo->addItems({"Base64", "Hex", "ROT/Caesar"});
        
        rotSpinBox = new QSpinBox();
        rotSpinBox->setRange(1, 25);
        rotSpinBox->setValue(13);
        rotSpinBox->setPrefix("Shift: ");
        rotSpinBox->setVisible(false);
        
        connect(algorithmCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
            rotSpinBox->setVisible(index == 2);
        });
        
        algorithmLayout->addWidget(algorithmLabel);
        algorithmLayout->addWidget(algorithmCombo);
        algorithmLayout->addWidget(rotSpinBox);
        algorithmLayout->addStretch();
        decoderLayout->addLayout(algorithmLayout);
        
        QLabel *inputLabel = new QLabel("Input:");
        inputLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
        decoderLayout->addWidget(inputLabel);
        
        decoderInputEdit = new QTextEdit();
        decoderInputEdit->setPlaceholderText("Paste your encoded string here...");
        decoderInputEdit->setMaximumHeight(150);
        decoderLayout->addWidget(decoderInputEdit);
        
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *decodeButton = new QPushButton("Decode");
        decodeButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #45a049; }");
        
        QPushButton *clearButton = new QPushButton("Clear");
        clearButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #da190b; }");
        
        connect(decodeButton, &QPushButton::clicked, this, &MainWindow::performDecode);
        connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearDecoder);
        
        buttonLayout->addWidget(decodeButton);
        buttonLayout->addWidget(clearButton);
        buttonLayout->addStretch();
        decoderLayout->addLayout(buttonLayout);
        
        QLabel *outputLabel = new QLabel("Output:");
        outputLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
        decoderLayout->addWidget(outputLabel);
        
        decoderOutputEdit = new QTextEdit();
        decoderOutputEdit->setReadOnly(true);
        decoderOutputEdit->setPlaceholderText("Decoded text will appear here...");
        decoderLayout->addWidget(decoderOutputEdit);
        
        QPushButton *copyButton = new QPushButton("Copy Output");
        copyButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #1976D2; }");
        connect(copyButton, &QPushButton::clicked, this, &MainWindow::copyDecoderOutput);
        decoderLayout->addWidget(copyButton);
        
        stackedWidget->addWidget(decoderWidget);
    }
    
    void setupUnpackerScreen()
    {
        QWidget *unpackerWidget = new QWidget();
        QVBoxLayout *unpackerLayout = new QVBoxLayout(unpackerWidget);
        
        QPushButton *backButton = new QPushButton("← Back to Home");
        backButton->setStyleSheet("QPushButton { background-color: #666; color: white; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #555; }");
        connect(backButton, &QPushButton::clicked, this, &MainWindow::goHome);
        unpackerLayout->addWidget(backButton);
        
        QLabel *titleLabel = new QLabel("JavaScript Deobfuscator");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin: 10px;");
        unpackerLayout->addWidget(titleLabel);
        
        QLabel *inputLabel = new QLabel("Obfuscated JavaScript:");
        inputLabel->setStyleSheet("font-weight: bold;");
        unpackerLayout->addWidget(inputLabel);
        
        unpackerInputEdit = new QTextEdit();
        unpackerInputEdit->setPlaceholderText("Paste obfuscated JavaScript code here...");
        unpackerInputEdit->setMaximumHeight(200);
        unpackerLayout->addWidget(unpackerInputEdit);
        
        QHBoxLayout *unpackButtonLayout = new QHBoxLayout();
        QPushButton *unpackButton = new QPushButton("Deobfuscate");
        unpackButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-weight: bold; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #F57C00; }");
        
        QPushButton *clearUnpackButton = new QPushButton("Clear");
        clearUnpackButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #da190b; }");
        
        connect(unpackButton, &QPushButton::clicked, this, &MainWindow::performUnpack);
        connect(clearUnpackButton, &QPushButton::clicked, this, &MainWindow::clearUnpacker);
        
        unpackButtonLayout->addWidget(unpackButton);
        unpackButtonLayout->addWidget(clearUnpackButton);
        unpackButtonLayout->addStretch();
        unpackerLayout->addLayout(unpackButtonLayout);
        
        QLabel *outputLabel = new QLabel("Deobfuscated Output:");
        outputLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
        unpackerLayout->addWidget(outputLabel);
        
        unpackerOutputEdit = new QTextEdit();
        unpackerOutputEdit->setReadOnly(true);
        unpackerOutputEdit->setPlaceholderText("Deobfuscated code will appear here...");
        unpackerLayout->addWidget(unpackerOutputEdit);
        
        QPushButton *copyUnpackButton = new QPushButton("Copy Output");
        copyUnpackButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #1976D2; }");
        connect(copyUnpackButton, &QPushButton::clicked, this, &MainWindow::copyUnpackerOutput);
        unpackerLayout->addWidget(copyUnpackButton);
        
        stackedWidget->addWidget(unpackerWidget);
    }
    
    void setupCurlBuilderScreen()
    {
        QWidget *curlWidget = new QWidget();
        QVBoxLayout *curlLayout = new QVBoxLayout(curlWidget);
        
        QPushButton *backButton = new QPushButton("← Back to Home");
        backButton->setStyleSheet("QPushButton { background-color: #666; color: white; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #555; }");
        connect(backButton, &QPushButton::clicked, this, &MainWindow::goHome);
        curlLayout->addWidget(backButton);
        
        QLabel *titleLabel = new QLabel("Curl Request Builder");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin: 10px;");
        curlLayout->addWidget(titleLabel);
        
        // URL Section
        QGroupBox *urlGroup = new QGroupBox("Request URL");
        QVBoxLayout *urlLayout = new QVBoxLayout(urlGroup);
        
        urlLineEdit = new QLineEdit();
        urlLineEdit->setPlaceholderText("https://api.example.com/endpoint");
        connect(urlLineEdit, &QLineEdit::textChanged, this, &MainWindow::updateCurlCommand);
        urlLayout->addWidget(urlLineEdit);
        
        curlLayout->addWidget(urlGroup);
        
        // Method and Options Section
        QGroupBox *methodGroup = new QGroupBox("HTTP Method & Options");
        QGridLayout *methodLayout = new QGridLayout(methodGroup);
        
        QLabel *methodLabel = new QLabel("Method:");
        methodCombo = new QComboBox();
        methodCombo->addItems({"GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS"});
        connect(methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateCurlCommand);
        
        methodLayout->addWidget(methodLabel, 0, 0);
        methodLayout->addWidget(methodCombo, 0, 1);
        
        // Flags
        QLabel *verboseLabel = new QLabel("Verbose:");
        verboseCombo = new QComboBox();
        verboseCombo->addItems({"None", "-v", "-vv", "-vvv"});
        connect(verboseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateCurlCommand);
        
        followRedirectsCheck = new QCheckBox("Follow redirects (-L)");
        connect(followRedirectsCheck, &QCheckBox::toggled, this, &MainWindow::updateCurlCommand);
        insecureCheck = new QCheckBox("Insecure (-k)");
        connect(insecureCheck, &QCheckBox::toggled, this, &MainWindow::updateCurlCommand);
        includeHeadersCheck = new QCheckBox("Include response headers (-i)");
        connect(includeHeadersCheck, &QCheckBox::toggled, this, &MainWindow::updateCurlCommand);
        
        methodLayout->addWidget(verboseLabel, 1, 0);
        methodLayout->addWidget(verboseCombo, 1, 1);
        methodLayout->addWidget(followRedirectsCheck, 2, 0);
        methodLayout->addWidget(insecureCheck, 2, 1);
        methodLayout->addWidget(includeHeadersCheck, 3, 0);
        
        curlLayout->addWidget(methodGroup);
        
        // Headers Section
        QGroupBox *headersGroup = new QGroupBox("Headers");
        QVBoxLayout *headersLayout = new QVBoxLayout(headersGroup);
        
        QHBoxLayout *headerButtonLayout = new QHBoxLayout();
        addHeaderButton = new QPushButton("Add Header");
        addHeaderButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 6px 12px; border: none; border-radius: 4px; }");
        connect(addHeaderButton, &QPushButton::clicked, this, &MainWindow::addHeader);
        headerButtonLayout->addWidget(addHeaderButton);
        headerButtonLayout->addStretch();
        headersLayout->addLayout(headerButtonLayout);
        
        QScrollArea *scrollArea = new QScrollArea();
        headersWidget = new QWidget();
        headersWidgetLayout = new QVBoxLayout(headersWidget);
        headersWidgetLayout->addStretch();
        scrollArea->setWidget(headersWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setMaximumHeight(200);
        scrollArea->setMinimumHeight(100);
        headersLayout->addWidget(scrollArea);
        
        curlLayout->addWidget(headersGroup);
        
        // Body Section
        QGroupBox *bodyGroup = new QGroupBox("Request Body");
        QVBoxLayout *bodyLayout = new QVBoxLayout(bodyGroup);
        
        QHBoxLayout *bodyTopLayout = new QHBoxLayout();
        QPushButton *formatJsonButton = new QPushButton("Format JSON");
        formatJsonButton->setStyleSheet("QPushButton { background-color: #673AB7; color: white; padding: 4px 8px; border: none; border-radius: 4px; }");
        connect(formatJsonButton, &QPushButton::clicked, this, &MainWindow::formatJsonBody);
        bodyTopLayout->addStretch();
        bodyTopLayout->addWidget(formatJsonButton);
        bodyLayout->addLayout(bodyTopLayout);
        
        bodyTextEdit = new QTextEdit();
        bodyTextEdit->setPlaceholderText("Request body (JSON, form data, etc.)");
        bodyTextEdit->setMaximumHeight(120);
        bodyTextEdit->setStyleSheet("QTextEdit { font-family: 'Courier New', monospace; }");
        connect(bodyTextEdit, &QTextEdit::textChanged, this, &MainWindow::updateCurlCommand);
        bodyLayout->addWidget(bodyTextEdit);
        
        curlLayout->addWidget(bodyGroup);
        
        // Generated Command Section
        QLabel *commandLabel = new QLabel("Generated Curl Command:");
        commandLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
        curlLayout->addWidget(commandLabel);
        
        curlCommandEdit = new QTextEdit();
        curlCommandEdit->setReadOnly(true);
        curlCommandEdit->setPlaceholderText("Your curl command will appear here...");
        curlCommandEdit->setMaximumHeight(120);
        curlLayout->addWidget(curlCommandEdit);
        
        QPushButton *copyCurlButton = new QPushButton("Copy Curl Command");
        copyCurlButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px 16px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #1976D2; }");
        connect(copyCurlButton, &QPushButton::clicked, this, &MainWindow::copyCurlCommand);
        curlLayout->addWidget(copyCurlButton);
        
        stackedWidget->addWidget(curlWidget);
        
        // Initial command generation
        updateCurlCommand();
        updateAddHeaderButton();
    }

    bool hasIncompleteHeader()
    {
        if (!headersWidget) return false;
        
        for (int i = 0; i < headersWidgetLayout->count() - 1; i++) {
            QWidget *widget = headersWidgetLayout->itemAt(i)->widget();
            if (widget) {
                QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(widget->layout());
                if (layout && layout->count() >= 3) {
                    QComboBox *keyCombo = qobject_cast<QComboBox*>(layout->itemAt(0)->widget());
                    QComboBox *valueCombo = qobject_cast<QComboBox*>(layout->itemAt(1)->widget());
                    if (keyCombo && valueCombo) {
                        if (keyCombo->currentText().isEmpty() || valueCombo->currentText().isEmpty()) {
                            return true; // Found incomplete header
                        }
                    }
                }
            }
        }
        return false;
    }

    void updateAddHeaderButton()
    {
        if (!addHeaderButton) return;
        
        bool hasIncomplete = hasIncompleteHeader();
        addHeaderButton->setEnabled(!hasIncomplete);
        
        if (hasIncomplete) {
            addHeaderButton->setStyleSheet("QPushButton { background-color: #cccccc; color: #666666; padding: 6px 12px; border: none; border-radius: 4px; }");
        } else {
            addHeaderButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 6px 12px; border: none; border-radius: 4px; }");
        }
    }

    void updateHeaderValueDropdown(QWidget *headerRow, const QString &headerName)
    {
        QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(headerRow->layout());
        if (!layout || layout->count() < 2) return;
        
        QComboBox *valueCombo = qobject_cast<QComboBox*>(layout->itemAt(1)->widget());
        if (!valueCombo) return;
        
        valueCombo->clear();
        QStringList values = CurlBuilder::getCommonHeaderValues(headerName);
        if (!values.isEmpty()) {
            valueCombo->addItems(values);
        }
        valueCombo->setCurrentText("");
    }

private:
    QStackedWidget *stackedWidget;
    QComboBox *algorithmCombo;
    QSpinBox *rotSpinBox;
    QTextEdit *decoderInputEdit;
    QTextEdit *decoderOutputEdit;
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"