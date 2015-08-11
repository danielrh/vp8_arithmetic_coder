
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

aritest:	test_arithmetic_coding.cpp bitwriter.cpp bitreader.cpp  bitwriter.h bitreader.h vpx_config.h
	gcc -o aritest bitwriter.cpp bitreader.cpp test_arithmetic_coding.cpp