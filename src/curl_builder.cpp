#include "curl_builder.h"
#include <QStringList>

QString CurlBuilder::buildCurlCommand(const CurlOptions &options)
{
    QString command = "curl";
    
    // Add URL first (right after curl)
    if (!options.url.isEmpty()) {
        command += " \"" + escapeShellArg(options.url) + "\"";
    }
    
    // Add method
    if (options.method != GET) {
        command += " -X " + httpMethodToString(options.method);
    }
    
    // Add flags
    if (options.verbose != None) {
        command += " " + verboseLevelToString(options.verbose);
    }
    if (options.followRedirects) {
        command += " -L";
    }
    if (options.insecure) {
        command += " -k";
    }
    if (options.includeResponseHeaders) {
        command += " -i";
    }
    
    // Add headers
    for (const auto &header : options.headers) {
        if (!header.first.isEmpty() && !header.second.isEmpty()) {
            command += " -H \"" + escapeShellArg(header.first + ": " + header.second) + "\"";
        }
    }
    
    // Add body
    if (!options.body.isEmpty()) {
        command += " -d \"" + escapeShellArg(options.body) + "\"";
    }
    
    return command;
}

QString CurlBuilder::httpMethodToString(HttpMethod method)
{
    switch (method) {
        case GET: return "GET";
        case POST: return "POST";
        case PUT: return "PUT";
        case DELETE: return "DELETE";
        case PATCH: return "PATCH";
        case HEAD: return "HEAD";
        case OPTIONS: return "OPTIONS";
        default: return "GET";
    }
}

QString CurlBuilder::verboseLevelToString(VerboseLevel level)
{
    switch (level) {
        case None: return "";
        case V: return "-v";
        case VV: return "-vv";
        case VVV: return "-vvv";
        default: return "";
    }
}

QStringList CurlBuilder::getCommonHeaderValues(const QString &headerName)
{
    if (headerName == "Content-Type") {
        return {
            "application/json",
            "application/xml",
            "application/x-www-form-urlencoded",
            "text/plain",
            "text/html",
            "multipart/form-data"
        };
    } else if (headerName == "Authorization") {
        return {
            "Bearer your_token_here",
            "Basic base64_encoded_credentials",
            "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9..."
        };
    } else if (headerName == "Accept") {
        return {
            "application/json",
            "application/xml",
            "text/html",
            "*/*"
        };
    } else if (headerName == "User-Agent") {
        return {
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)",
            "curl/7.68.0",
            "PostmanRuntime/7.28.0"
        };
    } else if (headerName == "Cache-Control") {
        return {
            "no-cache",
            "no-store",
            "max-age=0",
            "must-revalidate"
        };
    }
    
    return {};
}

QString CurlBuilder::escapeShellArg(const QString &arg)
{
    QString escaped = arg;
    escaped.replace("\"", "\\\"");
    return escaped;
}