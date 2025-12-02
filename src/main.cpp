#include <iostream>
#include <string>
#include <unordered_map>
#include "attacks.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

unordered_map<string, string> parseArgs(int argc, char *argv[]) {
    unordered_map<string, string> args;
    for (int i = 1; i < argc; i++) {
        string key = argv[i];
        if (key.rfind("--", 0) == 0) {
            if (i + 1 < argc && string(argv[i + 1]).rfind("--", 0) != 0) {
                args[key] = argv[i + 1];
                i++;
            } else {
                args[key] = "true";
            }
        }
    }
    return args;
}

void printBanner(){
    std::cout << R"(
-----------------------------------------------------------------------------------------------------------------    
|                      ______                                                                                    | 
|                   .-"      "-.   ██████╗ ██╗    ██╗ ██████╗██████╗  █████╗  ██████╗██╗  ██╗███████╗██████╗     | 
|                  /            \  ██╔══██╗██║    ██║██╔════╝██╔══██╗██╔══██╗██╔════╝██║ ██╔╝██╔════╝██╔══██╗    |
|                 |              | ██████╔╝██║ █╗ ██║██║     ██████╔╝███████║██║     █████╔╝ █████╗  ██████╔╝    |
|                 |,  .-.  .-.  ,| ██╔═══╝ ██║███╗██║██║     ██╔══██╗██╔══██║██║     ██╔═██╗ ██╔══╝  ██╔══██╗    |
|                 | )(__/  \__)( | ██║     ╚███╔███╔╝╚██████╗██║  ██║██║  ██║╚██████╗██║  ██╗███████╗██║  ██║    |
|                 |/     /\     \| ╚═╝      ╚══╝╚══╝  ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝    |
|       (@_       (_     ^^     _)                    Password Cracker Tool - CLI Version                        |
|  _     ) \_______\__|IIIIII|__/_____________________________________________________________________________   |
| (_)@8@8{}<________|-\IIIIII/-|______________________________________________________________________________>  |
|        )_/        \          /                                                                                 |
|       (@           `--------`                                                                                  |
-----------------------------------------------------------------------------------------------------------------

    )" <<std::endl;
}


int main(int argc, char *argv[]) {
    
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    printBanner();

    if (argc < 3) {
        cerr << "Usage:\n";
        cerr << "  --attack [brute|dict|hybrid|rules|rainbow] --target <password|hash> [other flags]\n";
        cerr << "  --mode generate --output <file> --charset <chars> --maxlen <n> [other flags]\n";
        cerr << "Examples:\n";
        cerr << "  ./pwcracker --attack brute --target cat --maxlen 4 --charset abc\n";
        cerr << "  ./pwcracker --attack dict --target cat --wordlist wordlists/test.txt\n";
        cerr << "  ./pwcracker --attack hybrid --target password123 --wordlist wordlists/test.txt --maxdigits 3\n";
        cerr << "  ./pwcracker --attack rules --target Password123! --wordlist wordlists/test.txt\n";
        cerr << "  ./pwcracker --attack rainbow --target 5f4dcc3b5aa765d61d8327deb882cf99 --table rainbow.db --charset abc --maxlen 6\n";
        cerr << "  ./pwcracker --mode generate --output rainbow.db --charset abc --maxlen 6 --chains 10000 --chainlen 1000\n";
        cerr << "  ./pwcracker --attack brute --target d077f244def8a70e5ea758bd8352fcd8 --maxlen 4 --charset abc --hash\n";
        return 1;
    }

    unordered_map<string, string> args = parseArgs(argc, argv);

    if (args.find("--mode") != args.end() && args["--mode"] == "generate") {
        if (args.find("--output") == args.end() || args.find("--charset") == args.end() || args.find("--maxlen") == args.end()) {
            cerr << "Error: --output, --charset, and --maxlen are required for rainbow table generation.\n";
            return 1;
        }

        string output = args["--output"];
        string charset = args["--charset"];
        int maxLen = stoi(args["--maxlen"]);
        int chainLength = (args.find("--chainlen") != args.end()) ? stoi(args["--chainlen"]) : 1000;
        int numChains = (args.find("--chains") != args.end()) ? stoi(args["--chains"]) : 10000;

        HashType hashType = HashType::MD5;
        if (args.find("--hashtype") != args.end()) {
            string ht = args["--hashtype"];
            if (ht == "sha1") hashType = HashType::SHA1;
            else if (ht == "sha256") hashType = HashType::SHA256;
            else if (ht == "ntlm") hashType = HashType::NTLM;
        }

        runRainbowTableGeneration(output, charset, maxLen, chainLength, numChains, hashType);
        return 0;
    }

    if (args.find("--attack") == args.end() || args.find("--target") == args.end()) {
        cerr << "Error: --attack and --target are required.\n";
        return 1;
    }

    string attackType = args["--attack"];
    string target = args["--target"];
    bool useHash = (args.find("--hash") != args.end());

    if (attackType == "brute") {
        if (args.find("--maxlen") == args.end() || args.find("--charset") == args.end()) {
            cerr << "Error: --maxlen and --charset are required for brute force.\n";
            return 1;
        }

        int maxLen = stoi(args["--maxlen"]);
        string charset = args["--charset"];
        runBruteForce(target, maxLen, charset, useHash);

    } else if (attackType == "dict") {
        if (args.find("--wordlist") == args.end()) {
            cerr << "Error: --wordlist is required for dictionary attack.\n";
            return 1;
        }

        string wordlist = args["--wordlist"];
        runDictionaryAttack(target, wordlist, useHash);

    } else if (attackType == "hybrid") {
        if (args.find("--wordlist") == args.end()) {
            cerr << "Error: --wordlist is required for hybrid attack.\n";
            return 1;
        }

        string wordlist = args["--wordlist"];
        int maxDigits = 3;
        if (args.find("--maxdigits") != args.end()) {
            maxDigits = stoi(args["--maxdigits"]);
        }
        runHybridAttack(target, wordlist, useHash, maxDigits);

    } else if (attackType == "rules") {
        if (args.find("--wordlist") == args.end()) {
            cerr << "Error: --wordlist is required for rule-based attack.\n";
            return 1;
        }

        string wordlist = args["--wordlist"];
        runRuleBasedAttack(target, wordlist, useHash);

    } else if (attackType == "rainbow") {
        if (args.find("--table") == args.end() || args.find("--charset") == args.end() || args.find("--maxlen") == args.end()) {
            cerr << "Error: --table, --charset, and --maxlen are required for rainbow table attack.\n";
            return 1;
        }

        string tablePath = args["--table"];
        string charset = args["--charset"];
        int maxLen = stoi(args["--maxlen"]);
        runRainbowAttack(target, tablePath, charset, maxLen);

    } else {
        cerr << "Error: Unknown attack type. Use 'brute', 'dict', 'hybrid', 'rules', or 'rainbow'.\n";
        return 1;
    }

    return 0;
}
