#  Password Cracker - Educational Security Tool

A comprehensive password cracking toolkit for educational purposes, penetration testing, and security research. This tool demonstrates various password attack techniques both offline and online.

**Status:** All core features completed ✅

---

## Features Overview

### Offline Attacks
- **Brute Force** - Exhaustive search with customizable charset
- **Dictionary** - Wordlist-based attacks
- **Hybrid** - Combines wordlist with number patterns
- **Rule-Based** - Applies transformations (leet speak, capitalization, etc.)
- **Rainbow Tables** - Pre-computed hash lookup tables

### Online Attacks (Requires Authorization)
- **HTTP Login Brute Force** - Web form authentication testing
- **Credential Stuffing** - Breach database validation
- **Password Spraying** - Single password across multiple accounts
- **FTP Brute Force** - File transfer protocol testing

### Hash Support
- MD5, SHA1, SHA256, NTLM, BCRYPT (detection)
- Automatic hash type detection
- Hash comparison across all attack modes

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

## Phase 3 (COMPLETED): Online Cracking Engine
1.  HTTP Login Brute Force (POST request support)
2.  Credential Stuffing with Proxy Support (breach database testing)
3.  FTP Brute Force using raw sockets (SSH support via external tools)
4.  Password Spraying Logic (account lockout prevention)

**Features:**
- Mandatory authorization warnings before execution
- Built-in rate limiting to prevent DoS
- Configurable delays between attempts
- Support for custom form fields
- Connection timeout handling
- Clear success/failure indicators

**Important Security Notes:**
- ⚠️ **LEGAL AUTHORIZATION REQUIRED** - You must have explicit written permission
- All online attacks require user confirmation before execution
- Designed for localhost testing and authorized pentesting only
- Rate limiting prevents service disruption
- Educational tool for security research and CTF competitions

---

## Online Attack Usage Examples

### 7. HTTP Login Brute Force
```bash
# Basic HTTP brute force
./pwcracker --attack http-brute --url http://localhost/login --username admin --wordlist wordlists/test.txt

# With custom delay and form fields
./pwcracker --attack http-brute --url http://localhost/login --username admin --wordlist wordlists/test.txt --delay 2000 --userfield email --passfield pass
```

### 8. Credential Stuffing
```bash
# Test breach credentials (format: username:password per line)
./pwcracker --attack credential-stuffing --url http://localhost/login --credentials breached.txt --delay 1000

# With proxy support
./pwcracker --attack credential-stuffing --url http://localhost/login --credentials breached.txt --proxy http://proxy:8080
```

### 9. Password Spraying
```bash
# Test single password against multiple users
./pwcracker --attack password-spray --url http://localhost/login --userlist users.txt --password Welcome2024! --delay 5000
```

### 10. FTP Brute Force
```bash
# Basic FTP brute force
./pwcracker --attack ftp-brute --host localhost --port 21 --username admin --wordlist wordlists/test.txt

# With custom delay
./pwcracker --attack ftp-brute --host 192.168.1.10 --port 21 --username ftpuser --wordlist wordlists/test.txt --delay 2000
```

**Note:** For SSH brute force, consider using established tools like:
- Hydra: `hydra -l user -P wordlist.txt ssh://target`
- Ncrack: `ncrack -U users.txt -P passwords.txt ssh://target`
- Medusa: `medusa -h target -u user -P wordlist.txt -M ssh`

---
