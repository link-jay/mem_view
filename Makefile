all: mem_view test

mem_view:mem_view.c
	cc mem_view.c -o mem_view.out -g
test:test/test.c
	cc test/test.c -o test/test.out -g
