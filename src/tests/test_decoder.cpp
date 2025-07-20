#include <QtTest/QtTest>

#include "../core/decoder.h"

class TestDecoder : public QObject {
    Q_OBJECT

  private slots:
    void testBase64Decode_data();
    void testBase64Decode();
    void testHexDecode_data();
    void testHexDecode();
    void testROTDecode_data();
    void testROTDecode();
    void testInvalidInput();
};

void TestDecoder::testBase64Decode_data() {
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");

    QTest::newRow("hello") << "aGVsbG8=" << "hello";
    QTest::newRow("world") << "d29ybGQ=" << "world";
    QTest::newRow("empty") << "" << "";
    QTest::newRow("padding") << "SGVsbG8gV29ybGQ=" << "Hello World";
    QTest::newRow("multiline") << "VGhpcyBpcyBhIHRlc3Q=" << "This is a test";
}

void TestDecoder::testBase64Decode() {
    QFETCH(QString, input);
    QFETCH(QString, expected);

    QString result = Decoder::decode(input, Decoder::Base64);
    QCOMPARE(result, expected);
}

void TestDecoder::testHexDecode_data() {
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");

    QTest::newRow("hello") << "68656c6c6f" << "hello";
    QTest::newRow("world") << "776f726c64" << "world";
    QTest::newRow("uppercase") << "48656C6C6F" << "Hello";
    QTest::newRow("mixed_case") << "48656c6C6f" << "Hello";
    QTest::newRow("with_spaces") << "48 65 6c 6c 6f" << "Hello";
    QTest::newRow("empty") << "" << "";
}

void TestDecoder::testHexDecode() {
    QFETCH(QString, input);
    QFETCH(QString, expected);

    QString result = Decoder::decode(input, Decoder::Hex);
    QCOMPARE(result, expected);
}

void TestDecoder::testROTDecode_data() {
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("shift");
    QTest::addColumn<QString>("expected");

    QTest::newRow("rot13_hello") << "uryyb" << 13 << "hello";
    QTest::newRow("rot13_world") << "jbeyq" << 13 << "world";
    QTest::newRow("rot1") << "ifmmp" << 1 << "hello";
    QTest::newRow("rot25") << "gdkkn" << 25 << "hello";
    QTest::newRow("uppercase") << "URYYB" << 13 << "HELLO";
    QTest::newRow("mixed_case") << "UrYyB" << 13 << "HeLlO";
    QTest::newRow("with_numbers") << "uryyb123" << 13 << "hello123";
    QTest::newRow("with_spaces") << "uryyb jbeyq" << 13 << "hello world";
    QTest::newRow("empty") << "" << 13 << "";
}

void TestDecoder::testROTDecode() {
    QFETCH(QString, input);
    QFETCH(int, shift);
    QFETCH(QString, expected);

    QString result = Decoder::decode(input, Decoder::ROT, shift);
    QCOMPARE(result, expected);
}

void TestDecoder::testInvalidInput() {
    // Test invalid Base64
    QString result = Decoder::decode("invalid base64!", Decoder::Base64);
    QVERIFY(!result.isEmpty());  // Should return error message or original

    // Test invalid hex (odd number of characters)
    result = Decoder::decode("68656c6c6", Decoder::Hex);
    QVERIFY(!result.isEmpty());  // Should handle gracefully

    // Test ROT with edge cases
    result = Decoder::decode("abc", Decoder::ROT, 0);
    QCOMPARE(result, "abc");  // No shift should return original
}

QTEST_MAIN(TestDecoder)
#include "test_decoder.moc"