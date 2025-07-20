#pragma once

#include <QString>

class Unpacker
{
public:
    static QString deobfuscateJavaScript(const QString &input);
    static QString beautifyJavaScript(const QString &input);
    static QString formatJson(const QString &input);

private:
    static QString unpackDeanEdwards(const QString &input);
    static QString toBase(int num, int base);
};