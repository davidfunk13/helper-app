#include <QtTest/QtTest>

#include "../core/curl_builder.h"

class TestCurlBuilder : public QObject {
    Q_OBJECT

  private slots:
    void testBasicCommand();
    void testHttpMethods_data();
    void testHttpMethods();
    void testHeaders();
    void testVerboseLevels_data();
    void testVerboseLevels();
    void testFlags();
    void testBodyData();
    void testComplexCommand();
    void testEscaping();
    void testCommonHeaderValues();
};

void TestCurlBuilder::testBasicCommand() {
    CurlBuilder::CurlOptions options;
    options.url = "https://example.com";

    QString result = CurlBuilder::buildCurlCommand(options);

    QVERIFY(result.startsWith("curl"));
    QVERIFY(result.contains("https://example.com"));
}

void TestCurlBuilder::testHttpMethods_data() {
    QTest::addColumn<CurlBuilder::HttpMethod>("method");
    QTest::addColumn<QString>("expectedFlag");

    QTest::newRow("GET") << CurlBuilder::GET << "";
    QTest::newRow("POST") << CurlBuilder::POST << "-X POST";
    QTest::newRow("PUT") << CurlBuilder::PUT << "-X PUT";
    QTest::newRow("DELETE") << CurlBuilder::DELETE << "-X DELETE";
    QTest::newRow("PATCH") << CurlBuilder::PATCH << "-X PATCH";
    QTest::newRow("HEAD") << CurlBuilder::HEAD << "-X HEAD";
    QTest::newRow("OPTIONS") << CurlBuilder::OPTIONS << "-X OPTIONS";
}

void TestCurlBuilder::testHttpMethods() {
    QFETCH(CurlBuilder::HttpMethod, method);
    QFETCH(QString, expectedFlag);

    CurlBuilder::CurlOptions options;
    options.url = "https://example.com";
    options.method = method;

    QString result = CurlBuilder::buildCurlCommand(options);

    if (expectedFlag.isEmpty()) {
        QVERIFY(!result.contains("-X"));  // GET shouldn't have -X flag
    } else {
        QVERIFY(result.contains(expectedFlag));
    }
}

void TestCurlBuilder::testHeaders() {
    CurlBuilder::CurlOptions options;
    options.url = "https://example.com";
    options.headers.append(QPair<QString, QString>("Content-Type", "application/json"));
    options.headers.append(QPair<QString, QString>("Authorization", "Bearer token123"));

    QString result = CurlBuilder::buildCurlCommand(options);

    QVERIFY(result.contains("-H \"Content-Type: application/json\""));
    QVERIFY(result.contains("-H \"Authorization: Bearer token123\""));
}

void TestCurlBuilder::testVerboseLevels_data() {
    QTest::addColumn<CurlBuilder::VerboseLevel>("level");
    QTest::addColumn<QString>("expectedFlag");

    QTest::newRow("None") << CurlBuilder::None << "";
    QTest::newRow("V") << CurlBuilder::V << "-v";
    QTest::newRow("VV") << CurlBuilder::VV << "-vv";
    QTest::newRow("VVV") << CurlBuilder::VVV << "-vvv";
}

void TestCurlBuilder::testVerboseLevels() {
    QFETCH(CurlBuilder::VerboseLevel, level);
    QFETCH(QString, expectedFlag);

    CurlBuilder::CurlOptions options;
    options.url = "https://example.com";
    options.verbose = level;

    QString result = CurlBuilder::buildCurlCommand(options);

    if (expectedFlag.isEmpty()) {
        QVERIFY(!result.contains("-v"));
    } else {
        QVERIFY(result.contains(expectedFlag));
    }
}

void TestCurlBuilder::testFlags() {
    CurlBuilder::CurlOptions options;
    options.url = "https://example.com";
    options.followRedirects = true;
    options.insecure = true;
    options.includeResponseHeaders = true;

    QString result = CurlBuilder::buildCurlCommand(options);

    QVERIFY(result.contains("-L"));
    QVERIFY(result.contains("-k"));
    QVERIFY(result.contains("-i"));
}

void TestCurlBuilder::testBodyData() {
    CurlBuilder::CurlOptions options;
    options.url = "https://example.com";
    options.body = "{\"test\": \"data\"}";

    QString result = CurlBuilder::buildCurlCommand(options);

    QVERIFY(result.contains("-d"));
    QVERIFY(result.contains("{\\\"test\\\": \\\"data\\\"}"));
}

void TestCurlBuilder::testComplexCommand() {
    CurlBuilder::CurlOptions options;
    options.url = "https://api.example.com/endpoint";
    options.method = CurlBuilder::POST;
    options.verbose = CurlBuilder::V;
    options.followRedirects = true;
    options.insecure = false;
    options.includeResponseHeaders = true;
    options.headers.append(QPair<QString, QString>("Content-Type", "application/json"));
    options.headers.append(QPair<QString, QString>("Authorization", "Bearer abc123"));
    options.body = "{\"name\": \"test\", \"value\": 42}";

    QString result = CurlBuilder::buildCurlCommand(options);

    // Check that URL comes first after curl
    QVERIFY(result.startsWith("curl \"https://api.example.com/endpoint\""));

    // Check all components are present
    QVERIFY(result.contains("-X POST"));
    QVERIFY(result.contains("-v"));
    QVERIFY(result.contains("-L"));
    QVERIFY(result.contains("-i"));
    QVERIFY(!result.contains("-k"));  // insecure is false
    QVERIFY(result.contains("-H \"Content-Type: application/json\""));
    QVERIFY(result.contains("-H \"Authorization: Bearer abc123\""));
    QVERIFY(result.contains("-d \"{\\\"name\\\": \\\"test\\\", \\\"value\\\": 42}\""));
}

void TestCurlBuilder::testEscaping() {
    CurlBuilder::CurlOptions options;
    options.url = "https://example.com/path with spaces";
    options.headers.append(QPair<QString, QString>("Custom-Header", "value with \"quotes\""));
    options.body = "data with \"quotes\" and 'apostrophes'";

    QString result = CurlBuilder::buildCurlCommand(options);

    // URL should be quoted
    QVERIFY(result.contains("\"https://example.com/path with spaces\""));

    // Headers should have escaped quotes
    QVERIFY(result.contains("value with \\\"quotes\\\""));

    // Body should have escaped quotes
    QVERIFY(result.contains("data with \\\"quotes\\\" and 'apostrophes'"));
}

void TestCurlBuilder::testCommonHeaderValues() {
    // Test Content-Type suggestions
    QStringList contentTypes = CurlBuilder::getCommonHeaderValues("Content-Type");
    QVERIFY(contentTypes.contains("application/json"));
    QVERIFY(contentTypes.contains("application/xml"));
    QVERIFY(contentTypes.contains("text/plain"));

    // Test Authorization suggestions
    QStringList authTypes = CurlBuilder::getCommonHeaderValues("Authorization");
    QVERIFY(authTypes.contains("Bearer your_token_here"));
    QVERIFY(authTypes.contains("Basic base64_encoded_credentials"));

    // Test Accept suggestions
    QStringList acceptTypes = CurlBuilder::getCommonHeaderValues("Accept");
    QVERIFY(acceptTypes.contains("application/json"));
    QVERIFY(acceptTypes.contains("*/*"));

    // Test unknown header
    QStringList unknown = CurlBuilder::getCommonHeaderValues("Unknown-Header");
    QVERIFY(unknown.isEmpty());
}

QTEST_MAIN(TestCurlBuilder)
#include "test_curl_builder.moc"