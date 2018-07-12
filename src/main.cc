#include <cstdio>
#include <string>

FILE* in_s = stdin;
FILE* out_s = stdout;

const char TAB = '\t';
char Look;

void GetChar() {
  Look = std::getc(in_s);
}

void Error(const std::string& s) {
  fprintf(out_s, "\nError: %s.\n", s.c_str());
}

void Abort(const std::string& s) {
  Error(s);
  std::abort();
}

void Expected(const std::string& s) {
  Abort(s + " Expected");
}

void Match(char x) {
  if (x == Look)
    GetChar();
  else
    Expected("\'" + std::string(1,x) + "\'");
}

bool IsAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IsDigit(char c) {
  return c >= '0' && c <= '9';
}

char UpCase(char c) {
  if (c >= 'a' && c <= 'z')
    return c-32;
  else
    return c;
}

char GetName() {
  if (!IsAlpha(Look))
    Expected("Name");
  char name = UpCase(Look);
  GetChar();
  return name;
}

char GetNum() {
  if (!IsDigit(Look))
    Expected("Integer");
  char num = Look;
  GetChar();
  return num;
}

void Emit(const std::string& s) {
  fprintf(out_s, "%c%s", TAB, s.c_str());
}

void EmitLn(const std::string& s) {
  Emit(s);
  fprintf(out_s, "\n");
}

void Init() {
  GetChar();
}

int main() {
  Init();
  return 0;
}
