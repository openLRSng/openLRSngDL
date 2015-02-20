void __putc(char c);

// String
void printLf()
{
  __putc('\r');
  __putc('\n');
}

#define printC(c) __putc((c))

void _printStr(const char *s, bool lf)
{
  while (*s) {
    __putc(*(s++));
  }
  if (lf) {
    printLf();
  }
}

#define printStr(s) _printStr((s),0)
#define printStrLn(s) _printStr((s),1)

// Numbers: unsigned long
void _printUL(uint32_t n, bool lf)
{
  char b[11];
  uint32_t i=10;
  b[10]=0;
  do {
    b[--i] = (n % 10) + '0';
    n = n / 10;
  } while (n);
  _printStr(b + i, lf);
}

#define printUL(n) _printUL((n),0)
#define printULLn(n) _printUL((n),1)

