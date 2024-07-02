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

// void fillTransitionTable();
// void fillFollowTable();
bool isToken(const string& str, const set<string>& querySet);
// bool isHtmlToken(const string& str);
// vector<Token> scan(const string& input);
// void printTokens(const vector<Token>& tokens);
// bool parser(const vector<Token>& input);
// bool translationParser(const vector<Token>& input);

map< pair<string,string>, vector<string>> transitionTable;
map<string, vector<string>> followTable;

set<string> tokens = {"ID", "#", "##", "###", "*", "**", "_", "[", "!(", ")", "]", "$"};
map<string,string> tokensValue = {
    {"ID", "TEXT"},
    {"#", "H1"},
    {"##", "H2"},
    {"###", "H3"},
    {"*", "ITALIC"},
    {"**", "BOLD"},
    {"_", "UNDERLINE"},
    {"[", "LINK"},
    {"!(", "IMG"},
    {")", "IMG"},
    {"]", "LINK"},
    {"$", "EOP"}
};
map<string, string> htmlTranslations = {
    {"#", "<h1>"},
    {"##", "<h2>"},
    {"###", "<h3>"},
    {"*", "<i>"},
    {"**", "<b>"},
    {"_", "<u>"},
    {"[", "<a href='"},
    {"]", "' alt ='LINK' />"},
    {"!(", "<img src='"},
    {")", "' alt ='IMG' />"},
    {"ID", "<p>"}
};

struct Token{
    string type = "";
    string value = "";

    int line, column;

    Token(string type="", string value="",int line=0, int column=0){
        //isToken(type, tokens) ? this->type = type : this->type = "ID";
        this->type = type;
        this->value = value;
        this->line = line;
        this->column = column;
    }

    string translateHTML(){
        auto it = htmlTranslations.find(this->type);
        if (it != htmlTranslations.end()) {
            if (this->type == "ID") {
                return  this->value;
            } else if (this->type == "[") {
                return "<a href='" + this->value + "'>";
            } else if (this->type == "!(") {
                return "<img src='" + this->value + "' />";
            } else if (this->type == "*") {
                return "<i>" + this->value + "</i>";
            } else if (this->type == "**") {
                return "<b>" + this->value + "</b>";
            } else if (this->type == "_") {
                return "<u>" + this->value + "</u>";
            } else {
                return it->second + this->value + "</" + it->second.substr(1);
            }
        }
        return this->value;
    }
};

void insertStringAt(string& original, const string& toInsert, size_t index) {
    if (index > original.length()) {
        cerr << "Index out of bounds!" << endl;
        return;
    }
    original.insert(index, toInsert);
}
string tokensToHTML(const vector<Token>& tokens) {
    string html;
    stack<string> tagStack;
    bool insideTag = false;
    bool insideLinkOrImg = false;

    html += "<!DOCTYPE html>\n";
    html += "<html>\n";
    html += "<head>\n";
    html += "<meta charset=\"UTF-8\">\n";
    html += "<title>Generated HTML</title>\n";
    html += "</head>\n";
    html += "<body>\n";

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];

        if (token.type == "EOP") {
            break;
        }

        if (htmlTranslations.find(token.type) != htmlTranslations.end()) {
            string htmlTag = htmlTranslations[token.type];

            if (token.type == "ID") {
                if (insideLinkOrImg) {
                    html += token.value;
                } else if (!tagStack.empty() && (tagStack.top() == "<h1>" || tagStack.top() == "<h2>" || tagStack.top() == "<h3>" || tagStack.top() == "<i>" || tagStack.top() == "<b>" || tagStack.top() == "<u>")) {
                    html += token.value;
                } else {
                    html += "<p>" + token.value + "</p>\n";
                }
            } else if (token.type == "[" || token.type == "!(") {
                html += htmlTag;
                tagStack.push(htmlTag);
                insideLinkOrImg = true;
            } else if (token.type == "]" || token.type == ")") {
                html += htmlTag;
                if (!tagStack.empty()) tagStack.pop();
                insideLinkOrImg = false;
            } else {
                if (insideTag && !tagStack.empty()) {
                    html += "</" + tagStack.top().substr(1) + ">\n";
                    tagStack.pop();
                }
                html += htmlTag;
                tagStack.push(htmlTag);
                insideTag = true;
            }
        }
    }

    // Close any remaining open tags
    while (!tagStack.empty()) {
        html += "</" + tagStack.top().substr(1) + ">\n";
        tagStack.pop();
    }

    html += "</body>\n";
    html += "</html>\n";

    return html;
}

