#  Roadmap to Build the Tool & the Updates

 **Warning**: This tool is under active development.  
The following is a list of planned updates and improvements.  
Feel free to contribute by implementing any feature from the list!  

---

## Disclaimer  

> **This project is created strictly for educational and research purposes** to demonstrate how password cracking techniques work in a controlled, legal environment.  
> **Do not** use this tool for unauthorized access to systems, networks, or accounts.  
> Any misuse of this code may violate applicable laws and could result in severe penalties.  
> The author takes **no responsibility** for any illegal or unethical use of this tool.  
> Always ensure you have **explicit permission** before testing security on any system.  

---

## Phase 1 (COMPLETED): Offline Cracking Basics
1.  Basic Brute Force (Alphabets only, fixed length)
2.  Dictionary Attack (from file)
3.  Hash Comparison (MD5/SHA1/SHA256/NTLM using OpenSSL)

---

## Phase 2 (COMPLETED): Advanced Offline Cracking
1.  Hybrid Attack (Append/prepend numbers to words)
2.  Rainbow Table Lookup (build and search rainbow tables)
3.  Rule-based Attacks (capitalization, leet speak, reversals, common suffixes)
4.  Hash Formats: MD5, SHA1, SHA256, NTLM (bcrypt detection added, requires external library for full support)

**Features:**
- Automatic hash type detection
- Rainbow table generation and lookup
- Multiple attack modes with hash support
- Customizable charset and password length
- Performance timing and attempt tracking

---

## Installation

### Prerequisites
- C++ compiler (g++ or MinGW)
- OpenSSL development libraries

### Windows
```bash
install.bat
```

### Linux/Mac
```bash
chmod +x install.sh
./install.sh
```

---

## Usage Examples

### 1. Basic Brute Force Attack
```bash
# Plain text password
./pwcracker --attack brute --target cat --maxlen 4 --charset abc

# MD5 hash
./pwcracker --attack brute --target d077f244def8a70e5ea758bd8352fcd8 --maxlen 4 --charset abc --hash
```

### 2. Dictionary Attack
```bash
# Plain text
./pwcracker --attack dict --target password123 --wordlist wordlists/test.txt

# With hash
./pwcracker --attack dict --target 5f4dcc3b5aa765d61d8327deb882cf99 --wordlist wordlists/test.txt --hash
```

### 3. Hybrid Attack (Dictionary + Numbers)
```bash
# Appends/prepends numbers to dictionary words
./pwcracker --attack hybrid --target password123 --wordlist wordlists/test.txt --maxdigits 3
```

### 4. Rule-Based Attack (Transformations)
```bash
# Applies capitalization, leet speak, reversals, common suffixes
./pwcracker --attack rules --target Password123! --wordlist wordlists/test.txt
```

### 5. Rainbow Table Generation
```bash
# Generate a rainbow table
./pwcracker --mode generate --output rainbow.db --charset abc --maxlen 6 --chains 10000 --chainlen 1000 --hashtype md5
```

### 6. Rainbow Table Attack
```bash
# Use rainbow table to crack hash
./pwcracker --attack rainbow --target 5f4dcc3b5aa765d61d8327deb882cf99 --table rainbow.db --charset abc --maxlen 6
```

### Supported Hash Types
- MD5 (32 characters)
- SHA1 (40 characters)
- SHA256 (64 characters)
- NTLM (32 characters, Windows hashes)
- BCRYPT (detection only, requires external library for cracking)

---

## Phase 3 (PENDING): Online Cracking Engine
1. Basic HTTP Login Brute Force (test dummy web server locally)
2. Credential Stuffing with Proxy Support
3. FTP/SSH Brute Force using libraries like libssh or raw sockets
4. Password Spraying Logic
5. Phishing Simulation with Key Capture

---
