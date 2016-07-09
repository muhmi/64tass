CC = gcc
OBJ = 64tass.o opcodes.o misc.o avl.o my_getopt.o eval.o error.o section.o encoding.o ternary.o file.o values.o variables.o mem.o isnprintf.o macro.o obj.o floatobj.o addressobj.o codeobj.o strobj.o listobj.o boolobj.o bytesobj.o intobj.o bitsobj.o functionobj.o instruction.o unicode.o unicodedata.o listing.o registerobj.o dictobj.o namespaceobj.o operobj.o gapobj.o typeobj.o noneobj.o longjump.o wctype.o wchar.o math.o arguments.o optimizer.o opt_bit.o
LDLIBS = -lm
LANG = C
REVISION := $(shell svnversion | grep "^[1-9]" || echo "1229?")
CFLAGS = -O2 -W -Wall -Wextra -Wwrite-strings -Wshadow -fstrict-aliasing -DREVISION="\"$(REVISION)\"" -Wstrict-aliasing=2 -Werror=missing-prototypes
LDFLAGS = -g
CFLAGS += $(LDFLAGS)
TARGET = 64tass
PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

.SILENT:

all: $(TARGET) README

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $@

README: README.html
	-sed -e 's/&larr;/<-/g;s/&hellip;/.../g;s/&lowast;/*/g;s/&minus;/-/g;s/&ndash;/-/g;' README.html | w3m -T text/html -dump -no-graph | sed -e 's/\s\+$$//' >README

64tass.o: 64tass.c 64tass.h stdbool.h inttypes.h wait_e.h wchar.h error.h \
 errors_e.h avl.h obj.h opcodes.h misc.h eval.h values.h section.h mem.h \
 encoding.h file.h variables.h macro.h instruction.h unicode.h \
 unicodedata.h listing.h optimizer.h listobj.h codeobj.h strobj.h \
 floatobj.h addressobj.h boolobj.h bytesobj.h intobj.h bitsobj.h \
 functionobj.h namespaceobj.h operobj.h gapobj.h typeobj.h noneobj.h \
 registerobj.h arguments.h
addressobj.o: addressobj.c addressobj.h obj.h stdbool.h inttypes.h \
 values.h error.h errors_e.h avl.h eval.h variables.h arguments.h \
 boolobj.h strobj.h intobj.h operobj.h typeobj.h noneobj.h
arguments.o: arguments.c arguments.h stdbool.h inttypes.h 64tass.h \
 wait_e.h opcodes.h my_getopt.h file.h avl.h error.h errors_e.h obj.h \
 unicode.h unicodedata.h
avl.o: avl.c avl.h stdbool.h inttypes.h
bitsobj.o: bitsobj.c bitsobj.h obj.h stdbool.h inttypes.h eval.h \
 variables.h unicode.h unicodedata.h encoding.h avl.h errors_e.h error.h \
 arguments.h codeobj.h values.h boolobj.h floatobj.h strobj.h bytesobj.h \
 intobj.h listobj.h operobj.h typeobj.h noneobj.h
boolobj.o: boolobj.c boolobj.h obj.h stdbool.h inttypes.h values.h eval.h \
 error.h errors_e.h avl.h variables.h arguments.h floatobj.h strobj.h \
 bytesobj.h bitsobj.h intobj.h operobj.h typeobj.h
bytesobj.o: bytesobj.c bytesobj.h obj.h stdbool.h inttypes.h eval.h \
 unicode.h unicodedata.h encoding.h avl.h errors_e.h variables.h \
 arguments.h boolobj.h values.h floatobj.h codeobj.h intobj.h strobj.h \
 bitsobj.h listobj.h operobj.h typeobj.h noneobj.h error.h
codeobj.o: codeobj.c codeobj.h obj.h stdbool.h inttypes.h values.h eval.h \
 mem.h 64tass.h wait_e.h section.h avl.h variables.h error.h errors_e.h \
 arguments.h boolobj.h floatobj.h namespaceobj.h listobj.h intobj.h \
 bitsobj.h bytesobj.h operobj.h gapobj.h typeobj.h noneobj.h
dictobj.o: dictobj.c dictobj.h obj.h stdbool.h inttypes.h values.h avl.h \
 eval.h error.h errors_e.h variables.h intobj.h listobj.h strobj.h \
 boolobj.h operobj.h typeobj.h noneobj.h
