# HTTP Flooder C++ - Advanced Network Stress Testing Tool

<div align="center">

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OpenSSL](https://img.shields.io/badge/OpenSSL-721412?style=for-the-badge&logo=openssl&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)

**A high-performance HTTP stress testing tool written in C++ for authorized testing and educational purposes.**

</div>

## ⚠️ **IMPORTANT DISCLAIMER**

This tool is designed for **authorized security testing**, **educational purposes**, and **personal server stress testing** only. 

- **DO NOT** use this tool against systems you do not own or have explicit permission to test
- **DO NOT** use this tool for malicious purposes, DDoS attacks, or any illegal activities
- The developers are **NOT responsible** for any misuse of this software
- Users are responsible for complying with all applicable laws and regulations

## 🚀 Features

### High Performance
- **Multi-threaded architecture** with configurable thread count
- **Asynchronous I/O** for maximum throughput
- **Connection reuse** to minimize overhead
- **Optimized network stack** usage for better performance
- **Real-time statistics** monitoring

### Protocol Support
- **HTTP/HTTPS** support with OpenSSL integration
- **GET/POST** request methods
- **Custom headers** support via external files
- **SSL/TLS** encryption handling
- **Advanced user agent rotation**

### Cross-Platform Compatibility
- ✅ **Linux** (Ubuntu, CentOS, Debian, etc.)
- ✅ **Windows** (Windows 7/8/10/11)
- ✅ **macOS** (Intel and Apple Silicon)

### Smart Features
- **Intelligent caching bypass** with random parameters
- **Multiple user agent simulation** (Chrome, Firefox, IE, Search Bots)
- **Referrer header randomization**
- **Accept header variation**
- **Real-time attack statistics**

## 📋 Prerequisites

### Linux/macOS
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential libssl-dev

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install openssl-devel

# macOS (with Homebrew)
brew install openssl
```

### Windows
- **Visual Studio 2019/2022** with C++ support, or
- **MinGW-w64** compiler
- **OpenSSL** development libraries

## 🛠️ Installation

### Method 1: Clone Repository
```bash
git clone https://github.com/yourusername/http-flooder-cpp.git
cd http-flooder-cpp
```

### Method 2: Manual Compilation

#### Linux/macOS
```bash
# Standard compilation
g++ -o http_flooder http_flooder.cpp -std=c++11 -pthread -lssl -lcrypto -O3

# With additional optimizations
g++ -o http_flooder http_flooder.cpp -std=c++11 -pthread -lssl -lcrypto -O3 -march=native -mtune=native
```

#### Windows (MinGW)
```bash
g++ -o http_flooder.exe http_flooder.cpp -std=c++11 -lws2_32 -lssl -lcrypto -O3
```

#### Windows (Visual Studio)
```bash
cl /EHsc /std:c++11 http_flooder.cpp /link ws2_32.lib ssleay32.lib libeay32.lib
```

## 🎯 Usage

### Basic Syntax
```bash
./http_flooder <url> <threads> <method> <duration> <headers>
```

### Parameters
- **url**: Target URL (http://example.com or https://example.com/path)
- **threads**: Number of concurrent threads (recommended: 2x CPU cores)
- **method**: HTTP method (`get` or `post`)
- **duration**: Attack duration in seconds
- **headers**: Custom headers file path or `nil` for default headers

### Examples

#### Basic GET Request
```bash
./http_flooder http://example.com 50 get 30 nil
```

#### HTTPS POST Request with Custom Headers
```bash
./http_flooder https://api.example.com/endpoint 100 post 60 headers.txt
```

#### High-Intensity Stress Test
```bash
./http_flooder http://localhost:8080 200 get 120 nil
```

## 📝 Custom Headers File

Create a text file with custom HTTP headers:

```text
User-Agent: Custom-Agent/1.0
Accept: application/json
Authorization: Bearer your-token-here
X-Custom-Header: custom-value
```

## 🔧 Performance Optimization

### Linux Optimization
```bash
# Increase file descriptor limit
ulimit -n 999999

# Increase network buffer sizes
echo 'net.core.rmem_max = 134217728' >> /etc/sysctl.conf
echo 'net.core.wmem_max = 134217728' >> /etc/sysctl.conf
sysctl -p
```

### Thread Count Guidelines
- **CPU Cores × 2-4**: Good starting point
- **Monitor CPU usage**: Adjust based on system performance
- **Network bandwidth**: Consider your connection limits

### Example Performance Configurations

#### Low-End System (2-4 cores)
```bash
./http_flooder http://target.com 8 get 30 nil
```

#### Mid-Range System (4-8 cores)
```bash
./http_flooder http://target.com 32 get 60 nil
```

#### High-End System (8+ cores)
```bash
./http_flooder http://target.com 100 get 120 nil
```

## 📊 Real-Time Statistics

The tool provides real-time monitoring with the following metrics:

- **Elapsed Time**: Current test duration
- **Total Requests**: Number of requests sent
- **Requests/Second**: Current throughput rate
- **Failed Requests**: Connection or send failures
- **Active Threads**: Currently running threads

Example output:
```
[15s] Requests: 45230 (3015/s) Failed: 12 Active Threads: 50
```

## 🛡️ Defensive Measures Detection

This tool implements various techniques to simulate legitimate traffic:

- **User Agent Rotation**: Simulates different browsers and search engines
- **Header Variation**: Uses multiple Accept and Accept-Language combinations
- **Referrer Randomization**: Includes realistic referrer headers
- **Parameter Randomization**: Adds random parameters to bypass caching
- **Connection Reuse**: Maintains persistent connections like real browsers

## 📈 Benchmarking Results

Typical performance on modern hardware:

| System Spec | Threads | Requests/Second | Notes |
|-------------|---------|-----------------|-------|
| 4-core CPU, 8GB RAM | 50 | 15,000-25,000 | Local testing |
| 8-core CPU, 16GB RAM | 100 | 35,000-50,000 | Optimal config |
| 16-core CPU, 32GB RAM | 200 | 75,000-100,000+ | High-end performance |

*Results may vary based on target server, network conditions, and system specifications.*

## 🔍 Troubleshooting

### Common Issues

#### Connection Failures
```
Connection Down!!! message appearing frequently
```
**Solutions:**
- Target server may be blocking your IP
- Reduce thread count
- Check firewall settings
- Verify target URL is accessible

#### Compilation Errors
```
fatal error: openssl/ssl.h: No such file or directory
```
**Solutions:**
- Install OpenSSL development headers
- Check SSL library paths
- Use package manager to install dependencies

#### Permission Denied
```
./http_flooder: permission denied
```
**Solutions:**
```bash
chmod +x http_flooder
```

### Performance Issues

#### Low Request Rate
- Increase thread count gradually
- Optimize system network settings
- Check for resource limitations
- Monitor CPU and memory usage

#### High System Load
- Reduce thread count
- Add small delays between requests
- Monitor system resources

## 🧪 Testing Scenarios

### Web Server Load Testing
```bash
# Test web server capacity
./http_flooder http://your-server.com 25 get 60 nil
```

### API Endpoint Testing
```bash
# Test API performance
./http_flooder https://api.yourservice.com/v1/endpoint 50 post 30 api_headers.txt
```

### CDN Performance Testing
```bash
# Test CDN edge servers
./http_flooder https://cdn.yoursite.com/static/file.js 75 get 45 nil
```

## 🤝 Contributing

We welcome contributions! Please follow these guidelines:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### Code Style
- Follow C++11 standards
- Use meaningful variable names
- Add comments for complex logic
- Maintain cross-platform compatibility

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **OpenSSL Team** for providing excellent cryptographic libraries
- **C++ Community** for continuous language improvements
- **Security Researchers** for responsible disclosure practices

## 📧 Contact

- **Issues**: Use GitHub Issues for bug reports and feature requests
- **Security**: Report security vulnerabilities privately
- **General**: Discussions and questions welcome

---

<div align="center">

**⚠️ Remember: Use this tool responsibly and only on systems you own or have explicit permission to test ⚠️**

**🔒 Ethical hacking promotes better security for everyone 🔒**

</div>
