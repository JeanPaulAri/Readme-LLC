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
#include <stack>

using namespace std;


struct Token{
    string type;
    string value;

    int line, column;
};
//declaracion de funciones:
void fillGrammar();
bool isToken(const string& str, const map<string, string>& queryMap);
bool isHtmlToken(const string& str);
vector<Token> scan(const string& input);
void printTokens(const vector<Token>& tokens);
bool parser(const vector<Token>& input, const map<pair<string, string>, vector<string>>& grammar);
bool translationParser(const vector<Token>& input);

map< pair<string,string>, vector<string>> grammar;

map<string, string> generalTokenTypesS = {
    {"#","H1"},
    {"##","H2"},
    {"###","H3"},
    {"*","ITALIC"},
    {"+","BOLD"},
    {"_","UNDERLINE"},
    {"ID", "TEXT"},
    {"$","EOP"}
};

map<string, string> generalTokenTypes = {
    {"H1","#"},
    {"H2","##"},
    {"H3","###"},
    {"ITALIC","*"},
    {"BOLD","+"},
    {"UNDERLINE","_"},
   {"TEXT", "ID"},
    {"EOP","$"}
};

map<string, string> htmlTokenTypesS = {
    {"<h1>","H1"},
    {"<h2>","H2"},
    {"<h3>","H3"},
    {"<b>","BOLD"},
    {"<i>","ITALIC"},
    {"</h1>","EOP"},
    {"</h2>","EOP"},
    {"</h3>","EOP"},
    {"</b>","EOP"},
    {"</i>","EOP"},
    {"$","EOP"}
};
map<string, string> htmlTokenTypes = {
    {"H1","<h1>"},
    {"H2","<h2>"},
    {"H3","<h3>"},
    {"BOLD","<b>"},
    {"ITALIC","<i>"},
    {"EOP","</h1>"},
    { "EOP","</h2>"},
    { "EOP","</h3>"},
    {"EOP","</b>"},
    {"EOP","</i>"},
    {"EOP","$"}
};

void fillGrammar(){
    grammar[{"PROGRAM","#"}] = {"STATEMENTS"};
    grammar[{"PROGRAM","##"}] = {"STATEMENTS"};
    grammar[{"PROGRAM","###"}] = {"STATEMENTS"};
    grammar[{"PROGRAM","*"}] = {"STATEMENTS"};
    grammar[{"PROGRAM","+"}] = {"STATEMENTS"};
    grammar[{"PROGRAM","_"}] = {"STATEMENTS"};
    

    grammar[{"STATEMENTS","#"}] = {"STATEMENT", "STATEMENTS"};
    grammar[{"STATEMENTS","##"}] = {"STATEMENT", "STATEMENTS"};
    grammar[{"STATEMENTS","###"}] = {"STATEMENT", "STATEMENTS"};
    grammar[{"STATEMENTS","*"}] = {"STATEMENT", "STATEMENTS"};
    grammar[{"STATEMENTS","+"}] = {"STATEMENT", "STATEMENTS"};
    grammar[{"STATEMENTS","_"}] = {"STATEMENT", "STATEMENTS"};
    grammar[{"STATEMENTS","$"}] = {}; // empty production for end of program
    

    grammar[{"STATEMENT","#"}] = {"H1", "TEXT", "EOP"};
    grammar[{"STATEMENT","##"}] = {"H2", "TEXT", "EOP"};
    grammar[{"STATEMENT","###"}] = {"H3", "TEXT", "EOP"};
    grammar[{"STATEMENT","*"}] = {"ITALIC", "TEXT","ITALIC", "EOP"};
    grammar[{"STATEMENT","+"}] = {"BOLD", "TEXT", "BOLD","EOP"};
    grammar[{"STATEMENT","_"}] = {"UNDERLINE", "TEXT", "UNDERLINE","EOP"};
 
    grammar[{"H1","#"}] = {"#"};
    grammar[{"H2","##"}] = {"##"};
    grammar[{"H3","###"}] = {"###"};

    grammar[{"ITALIC","*"}] = {"*"};
    grammar[{"BOLD","+"}] = {"+"};
    grammar[{"UNDERLINE","_"}] = {"_"};
    
    grammar[{"TEXT","ID"}] = {"ID"};
    grammar[{"EOP","$"}] = {"$"};
}


