#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <list>
#include <stack>
#include <algorithm>
#include <cstdio>
using namespace std;


#define BUFSIZE 256

enum type_of_lex {
    LEX_NULL, /*0*/

    LEX_AND, LEX_DO, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_INT, LEX_REAL, LEX_NOT, LEX_OR, LEX_PROGRAM, /*10*/
    LEX_READ, LEX_STRING, LEX_TRUE, LEX_GOTO, LEX_BREAK, LEX_WHILE, LEX_WRITE, LEX_FOR, /*18*/
    TYPE_STRUCT, /*19*/

    LEX_QUOTE, /*20*/
    LEX_BEGIN, LEX_END, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_NEQ, LEX_ASSIGN, LEX_LPAREN, /*28*/
    LEX_RPAREN, LEX_EQ, LEX_LSS, LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_GEQ, /*38*/
    LEX_PERCENT, /*39*/

    LEX_NUM, /*40*/
    LEX_STR, /*41*/
    LEX_RNUM, /*42*/
    LEX_ID, /*43*/
    POLIZ_LABEL, /*44*/
    POLIZ_ADDRESS, /*45*/
    POLIZ_GO, /*46*/
    POLIZ_FGO, /*47*/

};

class Ident {
    string name;
    string string_value;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;
    bool bool_value;

public:
    Ident() {
        declare = false;
        assign = false;
    }

    bool operator==(const string &s) const {
        return name == s;
    }

    Ident(const string new_str) {
        name = new_str;
        declare = false;
        assign = false;
    }

    string get_name() const {
        return name;
    }

    void put_name(string new_name) {
        name = new_name;
    }

    bool get_declare() const {
        return declare;
    }

    void put_declare() {
        declare = true;
    }

    type_of_lex get_type() const {
        return type;
    }

    void put_type(type_of_lex t) {
        type = t;
    }

    bool get_assign() const {
        return assign;
    }

    void put_assign() {
        assign = true;
    }

    int get_value() const {
        return value;
    }

    void put_value(int val) {
        value = val;
    }

    string get_str_val() const {
        return string_value;
    }

    void put_str_val(string str) {
        string_value = str;
    }
};

class Lex {
    type_of_lex t_lex;
    int v_lex;
    string s_lex;
    double real_lex;

public:
    Lex(type_of_lex t = LEX_NULL, int v = 0) : t_lex(t), v_lex(v), s_lex(""), real_lex(0) {} // Конструктор общий
    //Lex(type_of_lex t, int v) : t_lex(t), v_lex(v), s_lex(""), real_lex(0) {} // Конструктор int
    Lex(type_of_lex t, string strr): t_lex(t), v_lex(0), s_lex(strr), real_lex(0) {} // Конструктор для string лексем
    Lex(type_of_lex t, double r): t_lex(t), v_lex(0), s_lex(""), real_lex((r)) {} // Конструктор для real лексем

    type_of_lex get_type() const {
        return t_lex;
    }

    int get_value() const {
        return v_lex;
    }

    string get_str() const {
        return s_lex;
    }

    void put_type(type_of_lex t) {
        t_lex = t;
    }

    friend ostream &operator<<(ostream &s, Lex l);
};

int search(string buf, const char **list);

class Scanner {
    static FILE *fp;
    void gc();

    class State {
    public:
        virtual State *operator()(char c) = 0;
    };

    class H : public State {
    public:
        State *operator ()(char c);
    };

    class IDENT : public State {
    public:
        State *operator ()(char c);
    };

    class NUMB : public State {
    public:
        State *operator ()(char c);
    };

    class R_NUMB : public State {
    public:
        State *operator ()(char c);
    };

    class COM : public State {
    public:
        State *operator ()(char c);
    };

    class ALE : public State {
    public:
        State *operator ()(char c);
    };

    class NEQ : public State {
    public:
        State *operator ()(char c);
    };

    class SLASH : public State {
    public:
        State *operator ()(char c);
    };

    class STR : public State {
    public:
        State *operator ()(char c);
    };

    class COM_TIMES : public State {
    public:
        State *operator ()(char c);
    };

    class ERR : public State {
    public:
        State *operator ()(char c) {
            return new ERR;
        }
    };

