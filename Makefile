all:
	gcc src/gpt-parser.c --static -o gpt-parser

clean:
	rm -fv gpt-parser