bool isToken(const string& str, const map<string, string>& queryMap) {
    return queryMap.find(str) != queryMap.end();
}

bool isHtmlToken(const string& str) {
    return htmlTokenTypes.find(str) != htmlTokenTypes.end();
}

vector<Token> scan(const string& input) {
    vector<Token> tokens;
    size_t pos = 0;
    int line = 1, column = 1;

    auto getchr = [&]() -> char {
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

    auto peekchar = [&]() -> char {
        if (pos >= input.size()) {
            return '\0';
        }
        return input[pos];
    };

    auto getToken = [&]() -> Token {
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
        if (generalTokenTypesS.find(str) != generalTokenTypesS.end()) {
            // Si el caracter es el inicio de un token de marcado, lee el resto del token
            string nextStr = str + peekchar();
            while (generalTokenTypesS.find(nextStr) != generalTokenTypesS.end()) {
                str += getchr();
                nextStr = str + peekchar();
            }
            token.type = generalTokenTypesS[str];
            token.value = str;
        }else if (isdigit(ch)) {
            while (isdigit(ch = peekchar())) {
                str += getchr();
            }
            token.type = "ID";
            token.value = str;
        }  else if (isalpha(ch)) {
            while (isalpha(ch = peekchar()) || ch == ' ') { // Agrega un espacio a los caracteres permitidos para el texto sin formato
                str += getchr();
                ch = peekchar();
            }
            token.type = "ID";
            token.value = str;
        } else { // Si el caracter no es el final de la cadena
            cerr << " SCAN ERROR en (" << line << ":" << column << "): caracter inesperado '" << ch << "'\n";
            exit(1);
        }

        return token;
    };

    Token token;
    while ((token = getToken()).type != "$") {
        tokens.push_back(token);
    }
    tokens.push_back({"EOP", "$", line, column}); // Add end-of-program marker

    return tokens;
}

void printTokens (const vector<Token>& tokens) {
    cout << "INFO SCAN - Start scanning...\n";
    for (const auto& token : tokens) {
        cout << "DEBUG SCAN - " << token.type << " [ " << token.value << " ] found at (" << token.line << ":" << token.column << ")\n";
    }
    cout << "INFO SCAN - Completed with 0 errors\n"<<endl;
}

bool parser(const vector<Token> &input, const map<pair<string, string>, vector<string>> &grammar) {
    cout<< "==================== PARSER ====================\n" <<endl;
    stack<string> st;
    st.push("PROGRAM");

    int inputIndex = 0;

    while (!st.empty() && inputIndex < input.size()) {
        string top = st.top();
        st.pop();
        cout<< inputIndex << " , " <<  generalTokenTypes[input[inputIndex].type] << ",  " << top << endl;
        if (grammar.find({top, generalTokenTypes[input[inputIndex].type]}) != grammar.end()) {
            const auto &production = grammar.at({top, generalTokenTypes[input[inputIndex].type]});
            cout << "Expanding [" << top << "]  :"<<endl;
            for (const auto &sym : production) {
                cout <<" |-> "<< sym << endl;
            }
            cout << endl;
            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                if(!it->empty()){
                st.push(*it);}
            }
        } else if ((top == "TEXT" & input[inputIndex].type == "ID") || top == generalTokenTypes[input[inputIndex].type] || top == "EOP") {
            inputIndex++;


        } else {
            cerr << "Parse error at token " << input[inputIndex].value << " of type " << input[inputIndex].type << endl;
            return false;
        }
    }

    return st.empty() && inputIndex == input.size();
}

