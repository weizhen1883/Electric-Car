TOOLS_CFLAGS	:= -Wstrict-prototypes -Wshadow -Wpointer-arith -Wcast-qual \
		   -Wcast-align -Wwrite-strings -Wnested-externs -Winline \
		   -W -Wundef -Wmissing-prototypes
#
# Programs
#
all:	ECar GoForwordwithSensor

ECar:  ECar.o gpio-utils.o
	$(CC) $(LDFLAGS) -o $@ $^

GoForwordwithSensor:  GoForwordwithSensor.o gpio-utils.o
	$(CC) $(LDFLAGS) -o $@ $^

#
# Objects
#

%.o: %.c
	$(CC) $(CFLAGS) $(TOOLS_CFLAGS) -c $< -o $@

clean:
	rm gpio-utils.o ECar.o GoForwordwithSensor.o
