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

auto isToken = [](string str, map<string,string> queryMap) -> bool {
    return tokenTypes.find(str) != queryMap.end();
};


map<string, string> tokenTypes = {
    {"print", "PRINT_KEY"},
    {"=", "ASSIGN_OP"},
    {"(", "OPEN_PAR"},
    {")", "CLOSE_PAR"},
    {"+", "ADD_OP"},
    {"$", "EOP"}
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

    printTokenMap (tokenTypes);
    return 0;
}