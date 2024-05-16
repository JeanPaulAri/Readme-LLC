#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <list>
#include <sstream>
#include <fstream>

using namespace std;

enum class State{
    START,
    H1,
    H2,
    H3,
    ITALIC,
    BOLD,
    UNDERLINE,
    TEXT,
    PRINT_KEY,
    ASSIGN_OP,
    OPEN_PAR,
    CLOSE_PAR,
    ADD_OP,
    END
};

enum class Action {
    Shift,
    Reduce,
    Accept,
    Error
};

struct ParseTableEntry {
    Action action;
    State nextState;
};

struct Token{
    string type;
    string value;

    int line, column;
};

map<State, map<string, ParseTableEntry>> parseTable = {
    {State::START, {
        {"H1", {Action::Shift, State::H1}},
        {"H2", {Action::Shift, State::H2}},
        {"H3", {Action::Shift, State::H3}},
        {"ITALIC", {Action::Shift, State::ITALIC}},
        {"BOLD", {Action::Shift, State::BOLD}},
        {"UNDERLINE", {Action::Shift, State::UNDERLINE}},
        {"TEXT", {Action::Shift, State::TEXT}},
        {"$", {Action::Accept, State::END}}
    }},
    {State::H1, {
        {"TEXT", {Action::Shift, State::TEXT}},
        {"$", {Action::Reduce, State::START}}
    }},
    {State::H2, {
        {"TEXT", {Action::Shift, State::TEXT}},
        {"$", {Action::Reduce, State::START}}
    }},
    {State::H3, {
        {"TEXT", {Action::Shift, State::TEXT}},
        {"$", {Action::Reduce, State::START}}
    }},
    {State::ITALIC, {
        {"TEXT", {Action::Shift, State::TEXT}},
        {"$", {Action::Reduce, State::START}}
    }},
    {State::BOLD, {
        {"TEXT", {Action::Shift, State::TEXT}},
        {"$", {Action::Reduce, State::START}}
    }},
    {State::UNDERLINE, {
        {"TEXT", {Action::Shift, State::TEXT}},
        {"$", {Action::Reduce, State::START}}
    }},
    {State::TEXT, {
        {"H1", {Action::Reduce, State::START}},
        {"H2", {Action::Reduce, State::START}},
        {"H3", {Action::Reduce, State::START}},
        {"ITALIC", {Action::Reduce, State::START}},
        {"BOLD", {Action::Reduce, State::START}},
        {"UNDERLINE", {Action::Reduce, State::START}},
        {"$", {Action::Reduce, State::START}}
    }}
};

map<string, string> tokenTypes = {
    {"#", "H1"},
    {"##", "H2"},
    {"###", "H3"},
    {"*", "ITALIC"},
    {"**", "BOLD"},
    {"__", "UNDERLINE"},
    {"print", "PRINT_KEY"},
    {"=", "ASSIGN_OP"},
    {"(", "OPEN_PAR"},
    {")", "CLOSE_PAR"},
    {"+", "ADD_OP"},
    {"$", "EOP"}
};


struct Node{
    string type;
    string value;
    vector<Node> children;
};

auto parse = [](vector<Token> tokens) -> Node{
    Node root;
    root.type = "PROGRAM";
    
    for(auto& token:tokens){
        Node node;
        node.type = token.type;
        node.value = token.value;

        if(tokenTypes.find(token.type) != tokenTypes.end()){
            root.children.push_back(node);
        }

    }

    return root;
};

auto parse2 = [](vector<Token> tokens) -> Node {
    Node root;
    root.type = "PROGRAM";

    State currentState = State::START;
    for(auto& token:tokens){
        if(parseTable[currentState].find(token.type) == parseTable[currentState].end()){
            cerr << "Error en (" << token.line << ":" << token.column << "): token inesperado '" << token.value << "'\n";
            exit(1);
        }

        ParseTableEntry entry = parseTable[currentState][token.type];
        switch(entry.action){
            case Action::Shift:
                currentState = entry.nextState;
                break;
            case Action::Reduce:
                // Aquí debes implementar la lógica para reducir la pila de análisis
                break;
            case Action::Accept:
                return root;
            case Action::Error:
                cerr << "Error en (" << token.line << ":" << token.column << "): token inesperado '" << token.value << "'\n";
                exit(1);
        }

        Node node;
        node.type = token.type;
        node.value = token.value;
        root.children.push_back(node);
    }

    return root;
};
string input;
size_t pos = 0;
int line = 1, column = 1;

auto getchr = []() -> char {
      if (pos >= input.size()) {
        return '\0';
    }
    char ch = input[pos++];
    if (ch == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return ch;
};

auto peekchar = []() -> char {
    if (pos >= input.size()) {
        return '\0';
    }
    return input[pos];
};

auto getToken = []() -> Token {
       char ch;
    while ((ch = getchr()) != '\0' && isspace(ch)) {
        // Ignora los espacios en blanco
    }

    if (ch == '\0') {
        return {"$", "", line, column};
    }

    Token token;
    token.line = line;
    token.column = column - 1;

    string str(1, ch);
    if (tokenTypes.find(str) != tokenTypes.end()) {
        // Si el carácter es el inicio de un token de marcado, lee el resto del token
        string nextStr = str + peekchar();
        while (tokenTypes.find(nextStr) != tokenTypes.end()) {
            str += getchr();
            nextStr = str + peekchar();
        }
        token.type = tokenTypes[str];
        token.value = str;
    } else if (isdigit(ch)) {
        while (isdigit(ch = peekchar())) {
            str += getchr();
        }
        token.type = "INT";
        token.value = str;
    } else if (isalpha(ch)) {
        while (isalpha(ch = peekchar()) || ch == ' ') { // Agrega un espacio a los caracteres permitidos para el texto sin formato
            str += getchr();
            ch = peekchar();
        }
        token.type = "ID"; 
        token.value = str;
    } else { // Si el carácter no es el final de la cadena
        cerr << "Error en (" << line << ":" << column << "): caracter inesperado '" << ch << "'\n";
        exit(1);
    }

    return token;
};
auto scan = []() -> vector<Token> {
    vector<Token> tokens;
    Token token;
    while ((token = getToken()).type != "$") {
        tokens.push_back(token);
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

auto printTokens = [](const vector<Token>& tokens) {
    cout << "INFO SCAN - Start scanning...\n";
    for (const auto& token : tokens) {
        cout << "DEBUG SCAN - " << token.type << " [ " << token.value << " ] found at (" << token.line << ":" << token.column << ")\n";
    }
    cout << "INFO SCAN - Completed with 0 errors\n";
};

int main(){
    ifstream file("input.txt");

    if (!file) {
    cerr << "No se pudo abrir el archivo input.txt";
    return 1; // Retorna un código de error
    }

     input = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    vector<Token> tokens;
    Token token;
    while ((token = getToken()).type != "$") {
        tokens.push_back(token);
    }

    Node ast = parse(tokens);
    printTokens(tokens);
    return 0;
}
