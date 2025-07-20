#pragma once

#include <QString>
#include <QStringList>
#include <QPair>

class CurlBuilder
{
public:
    enum HttpMethod {
        GET,
        POST,
        PUT,
        DELETE,
        PATCH,
        HEAD,
        OPTIONS
    };

    enum VerboseLevel {
        None,
        V,
        VV,
        VVV
    };

    struct CurlOptions {
        QString url;
        HttpMethod method = GET;
        VerboseLevel verbose = None;
        bool followRedirects = false;
        bool insecure = false;
        bool includeResponseHeaders = false;
        QList<QPair<QString, QString>> headers;
        QString body;
    };

    static QString buildCurlCommand(const CurlOptions &options);
    static QString httpMethodToString(HttpMethod method);
    static QString verboseLevelToString(VerboseLevel level);
    static QStringList getCommonHeaderValues(const QString &headerName);

private:
    static QString escapeShellArg(const QString &arg);
};