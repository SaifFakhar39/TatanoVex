
# TatanoVex

```
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

---

## مقدمة

TatanoVex هي أداة عالية الأداء مبرمجة بلغة C++، مصممة لتنفيذ هجمات HTTP flood عالية السرعة وفعالة باستخدام خيوط متعددة، مع دعم كامل لطلبات GET وPOST.

تم تحسين الأداة لتستخدم موارد النظام بأقصى كفاءة، ما يجعلها سريعة جداً لأنها تتحول إلى لغة آلة (Machine Code).

---

## الميزات

- دعم GET و POST مع إضافة رؤوس مخصصة (Headers).
- تنفيذ متزامن باستخدام خيوط متعددة (Threads).
- دعم SSL/TLS للاتصالات HTTPS.
- إمكانية تخصيص مدة الهجوم وعدد الخيوط.
- قراءة رؤوس HTTP من ملف خارجي.
- أداء عالي جدًا وسريع جدًا.

---

## المتطلبات

- نظام تشغيل Linux, macOS, Windows.
- مكتبات OpenSSL مثبتة.
- مترجم C++17 أو أحدث.

---

## طريقة الاستخدام

```bash
./flooder <url> <threads> <get/post> <seconds> <header_file/nil>
```

- `<url>`: رابط الموقع (مع http:// أو https://).
- `<threads>`: عدد الخيوط (Threads).
- `<get/post>`: نوع الطلب (GET أو POST).
- `<seconds>`: مدة الهجوم بالثواني.
- `<header_file/nil>`: اسم ملف يحتوي على رؤوس HTTP أو اكتب `nil` لعدم استخدام رؤوس.

---

## مثال

```bash
./flooder https://example.com 150 POST 90 headers.txt
```

---

## ملف رؤوس HTTP (headers.txt) - مثال

```
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Accept-Language: en-US,en;q=0.5
Connection: keep-alive
```

---

## طرق التحميل

- على Linux:
```bash
git clone https://github.com/username/TatanoVex.git
cd TatanoVex
g++ -std=c++17 flooder.cpp -o flooder -pthread -lssl -lcrypto
```

- على macOS:
```bash
brew install openssl
g++ -std=c++17 flooder.cpp -o flooder -pthread -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lssl -lcrypto
```

- على Windows (باستخدام MinGW أو MSYS2):
```bash
g++ -std=c++17 flooder.cpp -o flooder.exe -pthread -lssl -lcrypto
```

---

## ملاحظة

TatanoVex أداة قوية وسريعة جداً لأنها تستخدم لغة C++ مع تحسينات أداء على مستوى النظام، لكنها يجب أن تستخدم فقط لأغراض اختبار الأمان الخاص بك أو ضمن بيئة قانونية.

---

## الترخيص

MIT License

---

## التواصل

لأي استفسار، يمكنك التواصل عبر Issues في GitHub.