encoding.o: encoding.c encoding.h avl.h stdbool.h inttypes.h errors_e.h \
 error.h obj.h ternary.h misc.h unicode.h unicodedata.h strobj.h \
 bytesobj.h typeobj.h values.h
error.o: error.c error.h stdbool.h inttypes.h errors_e.h avl.h obj.h \
 wchar.h wctype.h misc.h file.h variables.h 64tass.h wait_e.h macro.h \
 strobj.h unicode.h unicodedata.h addressobj.h values.h registerobj.h \
 namespaceobj.h operobj.h typeobj.h eval.h arguments.h
eval.o: eval.c eval.h obj.h stdbool.h inttypes.h math.h file.h avl.h \
 section.h mem.h encoding.h errors_e.h macro.h wait_e.h variables.h \
 64tass.h misc.h unicode.h unicodedata.h listing.h error.h values.h \
 arguments.h optimizer.h floatobj.h boolobj.h intobj.h bitsobj.h strobj.h \
 codeobj.h bytesobj.h addressobj.h listobj.h dictobj.h registerobj.h \
 namespaceobj.h operobj.h gapobj.h typeobj.h noneobj.h
file.o: file.c file.h stdbool.h inttypes.h avl.h wchar.h 64tass.h \
 wait_e.h unicode.h unicodedata.h error.h errors_e.h obj.h strobj.h \
 arguments.h
floatobj.o: floatobj.c floatobj.h obj.h stdbool.h inttypes.h values.h \
 error.h errors_e.h avl.h eval.h variables.h arguments.h boolobj.h \
 codeobj.h strobj.h bytesobj.h intobj.h bitsobj.h operobj.h typeobj.h \
 noneobj.h
functionobj.o: functionobj.c functionobj.h obj.h stdbool.h inttypes.h \
 values.h math.h isnprintf.h eval.h misc.h variables.h error.h errors_e.h \
 avl.h floatobj.h strobj.h listobj.h intobj.h boolobj.h operobj.h \
 typeobj.h noneobj.h
gapobj.o: gapobj.c gapobj.h obj.h stdbool.h inttypes.h error.h errors_e.h \
 avl.h eval.h variables.h strobj.h operobj.h intobj.h values.h boolobj.h \
 typeobj.h noneobj.h
instruction.o: instruction.c instruction.h stdbool.h inttypes.h opcodes.h \
 obj.h 64tass.h wait_e.h section.h avl.h mem.h file.h listing.h error.h \
 errors_e.h addressobj.h values.h listobj.h registerobj.h codeobj.h \
 typeobj.h noneobj.h longjump.h arguments.h optimizer.h
intobj.o: intobj.c intobj.h obj.h stdbool.h inttypes.h values.h unicode.h \
 unicodedata.h encoding.h avl.h errors_e.h error.h eval.h variables.h \
 arguments.h boolobj.h floatobj.h codeobj.h strobj.h bytesobj.h bitsobj.h \
 operobj.h typeobj.h noneobj.h
isnprintf.o: isnprintf.c isnprintf.h inttypes.h unicode.h unicodedata.h \
 eval.h obj.h stdbool.h floatobj.h values.h strobj.h intobj.h typeobj.h \
 noneobj.h error.h errors_e.h avl.h
listing.o: listing.c listing.h inttypes.h stdbool.h file.h avl.h error.h \
 errors_e.h obj.h 64tass.h wait_e.h opcodes.h unicode.h unicodedata.h \
 section.h mem.h instruction.h values.h arguments.h
listobj.o: listobj.c listobj.h obj.h stdbool.h inttypes.h values.h eval.h \
 variables.h error.h errors_e.h avl.h arguments.h boolobj.h codeobj.h \
 strobj.h intobj.h operobj.h typeobj.h noneobj.h
longjump.o: longjump.c longjump.h avl.h stdbool.h inttypes.h section.h \
 mem.h error.h errors_e.h obj.h
macro.o: macro.c macro.h obj.h stdbool.h inttypes.h wait_e.h misc.h \
 file.h avl.h eval.h values.h section.h mem.h variables.h 64tass.h \
 listing.h error.h errors_e.h listobj.h typeobj.h noneobj.h \
 namespaceobj.h arguments.h optimizer.h
math.o: math.c math.h inttypes.h
mem.o: mem.c mem.h stdbool.h inttypes.h error.h errors_e.h avl.h obj.h \
 file.h 64tass.h wait_e.h listing.h arguments.h
