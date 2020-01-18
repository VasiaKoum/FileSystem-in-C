obj = begin_cfs
all: $(obj)
$(obj): %: %.c
	$(CC) $(CFLAGS) -o $@ commands.c $< 
run:
	./begin_cfs
val:
	valgrind --trace-children=yes -v --leak-check=full --show-leak-kinds=all ./begin_cfs
clean:
	rm -f begin_cfs
