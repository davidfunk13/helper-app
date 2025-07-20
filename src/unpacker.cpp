#include "unpacker.h"
#include <QRegularExpression>
#include <QStringList>
#include <QChar>
#include <QtCore/qmath.h>

QString Unpacker::deobfuscateJavaScript(const QString &input)
{
    QString result = input;
    
    // Handle Dean Edwards packer format first
    result = unpackDeanEdwards(result);
    
    // Handle hex escapes \\xXX
    QRegularExpression hexRegex("\\\\x([0-9A-Fa-f]{2})");
    QRegularExpressionMatchIterator hexIt = hexRegex.globalMatch(result);
    QList<QPair<QString, QString>> hexReplacements;
    while (hexIt.hasNext()) {
        QRegularExpressionMatch match = hexIt.next();
        bool ok;
        int value = match.captured(1).toInt(&ok, 16);
        if (ok && value > 0) {
            hexReplacements.append({match.captured(0), QString(QChar(value))});
        }
    }
    for (const auto &replacement : hexReplacements) {
        result.replace(replacement.first, replacement.second);
    }
    
    // Handle unicode escapes \\uXXXX
    QRegularExpression unicodeRegex("\\\\u([0-9A-Fa-f]{4})");
    QRegularExpressionMatchIterator unicodeIt = unicodeRegex.globalMatch(result);
    QList<QPair<QString, QString>> unicodeReplacements;
    while (unicodeIt.hasNext()) {
        QRegularExpressionMatch match = unicodeIt.next();
        bool ok;
        int value = match.captured(1).toInt(&ok, 16);
        if (ok && value > 0) {
            unicodeReplacements.append({match.captured(0), QString(QChar(value))});
        }
    }
    for (const auto &replacement : unicodeReplacements) {
        result.replace(replacement.first, replacement.second);
    }
    
    // Handle String.fromCharCode calls
    QRegularExpression charCodeRegex("String\\.fromCharCode\\(([0-9,\\s]+)\\)");
    QRegularExpressionMatchIterator charIt = charCodeRegex.globalMatch(result);
    QList<QPair<QString, QString>> charReplacements;
    while (charIt.hasNext()) {
        QRegularExpressionMatch match = charIt.next();
        QString chars = match.captured(1);
        QStringList charCodes = chars.split(',', Qt::SkipEmptyParts);
        QString decoded;
        for (const QString &code : charCodes) {
            bool ok;
            int value = code.trimmed().toInt(&ok);
            if (ok && value > 0 && value <= 0x10FFFF) {
                decoded += QChar(value);
            }
        }
        charReplacements.append({match.captured(0), '"' + decoded + '"'});
    }
    for (const auto &replacement : charReplacements) {
        result.replace(replacement.first, replacement.second);
    }
    
    // Unescape common patterns
    result.replace("\\\\n", "\n");
    result.replace("\\\\t", "\t");
    result.replace("\\\\r", "\r");
    result.replace("\\\\\\\\", "\\");
    result.replace("\\\\'", "'");
    result.replace("\\\\\"", "\"");
    
    return result;
}

QString Unpacker::unpackDeanEdwards(const QString &input)
{
    // More flexible Dean Edwards packer pattern
    QRegularExpression packerRegex("eval\\(function\\(p,a,c,k,e,r\\)\\{.*?\\}\\('([^']*)',(\\d+),(\\d+),'([^']*)'\\.split\\('\\|'\\),0,\\{\\}\\)\\)");
    QRegularExpressionMatch match = packerRegex.match(input);
    
    if (!match.hasMatch()) {
        return input; // Not packed with Dean Edwards packer
    }
    
    QString packedCode = match.captured(1);
    int base = match.captured(2).toInt();
    int count = match.captured(3).toInt();
    QString keywordsStr = match.captured(4);
    
    QStringList keywords = keywordsStr.split('|');
    
    // Pad keywords array to match count
    while (keywords.size() < count) {
        keywords.append("");
    }
    
    QString result = packedCode;
    
    // Replace encoded tokens with keywords - process in reverse order to avoid conflicts
    for (int i = count - 1; i >= 0; i--) {
        QString keyword = (i < keywords.size()) ? keywords[i] : "";
        if (!keyword.isEmpty()) {
            QString token = toBase(i, base);
            
            // Use word boundaries for replacement - fix the regex pattern
            QRegularExpression tokenRegex("\\b" + QRegularExpression::escape(token) + "\\b");
            result.replace(tokenRegex, keyword);
        }
    }
    
    return result;
}

QString Unpacker::toBase(int num, int base)
{
    if (num == 0) return "0";
    
    QString chars = "0123456789abcdefghijklmnopqrstuvwxyz";
    QString result;
    
    while (num > 0) {
        result.prepend(chars[num % base]);
        num /= base;
    }
    
    return result;
}