    class OK : public State {
    public:
        State *operator ()(char c) {
            cout << "OK\n";
            return new OK;
        }
    };


public:
    static string buf;
    static char c;
    static int d, j, k;
    static double r;
    static bool end_flag;
    static int balance;
    bool st_start = false;
    static const char *TW[], *TD[];
    static list<Lex> Lexems;

    Scanner(const char *program);

    void get_lex();
    void lex_analyzer();

    ~Scanner() { fclose(fp); }
};

vector<Ident> TID; //таблица идентификаторов анализируемой программы
int Scanner::balance = 0;
//bool Scanner::lex_flag = true;
//таблица служебных слов модельного языка
const char *
        Scanner::TW[] = {
            "",     // z
            "and",
            "bool",
            "do",
            "else",
            "if",
            "false",
            "int",
            "not",
            "or",
            "program",

            "read",
            "string",
            "true",
            "goto",
            "break",
            "while",
            "write",
            "for",
            "struct", NULL
        };



//таблица ограничителей модельного языка
const char *
        Scanner::TD[] = {
            "\"", "{", "}", ";", ",", ":", "!=", "=", "(", ")",
            //                 0     1    2    3    4    5    6    7     8    9    10
            "==", "<", ">", "+", "-", "*", "/", "<=", ">=", "%", NULL
        };
//                 11    12   13  14   15    16   17   18    19    20   21

int search(const string buf, const char **list) {
    int i = 0;
    while (list[i]) {
        if (buf == list[i])
            return i;
        ++i;
    }
    return 0;
}

void Scanner::gc() {
    c = fgetc(fp);
    if (feof(fp)) {
        //cout << "c = " << c << '\n';
        if (balance != 0) {
            char s[BUFSIZE];
            sprintf(s, "Не хватает закрывающей скобки\n");
            string str = string(s);
            throw str;
        }
        end_flag = true;
    }

}

// Объявление статических членов класса Scanner
string Scanner::buf = "";
int Scanner::d;
int Scanner::j;
int Scanner::k;
double Scanner::r;
list<Lex> Scanner::Lexems;
FILE* Scanner::fp;
char Scanner::c;
bool Scanner::end_flag = false;
// ---------------------------------------------

Scanner::Scanner(const char *program) {
    if (!(fp = fopen(program, "r"))) {
        char s[BUFSIZE];
        sprintf(s, "Не удалось открыть файл\n");
        string str = string(s);
        throw str;
    }
}

int put(const string &buf) {
    vector<Ident>::iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end())
        return k - TID.begin();
    TID.push_back(Ident(buf));
    return TID.size() - 1;
}

Scanner::State *Scanner::IDENT::operator()(char c) {
    if (isalpha(c) || isdigit(c) || c == '.' || c == '_') {
        buf.push_back(c);
        return new IDENT;
    } else {
        ungetc(c, fp);
        if ((j = search(buf, TW))) {
            Lexems.push_back(Lex((type_of_lex) j, j));
            return new OK;
        } else {
            j = put(buf);
            Lexems.push_back(Lex(LEX_ID, j));
            return new OK;
        }
    }
}

Scanner::State *Scanner::H::operator()(char c) {
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        return new H;
    } else if (c == '{') {
        balance++;
        Lexems.push_back(Lex(LEX_BEGIN, '{'));
        return new OK;
    } else if (c == '}') {
        balance--;
        if (balance < 0) {
            cout << "Непонятная закрывающая скобка.\n";
            return new ERR;
        }
        Lexems.push_back(Lex(LEX_END, '}'));
        return new OK;
    } else if (isalpha(c)) {
        buf.push_back(c);
        return new IDENT;
    } else if (isdigit(c)) {
        d = c - '0';
        k = 1;
        return new NUMB;
    } else if (c == '/') {
        buf.push_back(c);
        return new SLASH;
    } else if (c == '=' || c == '<' || c == '>') {
        buf.push_back(c);
        return new ALE;
    } else if (c == '!') {
        buf.push_back(c);
        return new NEQ;
    } else if (c == '"') {
        return new STR;
    } else {
        cout << buf;
        buf.push_back(c);
        if ((j = search(buf, TD))) {
            Lexems.push_back(Lex((type_of_lex) (j + (int) LEX_QUOTE), j));
            return new OK;
        }
    }
}