void saveToHTMLFile(const string& content, const string& filename) {
    ofstream outFile(filename);

    if (outFile.is_open()) {
        outFile << content;
        outFile.close();
        cout << "File saved successfully: " << filename << endl;
    } else {
        cerr << "Error: Could not open file " << filename << " for writing." << endl;
    }
}
void filltransitionTable(){
    // PROGRAM
    //EOP = End of Program
    transitionTable[{"PROGRAM","$"}] = {"STATEMENTS"};
    transitionTable[{"PROGRAM","ID"}] = {"STATEMENTS"};
    //TITLES
    transitionTable[{"PROGRAM","#"}] = {"STATEMENTS"};
    transitionTable[{"PROGRAM","##"}] = {"STATEMENTS"};
    transitionTable[{"PROGRAM","###"}] = {"STATEMENTS"};

    //STYLES
    transitionTable[{"PROGRAM","*"}] = {"STATEMENTS"};
    transitionTable[{"PROGRAM","**"}] = {"STATEMENTS"};
    transitionTable[{"PROGRAM","_"}] = {"STATEMENTS"};

    //URLS
    transitionTable[{"PROGRAM","["}] = {"STATEMENTS"};
    transitionTable[{"PROGRAM","!("}] = {"STATEMENTS"};
    
    //transitionTable
    //EOP = End of Program
    transitionTable[{"STATEMENTS","$"}] = {"EOP"};
    transitionTable[{"STATEMENTS","ID"}] = {"STATEMENT", "STATEMENTS"};
    //TITLES
    transitionTable[{"STATEMENTS","#"}] = {"STATEMENT", "STATEMENTS"};
    transitionTable[{"STATEMENTS","##"}] = {"STATEMENT", "STATEMENTS"};
    transitionTable[{"STATEMENTS","###"}] = {"STATEMENT", "STATEMENTS"};

    //STYLES
    transitionTable[{"STATEMENTS","*"}] = {"STATEMENT", "STATEMENTS"};
    transitionTable[{"STATEMENTS","**"}] = {"STATEMENT", "STATEMENTS"};
    transitionTable[{"STATEMENTS","_"}] = {"STATEMENT", "STATEMENTS"};

    //URLS
    transitionTable[{"STATEMENTS","["}] = {"STATEMENT", "STATEMENTS"};
    transitionTable[{"STATEMENTS","!("}] = {"STATEMENT", "STATEMENTS"};

    //transitionTable STATEMENT
    //ID = Identifier
    transitionTable[{"STATEMENT","ID"}] = {"STYLE"};
    //TITLES
    transitionTable[{"STATEMENT","#"}] = {"TITLE"};
    transitionTable[{"STATEMENT","##"}] = {"TITLE"};
    transitionTable[{"STATEMENT","###"}] = {"TITLE"};

    //STYLES
    transitionTable[{"STATEMENT","*"}] = {"STYLE"};
    transitionTable[{"STATEMENT","**"}] = {"STYLE"};
    transitionTable[{"STATEMENT","_"}] = {"STYLE"};

    //URLS
    transitionTable[{"STATEMENT","["}] = {"URL"};
    transitionTable[{"STATEMENT","!("}] = {"URL"};

    //transitionTable TOKENS TYPES
    //ID = Identifier
    transitionTable[{"STYLE","ID"}] = {"TEXT"};

    //TITLES
    transitionTable[{"TITLE","#"}] = {"H1"};
    transitionTable[{"TITLE","##"}] = {"H2"};
    transitionTable[{"TITLE","###"}] = {"H3"};

    //STYLES
    transitionTable[{"STYLE","*"}] = {"ITALIC"};
    transitionTable[{"STYLE","**"}] = {"BOLD"};
    transitionTable[{"STYLE","_"}] = {"UNDERLINE"};
    
    //URLS
    transitionTable[{"URL","["}] = {"LINK"};
    transitionTable[{"URL","!("}] = {"IMG"};

    //transitionTable TOKENS
    //TITLE
    transitionTable[{"H1","#"}] = {"#","STYLE"};
    transitionTable[{"H2","##"}] = {"##","STYLE"};
    transitionTable[{"H3","###"}] = {"###","STYLE"};

    //STYLES
    transitionTable[{"ITALIC","*"}] = {"*","STATEMENT","*"};
    transitionTable[{"BOLD","**"}] = {"**","STATEMENT","**"};
    transitionTable[{"UNDERLINE","_"}] = {"_","STATEMENT","_"};

    //URLS
    transitionTable[{"LINK","["}] = {"[","STYLE","]"};
    transitionTable[{"IMG","!("}] = {"!(","STYLE",")"};


    //FINAL TOKENS
    transitionTable[{"TEXT","ID"}] = {"ID"};
    transitionTable[{"EOP","$"}] = {}; //Empty
}