QString Unpacker::beautifyJavaScript(const QString &input)
{
    QString result = input;
    int indentLevel = 0;
    QString indentStr = "    "; // 4 spaces
    QString formatted;
    bool inString = false;
    QChar stringChar;
    bool escaped = false;
    
    for (int i = 0; i < result.length(); i++) {
        QChar ch = result[i];
        
        // Handle string literals
        if (!escaped && (ch == '"' || ch == '\'')) {
            if (!inString) {
                inString = true;
                stringChar = ch;
            } else if (ch == stringChar) {
                inString = false;
            }
            formatted += ch;
            continue;
        }
        
        if (inString) {
            formatted += ch;
            escaped = (ch == '\\' && !escaped);
            continue;
        }
        
        escaped = false;
        
        switch (ch.toLatin1()) {
            case '{':
                formatted += ch;
                formatted += '\n';
                indentLevel++;
                for (int j = 0; j < indentLevel; j++) {
                    formatted += indentStr;
                }
                break;
                
            case '}':
                // Remove trailing whitespace
                while (formatted.endsWith(' ') || formatted.endsWith('\t')) {
                    formatted.chop(1);
                }
                if (!formatted.endsWith('\n')) {
                    formatted += '\n';
                }
                indentLevel = qMax(0, indentLevel - 1);
                for (int j = 0; j < indentLevel; j++) {
                    formatted += indentStr;
                }
                formatted += ch;
                if (i + 1 < result.length() && result[i + 1] != ')' && result[i + 1] != ';' && result[i + 1] != '}') {
                    formatted += '\n';
                    for (int j = 0; j < indentLevel; j++) {
                        formatted += indentStr;
                    }
                }
                break;
                
            case ';':
                formatted += ch;
                if (i + 1 < result.length() && result[i + 1] != '}' && result[i + 1] != ')') {
                    formatted += '\n';
                    for (int j = 0; j < indentLevel; j++) {
                        formatted += indentStr;
                    }
                }
                break;
                
            case '(':
                formatted += ch;
                // Check if this is a function declaration
                if (i > 7 && result.mid(i - 8, 8) == "function") {
                    // Keep parameters on same line for function declarations
                }
                break;
                
            case ')':
                formatted += ch;
                // Look ahead for opening brace
                if (i + 1 < result.length() && result[i + 1] == '{') {
                    formatted += '\n';
                    for (int j = 0; j < indentLevel; j++) {
                        formatted += indentStr;
                    }
                }
                break;
                
            case ':':
                formatted += ch;
                formatted += " ";
                break;
                
            case ' ':
                // Don't add multiple spaces
                if (!formatted.endsWith(' ') && !formatted.endsWith('\n') && !formatted.endsWith('\t')) {
                    formatted += ch;
                }
                break;
                
            default:
                formatted += ch;
                break;
        }
    }
    
    // Clean up extra newlines
    while (formatted.contains("\n\n\n")) {
        formatted.replace("\n\n\n", "\n\n");
    }
    
    return formatted.trimmed();
}

QString Unpacker::formatJson(const QString &input)
{
    QString text = input.trimmed();
    if (text.isEmpty()) return text;
    
    // Fix common JSON issues
    QString fixed = text;
    
    // Add quotes to unquoted keys and string values
    fixed.replace(QRegularExpression("([{,]\\s*)([a-zA-Z_][a-zA-Z0-9_]*)\\s*:"), "\\1\"\\2\":");
    fixed.replace(QRegularExpression(":\\s*([a-zA-Z_][a-zA-Z0-9_\\s]*)\\s*([,}])"), ": \"\\1\"\\2");
    
    // Fix single quotes to double quotes
    fixed.replace("'", "\"");
    
    // Clean up any double quotes around numbers or booleans
    fixed.replace(QRegularExpression("\"(\\d+(?:\\.\\d+)?)\""), "\\1");
    fixed.replace(QRegularExpression("\"(true|false|null)\""), "\\1");
    
    // Format with proper indentation
    QString formatted = "";
    int indentLevel = 0;
    bool inString = false;
    bool escaped = false;
    
    for (int i = 0; i < fixed.length(); i++) {
        QChar ch = fixed[i];
        
        if (!escaped && ch == '\"') {
            inString = !inString;
        }
        
        if (!inString) {
            switch (ch.toLatin1()) {
                case '{':
                case '[':
                    formatted += ch;
                    formatted += "\n";
                    indentLevel++;
                    for (int j = 0; j < indentLevel; j++) {
                        formatted += "  ";
                    }
                    break;
                    
                case '}':
                case ']':
                    // Remove trailing spaces and add newline
                    formatted = formatted.trimmed();
                    formatted += "\n";
                    indentLevel = qMax(0, indentLevel - 1);
                    for (int j = 0; j < indentLevel; j++) {
                        formatted += "  ";
                    }
                    formatted += ch;
                    break;
                    
                case ',':
                    formatted += ch;
                    formatted += "\n";
                    for (int j = 0; j < indentLevel; j++) {
                        formatted += "  ";
                    }
                    break;
                    
                case ':':
                    formatted += ch;
                    formatted += " ";
                    break;
                    
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    // Skip whitespace outside strings
                    break;
                    
                default:
                    formatted += ch;
                    break;
            }
        } else {
            formatted += ch;
        }
        
        escaped = (ch == '\\' && !escaped);
    }
    
    return formatted.trimmed();
}