#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <list>
#include <sstream>

using namespace std;

map<string, string> tokenTypes = {
    {"print", "PRINT_KEY"},
    {"=", "ASSIGN_OP"},
    {"(", "OPEN_PAR"},
    {")", "CLOSE_PAR"},
    {"+", "ADD_OP"},
    {"$", "EOP"}
};

// map<string,string> tokenMd = {
//     {"#","TITLE"}, 
// }

struct Token{
    string type;
    string value;

    int line, column;
};

auto scan = [](string input) -> vector<Token>{

    vector<Token> tokens;
    istringstream iss(input);

    string word;
    int input_line = 1, input_column = 1;

    while(iss>> word){
        Token token;

        bool isInt = true;
        for (char symbol: word){
            if (!isdigit(symbol)){
                isInt = false;
                break;
            }
        }

        if( tokenTypes.find(word) != tokenTypes.end()){
            token.type = tokenTypes[word];
        }else if(isInt){
            token.type = "INT";
        }else{
                token.type = "ID";
        }

        token.value = word;
        token.line = input_line;
        token.column = input_column;

        tokens.push_back(token);

        input_column += word.length() +1;
    }


    return tokens;
};


auto isToken = [](string str, map<string,string> queryMap) -> bool {
    return queryMap.find(str) != queryMap.end();
};


auto printTokenRecognized = [](const std::string& str) {
        if (isToken(str, tokenTypes)) {
            cout << "TOKEN (" << str << ") RECOGNIZED -> \"" << tokenTypes[str] << "\"\n";
        } else {
            cout << "Not a token: " << str << "\n";
        }
    };
void printTokenMap(map<string, string> tokenMap) { //function to validate map of tokenTypes inputs avaliable
    cout << "Token Map Contents:\n";
    for (const auto& pair : tokenMap) {
        cout << pair.first << " -> " << pair.second << "\n";
    }
}

int main(){
    string input = "a = 2 + 7\nprint(a) # este es un comentario\n$";

    // Escanear el string
    vector<Token> tokens = scan(input);

    // Imprimir información de los tokens
    cout << "INFO SCAN - Start scanning...\n";
    for (const auto& token : tokens) {
        cout << "DEBUG SCAN - " << token.type << " [ " << token.value << " ] found at (" << token.line << ":" << token.column << ")\n";
    }
    cout << "INFO SCAN - Completed with 0 errors\n";
    return 0;
}