bool translationParser(const vector<Token> &input) {
    stack<string> st;
    st.push("PROGRAM");

    int inputIndex = 0;

    while (!st.empty() && inputIndex < input.size()) {
        string top = st.top();
        st.pop();

        if (grammar.find({top,generalTokenTypes[input[inputIndex].type]}) != grammar.end()) {
            const auto &production = grammar.at({top,generalTokenTypes[input[inputIndex].type]});
            cout << "Translating: " << top << " -> ";
            for (const auto &sym : production) {
                cout << sym << " ";
            }
            cout << endl;
            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                if(!it->empty()){
                st.push(*it);}
            }
        } else if (top == generalTokenTypes[input[inputIndex].type] || top == "TEXT" || top == "EOP") {
            inputIndex++;
        }else {
            cerr << "Translation parse error at token " << input[inputIndex].value << " of type " << input[inputIndex].type << endl;
            return false;
        }
    }

    return st.empty() && inputIndex == input.size();
}


auto printTokenRecognized = [](const std::string& str) {
        if (isToken(str, generalTokenTypes)) {
            cout << "TOKEN (" << str << ") RECOGNIZED -> \"" << generalTokenTypes[str] << "\"\n";
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

    fillGrammar();

    ifstream file("input.txt");

    if (!file) {
        cerr << "No se pudo abrir el archivo input.txt";
        return 1; // Retorna un código de error
    }
    // string line;
    // vector<Token> input;
    // int lineNum = 1;
    // while (getline(file, line)) {
    //     istringstream iss(line);
    //     string token;
    //     int columnNum = 1;

    //      while (iss >> token) {
    //         size_t pos = 0;
    //         while (pos < token.length()) {
    //             string temp;
    //             while (pos < token.length() && !isToken(string(1, token[pos]),generalTokenTypes) && !isHtmlToken(string(1, token[pos]))) {
    //                 temp += token[pos++];
    //             }
    //             if (!temp.empty()) {
    //                 input.push_back({"ID", temp, lineNum, columnNum});
    //                 columnNum += temp.length();
    //             }
    //             if (pos < token.length()) {
    //                 string tok(1, token[pos]);
    //                 pos++;
    //                 while (pos < token.length() && (isToken(tok + token[pos],generalTokenTypes) || isHtmlToken(tok + token[pos]))) {
    //                     tok += token[pos++];
    //                 }
    //                 if (pos < token.length() && (isToken(tok + token[pos],generalTokenTypes) || isHtmlToken(tok + token[pos]))) {
    //                     tok += token[pos++];
    //                 }
    //                 if (isHtmlToken(tok)) {
    //                     input.push_back({htmlTokenTypes[tok], tok, lineNum, columnNum});
    //                 } else {
    //                     input.push_back({generalTokenTypes[tok], tok, lineNum, columnNum});
    //                 }
    //                 columnNum += tok.length();
    //             }
    //         }
    //         columnNum++;
    //     }
    //     lineNum++;
    // }
    //     // if (isToken(line, tokenTypes)) {
    //     //     input.push_back({tokenTypes[line], line, lineNum, 1});
    //     // } else {
    //     //     input.push_back({"ID", line, lineNum, 1});
    //     // }
    // input.push_back({"EOP","$",lineNum,1});  // Add end-of-program marker

    stringstream buffer;

    buffer << file.rdbuf();
    string input = buffer.str();
    vector<Token> tokens = scan(input);
    printTokens(tokens);


    // for (const auto &token : input) {
    //     cout << "Token: " << token.value << " Type: " << token.type << " Line: " << token.line << " Column: " << token.column << endl;
    // }

    if (parser(tokens, grammar)) {
        cout << "Parsing successful!" << endl;
    } else {
        cout << "Parsing failed." << endl;
    }
    

    // if (translationParser(tokens)) {
    //     cout << "Translation parsing successful!" << endl;
    // } else {
    //     cout << "Translation parsing failed." << endl;
    // }
    //  input = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    // vector<Token> tokens;
    // Token token;
    // while ((token = getToken()).type != "$") {
    //     tokens.push_back(token);
    // }

    //Node ast = parse(tokens);
    //printTokens(tokens);
    return 0;
}
