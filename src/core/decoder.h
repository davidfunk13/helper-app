#pragma once

#include <QString>

class Decoder {
  public:
    enum Algorithm { Base64, Hex, ROT };

    static QString decode(const QString &input, Algorithm algorithm, int rotShift = 13);
    static QString decodeBase64(const QString &input);
    static QString decodeHex(const QString &input);
    static QString decodeROT(const QString &input, int shift);

  private:
    static QString toBase(int num, int base);
};