Scanner::State *Scanner::NUMB::operator()(char c) {
    if (isdigit(c)) {
        d = d * 10 + (c - '0');
        return new NUMB;
    }
    if (c == '.') {
        r = (double)d;
        return new R_NUMB;
    }
    ungetc(c, fp);
    Lexems.push_back(Lex(LEX_NUM, d));
    return new OK;
}

Scanner::State *Scanner::R_NUMB::operator()(char c) {
    if (isdigit(c)) {
        r = r + ((double)(c - '0'))/(10*k);
        cout << r << '\n';
        k++;
        return new R_NUMB;
    }
    ungetc(c, fp);
    Lexems.push_back(Lex(LEX_RNUM, r));
    return new OK;
}


Scanner::State *Scanner::SLASH::operator()(char c) {
    if (c == '*') {
        buf.pop_back();
        return new COM;
    } else {
        ungetc(c, fp);
        j = search(buf, TD);
        Lexems.push_back(Lex(LEX_SLASH, j));
        return new OK;
    }
}

Scanner::State *Scanner::COM::operator()(char c) {
    if (c == '*')
        return new COM_TIMES;
    return new COM;
}

Scanner::State *Scanner::COM_TIMES::operator()(char c) {
    if (c == '/')
        return new H;
    return new COM;
}

Scanner::State *Scanner::ALE::operator()(char c) {
    if (c == '=') {
        buf.push_back(c);
    } else {
        ungetc(c, fp);
    }
    j = search(buf, TD);
    Lexems.push_back(Lex((type_of_lex) (j + (int) LEX_QUOTE), j));
    return new OK;
}

Scanner::State *Scanner::NEQ::operator()(char c) {
    if (c == '=') {
        buf.push_back(c);
        j = search(buf, TD);
        Lexems.push_back(Lex(LEX_NEQ, j));
        return new OK;
    } else {
        cout << "После '!' должно идти '='";
        return new ERR;
    }
}

Scanner::State *Scanner::STR::operator()(char c) {
    if (c != '"') {
        buf.push_back(c);
        return new STR;
    }
    Lexems.push_back(Lex(LEX_STR, buf));
    return new OK;
}


void Scanner::get_lex() {
    int d, j;
    string buf;
    State * temp, *next; // текущее и следующее состояния автомата (ДС)
    for (temp = new H, gc(); !( dynamic_cast<ERR *> (temp)) && !( dynamic_cast<OK *> (temp)); gc()){
        if (end_flag) {
            break;
        }
        next=(*temp)(c);
        delete temp;
        temp=next;
        if (dynamic_cast<OK *> (temp)) {
            break;
        }
    }
    cout << buf << '\n';
    if (dynamic_cast<ERR *> (temp)) {
        cout<< "Error" <<endl;
    }
    delete temp;
}

void Scanner::lex_analyzer() {
    do {
        get_lex();
        buf.clear();
    } while(Scanner::end_flag == false);
}

ostream & operator<< ( ostream &s, Lex l ) {
    string t;
    if ( l.t_lex <= TYPE_STRUCT )
        t = Scanner::TW[l.t_lex];
    else if ( l.t_lex >= LEX_QUOTE && l.t_lex <= LEX_PERCENT )
        t = Scanner::TD[ l.t_lex - LEX_QUOTE ];
    else if ( l.t_lex == LEX_NUM )
        t = "NUMB";
    else if ( l.t_lex == LEX_RNUM) {
        s << "(REAL_NUM," << l.real_lex << ");" << endl;
        return s;
    }
    else if ( l.t_lex == LEX_ID )
        t = TID[l.v_lex].get_name();
    else if( l.t_lex == LEX_STR ){
        s << "(STR," << l.s_lex << ");" << endl;
        return s;
    }
    else{
        cout << l;
        char s[BUFSIZE];
        sprintf(s, " Ошибка печати, недопустимая лексема\n");
        string str = string(s);
        throw str;
    }
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}




int main() {
    system("chcp 65001");

    Scanner S1("C:\\Users\\dan-z\\CLionProjects\\first\\text.txt");
    try {
        S1.lex_analyzer();
        for (auto p: Scanner::Lexems) {
            cout << p;
        }
    }
    catch (string& str) {
        cout << str << endl;;
    }
}
//