misc.o: misc.c misc.h stdbool.h inttypes.h section.h avl.h mem.h \
 longjump.h encoding.h errors_e.h file.h eval.h obj.h variables.h \
 ternary.h unicode.h unicodedata.h error.h values.h arguments.h opt_bit.h \
 namespaceobj.h
my_getopt.o: my_getopt.c my_getopt.h stdbool.h unicode.h inttypes.h \
 unicodedata.h
namespaceobj.o: namespaceobj.c namespaceobj.h obj.h stdbool.h inttypes.h \
 avl.h variables.h eval.h intobj.h values.h listobj.h error.h errors_e.h \
 strobj.h operobj.h typeobj.h noneobj.h
noneobj.o: noneobj.c noneobj.h obj.h stdbool.h inttypes.h error.h \
 errors_e.h avl.h eval.h typeobj.h values.h
obj.o: obj.c obj.h stdbool.h inttypes.h variables.h misc.h eval.h error.h \
 errors_e.h avl.h values.h boolobj.h floatobj.h strobj.h macro.h wait_e.h \
 intobj.h listobj.h namespaceobj.h addressobj.h codeobj.h registerobj.h \
 bytesobj.h bitsobj.h functionobj.h dictobj.h operobj.h gapobj.h \
 typeobj.h noneobj.h
opcodes.o: opcodes.c opcodes.h inttypes.h
operobj.o: operobj.c operobj.h obj.h stdbool.h inttypes.h strobj.h \
 typeobj.h
opt_bit.o: opt_bit.c opt_bit.h inttypes.h stdbool.h error.h errors_e.h \
 avl.h obj.h
optimizer.o: optimizer.c optimizer.h inttypes.h stdbool.h error.h \
 errors_e.h avl.h obj.h section.h mem.h opcodes.h opt_bit.h
registerobj.o: registerobj.c registerobj.h obj.h stdbool.h inttypes.h \
 values.h error.h errors_e.h avl.h eval.h variables.h boolobj.h strobj.h \
 intobj.h operobj.h typeobj.h noneobj.h
section.o: section.c section.h avl.h stdbool.h inttypes.h mem.h unicode.h \
 unicodedata.h error.h errors_e.h obj.h misc.h 64tass.h wait_e.h values.h \
 intobj.h longjump.h optimizer.h
strobj.o: strobj.c strobj.h obj.h stdbool.h inttypes.h eval.h unicode.h \
 unicodedata.h error.h errors_e.h avl.h variables.h arguments.h boolobj.h \
 values.h floatobj.h bytesobj.h intobj.h bitsobj.h listobj.h operobj.h \
 typeobj.h noneobj.h
ternary.o: ternary.c ternary.h stdbool.h inttypes.h unicode.h \
 unicodedata.h error.h errors_e.h avl.h obj.h
typeobj.o: typeobj.c typeobj.h obj.h stdbool.h inttypes.h variables.h \
 eval.h error.h errors_e.h avl.h strobj.h operobj.h intobj.h values.h \
 boolobj.h listobj.h noneobj.h
unicodedata.o: unicodedata.c unicodedata.h inttypes.h
unicode.o: unicode.c unicode.h inttypes.h unicodedata.h wchar.h wctype.h \
 error.h stdbool.h errors_e.h avl.h obj.h
values.o: values.c values.h inttypes.h obj.h stdbool.h unicode.h \
 unicodedata.h error.h errors_e.h avl.h strobj.h typeobj.h noneobj.h \
 variables.h
variables.o: variables.c variables.h stdbool.h inttypes.h obj.h unicode.h \
 unicodedata.h misc.h 64tass.h wait_e.h file.h avl.h boolobj.h values.h \
 floatobj.h error.h errors_e.h namespaceobj.h strobj.h codeobj.h \
 registerobj.h functionobj.h listobj.h intobj.h bytesobj.h bitsobj.h \
 dictobj.h addressobj.h gapobj.h typeobj.h noneobj.h arguments.h
wchar.o: wchar.c wchar.h
wctype.o: wctype.c wctype.h

.PHONY: all clean distclean install install-strip uninstall

clean:
	-rm -f $(OBJ)

distclean: clean
	-rm -f $(TARGET)

install: $(TARGET)
	install -D $(TARGET) $(BINDIR)/$(TARGET)

install-strip: $(TARGET)
	install -D -s $(TARGET) $(BINDIR)/$(TARGET)

uninstall:
	-rm $(BINDIR)/$(TARGET)
