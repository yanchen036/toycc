#include <cstdio>
#include <string>

FILE *in_s = stdin;
FILE *out_s = stdout;

const char TAB = '\t';
const char CR = '\n';
char Look;
int Lcount = 0;

void GetChar() { Look = std::getc(in_s); }

void Error(const std::string &s) {
  fprintf(out_s, "\nError: %s.\n", s.c_str());
}

void Abort(const std::string &s) {
  Error(s);
  std::abort();
}

std::string char2str(char c) { return std::string(1, c); }

void Expected(const std::string &s) { Abort(s + " Expected"); }

void Match(char x) {
  if (x == Look) {
    GetChar();
  } else {
    Expected("\'" + char2str(x) + "\'");
  }
}

bool IsAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IsDigit(char c) { return c >= '0' && c <= '9'; }

bool IsAddop(char c) { return c == '+' or c == '-'; }

char UpCase(char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 32;
  } else {
    return c;
  }
}

char GetName() {
  if (!IsAlpha(Look)) {
    Expected("Name");
  }
  char name = UpCase(Look);
  GetChar();
  return name;
}

char GetNum() {
  if (!IsDigit(Look)) {
    Expected("Integer");
  }
  char num = Look;
  GetChar();
  return num;
}

void Emit(const std::string &s) { fprintf(out_s, "%c%s", TAB, s.c_str()); }

void EmitLn(const std::string &s) {
  Emit(s);
  fprintf(out_s, "\n");
}

void Init() { GetChar(); }

// expression
void Expression();

void Ident() {
  char Name = GetName();
  if (Look == '(') {
    Match('(');
    Match(')');
    EmitLn("BSR " + char2str(Name));
  } else {
    EmitLn("MOVE " + char2str(GetName()) + "(PC),D0");
  }
}

void Factor() {
  if (Look == '(') {
    Match('(');
    Expression();
    Match(')');
  } else if (IsAlpha(Look)) {
    Ident();
  } else {
    EmitLn("MOVE #" + char2str(GetNum()) + ",D0");
  }
}

void Multiply() {
  Match('*');
  Factor();
  EmitLn("MULS (SP)+,D0");
}

void Divide() {
  Match('/');
  Factor();
  EmitLn("MOVE (SP)+,D1");
  EmitLn("DIVS D1,D0");
}

void Term() {
  Factor();
  while (Look == '*' || Look == '/') {
    EmitLn("MOVE D0, -(SP)");
    switch (Look) {
    case '*':
      Multiply();
      break;
    case '/':
      Divide();
      break;
    default:
      Expected("Mulop");
    }
  }
}

void Add() {
  Match('+');
  Term();
  EmitLn("ADD (SP)+,D0");
}

void Subtract() {
  Match('-');
  Term();
  EmitLn("SUB (SP)+,D0");
  EmitLn("NEG D0");
}

void Expression() {
  if (IsAddop(Look)) {
    EmitLn("CLR D0");
  } else {
    Term();
  }
  while (Look == '+' || Look == '-') {
    EmitLn("MOVE D0,-(SP)");
    switch (Look) {
    case '+':
      Add();
      break;
    case '-':
      Subtract();
      break;
    default:
      Expected("Addop");
    }
  }
}

void Assignment() {
  char Name = GetName();
  Match('=');
  Expression();
  EmitLn("LEA " + char2str(Name) + "(PC),A0");
  EmitLn("MOVE D0,(A0)");
}

// control flow
std::string NewLable() {
  std::string s = "L" + std::to_string(Lcount);
  Lcount++;
  return s;
}

void PostLabel(std::string L) { fprintf(out_s, "%s:", L.c_str()); }

void Other() {EmitLn(char2str(GetName())); }

void Block();

void Condition() { EmitLn("<condition>"); }

void DoIf() {
  Match('i');
  std::string L1 = NewLable();
  std::string L2 = L1;
  Condition();
  EmitLn("BEQ " + L1);
  Block();
  if (Look == 'l') {
    Match('l');
    L2 = NewLable();
    EmitLn("BRA " + L2);
    PostLabel(L1);
    Block();
  }
  Match('e');
  PostLabel(L2);
}

void Block() {
  while (Look != 'e' && Look != 'l') {
    switch (Look) {
    case 'i':
      DoIf();
      break;
    default:
      Other();
      break;
    }
  }
}

void DoProgram() {
  Block();
  if (Look != 'e') {
    Expected("End");
  }
  EmitLn("END");
}

int main() {
  Init();
  DoProgram();
  return 0;
}
