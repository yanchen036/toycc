#include <cstdio>
#include <string>

FILE *in_s = stdin;
FILE *out_s = stdout;

const char TAB = '\t';
const char CR = '\n';
char Look;
int Lcount;

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

void Other() { EmitLn(char2str(GetName())); }

void Block(std::string L);

void Condition() { EmitLn("<condition>"); }

void DoIf(std::string L) {
  Match('i');
  std::string L1 = NewLable();
  std::string L2 = L1;
  Condition();
  EmitLn("BEQ " + L1);
  Block(L);
  if (Look == 'l') {
    Match('l');
    L2 = NewLable();
    EmitLn("BRA " + L2);
    PostLabel(L1);
    Block(L);
  }
  Match('e');
  PostLabel(L2);
}

void DoWhile() {
  std::string L1, L2;
  Match('w');
  L1 = NewLable();
  L2 = NewLable();
  PostLabel(L1);
  Condition();
  EmitLn("BEQ " + L2);
  Block(L2);
  Match('e');
  EmitLn("BRA " + L1);
  PostLabel(L2);
}

void DoLoop() {
  std::string L1, L2;
  Match('p');
  L1 = NewLable();
  L2 = NewLable();
  PostLabel(L1);
  Block(L2);
  Match('e');
  EmitLn("BRA " + L1);
  PostLabel(L2);
}

void DoRepeat() {
  std::string L;
  Match('r');
  L = NewLable();
  PostLabel(L);
  Block(L);
  Match('u');
  Condition();
  EmitLn("BEQ " + L);
}

void ExpressionTMP() { EmitLn("<expr>"); }

void DoFor() {
  std::string L1, L2;
  char Name;
  Match('f');
  L1 = NewLable();
  L2 = NewLable();
  Name = GetName();
  Match('=');
  ExpressionTMP();
  EmitLn("SUBQ #1,D0");
  EmitLn("LEA " + char2str(Name) + "(PC),A0");
  EmitLn("MOVE D0,(A0)");
  ExpressionTMP();
  EmitLn("MOVE D0,-(SP)");
  PostLabel(L1);
  EmitLn("LEA " + char2str(Name) + "(PC),A0");
  EmitLn("MOVE (A0),D0");
  EmitLn("ADDQ #1,D0");
  EmitLn("MOVE D0,(A0)");
  EmitLn("CMP (SP),D0");
  EmitLn("BGT " + L2);
  Block(L2);
  Match('e');
  EmitLn("BRA " + L1);
  PostLabel(L2);
  EmitLn("ADDQ #2,SP");
}

void DoDo() {
  std::string L1, L2;
  Match('d');
  L1 = NewLable();
  L2 = NewLable();
  ExpressionTMP();
  EmitLn("SUBQ #1,D0");
  PostLabel(L1);
  EmitLn("MOVE D0,-(SP)");
  Block(L2);
  EmitLn("MOVE (SP)+,D0");
  EmitLn("DBRA D0," + L1);
  EmitLn("SUBQ #2,SP");
  PostLabel(L2);
  EmitLn("ADDQ #2,SP");
}

void DoBreak(std::string L) {
  Match('b');
  if (L != "") {
    EmitLn("BRA " + L);
  } else {
    Abort("No loop to break from");
  }
}

void Block(std::string L) {
  while (Look != 'e' && Look != 'l' && Look != 'u' && Look != '=') {
    switch (Look) {
    case 'i':
      DoIf(L);
      break;
    case 'w':
      DoWhile();
      break;
    case 'p':
      DoLoop();
      break;
    case 'r':
      DoRepeat();
      break;
    case 'f':
      DoFor();
      break;
    case 'd':
      DoDo();
      break;
    case 'b':
      DoBreak(L);
      break;
    default:
      Other();
      break;
    }
  }
}

void DoProgram() {
  Block("");
  if (Look != 'e') {
    Expected("End");
  }
  EmitLn("END");
}

void Init() {
  Lcount = 0;
  GetChar();
}

int main() {
  Init();
  DoProgram();
  return 0;
}