void fillFollowTable(){
    followTable["PROGRAM"] = {};
    followTable["STATEMENTS"] = {};
    followTable["STATEMENT"] = {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!("};

    followTable["TITLE"] =  {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!("};
    followTable["STYLE"] = {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!(","]",")"};
    followTable["URL"] =    {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!("};
    
    followTable["H1"] = {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!("};
    followTable["H2"] = {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!("};
    followTable["H3"] = {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!("};

    followTable["ITALIC"] =          {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!(","]",")"};
    followTable["BOLD"] =           {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!(","]",")"};
    followTable["UNDERLINE"] = {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!(","]",")"};

    followTable["LINK"] = {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!("};
    followTable["IMG"] =  {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!("};

    followTable["TEXT"] = {"#", "##" , "###" , "*" , "**","_", "ID", "[", "!(","]",")"};

    followTable["EOP"] = {};
}

void initTables(){
    filltransitionTable();
    fillFollowTable();
}

bool isToken(const string& str, const set<string>& querySet) {
    return querySet.find(str) != querySet.end();
}

vector<Token> scan(const string& input){
   vector<Token> tokensVector;
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
        if (isToken(str, tokens)) {
            // Si el caracter es el inicio de un token de marcado, lee el resto del token
            string nextStr = str + peekchar();
            while (isToken(nextStr, tokens) ){
                str += getchr();
                nextStr = str + peekchar();
            }
            token.type = str;
            token.value = tokensValue[str];
        }else if (isdigit(ch)) {
            while (isdigit(ch = peekchar())) {
                str += getchr();
            }
            token.type = "ID";
            token.value = str;
        }  else if (isalpha(ch)) {
            while (isalpha( peekchar()) || peekchar() == ' ' || peekchar() == '.') { // Agrega un espacio a los caracteres permitidos para el texto sin formato
                str += getchr();
                
            }
            token.type = "ID";
            token.value = str;
        } else if (ispunct(ch)&& !isToken(string(1, ch), tokens)) { // Manejar caracteres de puntuacion como parte de texto plano
            while (ispunct(peekchar()) && !isToken(string(1, peekchar()), tokens)) {
                str += getchr();
            }
            if (isToken(str, tokens)) { // Si se formo un token especial como "!("
                token.type = str;
                token.value = tokensValue[str];
            } else { // Si es solo puntuacion normal
                token.type = "ID";
                token.value = str;
            }
            } else { // Si el caracter no es el final de la cadena
            cerr << " SCAN ERROR en (" << line << ":" << column << "): caracter inesperado '" << ch << "'\n";
            token.type = "ERROR";
            token.value = str;
        }

        return token;
    };

    Token token;
    while ((token = getToken()).type != "$") {
        if(token.type != "ERROR"){
            tokensVector.push_back(token);
        }
    }
    tokensVector.push_back({"$", "EOP", line, column}); // Add end-of-program marker

    return tokensVector;
}

void printTokens (const vector<Token>& tokens) {
    cout << "INFO SCAN - Start scanning...\n";
    for (const auto& token : tokens) {
        cout << "DEBUG SCAN - " << token.type << " [ " << token.value << " ] found at (" << token.line << ":" << token.column << ")\n";
    }
    cout << "INFO SCAN - Completed with 0 errors\n"<<endl;
}

bool parser(const vector<Token>& input) {
    cout << "==================== PARSER ====================\n" << endl;

    bool flag = true;

    stack<string> st;
    pair<string, string> pairIndex;
    string currentToken;

    st.push("PROGRAM");
    for (size_t i = 0; i < input.size(); ++i) {
        
        string top = st.top();


        currentToken = input[i].type;

        cout << "DEBUG PARSER - Current token: " << currentToken << " | Stack top: " << top << endl;

        // nullable detected
        if (top == "") {
            cout<<"DEBUG PARSER - Epsilon founded"<<endl;
            st.pop();
            --i;
            continue;
        }
        //terminal founded
        if (top == currentToken) { 
            st.pop();
            continue;
        }
        
        //current
        pairIndex = make_pair(top, currentToken);

        //No terminal founded
        if (transitionTable.find(pairIndex) != transitionTable.end()) {
            st.pop();

            // Adiciona las producciones al stack
            //cout << "DEBUG PARSER - Applying production for: " << top << " -> ";
            for (int r = transitionTable[pairIndex].size() - 1; r >= 0; --r) {
                st.push(transitionTable[pairIndex][r]);
            }
            --i;
        } else { // Manejo de errores
            flag = false;

            bool errorType = true; 
            for (const auto& it : transitionTable) {
                if (it.first.first == top) {
                    errorType = false; 
                    break;
                }
            }

            if (errorType) {
                cout << "[ERROR] : Parser expected ->  " << top << "  instead found : "<< currentToken << endl;
                st.pop();
            } else { 
                cout << "[ERROR] : Unexpected symbol -> " << currentToken
                     << " , with FOLLOWING RULE : ()" << top << " )" << endl;

                // if error founded flag to skip 
                bool skipInput = true;
                for (const auto& follow : followTable[top]) {
                    if (follow == currentToken) {
                        skipInput = false;
                        break;
                    }
                }

                if (!skipInput) {
                    st.pop();
                    cout << "[ TOKEN IS FOLLOW  ] ->" << currentToken << endl;
                } else {
                    cout << "[ TOKEN IS NOT FOLLOW ] -> " << currentToken << endl;
                }
            }
        }
    }

    if (st.size() == 0 || (st.size() == 1 && st.top() == "$")) 
    {
        cout << "DEBUG PARSER - Final stack state: Empty or [EOP]" << endl;
        return flag;
    }

    cout << "Still tokens in stack:" << endl;
    while (st.size() > 1) {
        cout << st.top() << endl;
        st.pop();
    }
    return false;
}

void mdToHTML(){

}


int main(){
   
    initTables();
    ifstream file("input.txt");

    if (!file) {
        cerr << "No se pudo abrir el archivo input.txt";
        return 1; // Retorna un código de error
    }

    stringstream buffer;

    buffer << file.rdbuf();
    string input = buffer.str();
    vector<Token> tokens = scan(input);
    printTokens(tokens);

     string htmlContent = tokensToHTML(tokens);
    saveToHTMLFile(htmlContent, "output.html");

    parser(tokens);

   

    return 0;
}
