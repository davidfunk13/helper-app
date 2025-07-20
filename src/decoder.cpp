#include "decoder.h"
#include <QByteArray>
#include <QRegularExpression>

QString Decoder::decode(const QString &input, Algorithm algorithm, int rotShift)
{
    switch (algorithm) {
        case Base64:
            return decodeBase64(input);
        case Hex:
            return decodeHex(input);
        case ROT:
            return decodeROT(input, rotShift);
        default:
            return "Error: Unknown algorithm";
    }
}

QString Decoder::decodeBase64(const QString &input)
{
    QByteArray inputBytes = input.toUtf8();
    QByteArray decoded = QByteArray::fromBase64(inputBytes);
    
    if (decoded.isEmpty() && !input.isEmpty()) {
        return "Error: Invalid base64 input";
    }
    
    return QString::fromUtf8(decoded);
}

QString Decoder::decodeHex(const QString &input)
{
    QString cleanInput = input;
    cleanInput.remove(QRegularExpression("[^0-9A-Fa-f]"));
    
    if (cleanInput.length() % 2 != 0) {
        return "Error: Invalid hex input (odd length)";
    }
    
    QByteArray decoded = QByteArray::fromHex(cleanInput.toUtf8());
    return QString::fromUtf8(decoded);
}

QString Decoder::decodeROT(const QString &input, int shift)
{
    QString result;
    for (const QChar &ch : input) {
        if (ch.isLetter()) {
            QChar base = ch.isUpper() ? 'A' : 'a';
            int shifted = (ch.toLatin1() - base.toLatin1() - shift + 26) % 26;
            result += QChar(base.toLatin1() + shifted);
        } else {
            result += ch;
        }
    }
    return result;
}

QString Decoder::toBase(int num, int base)
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