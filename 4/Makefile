all: reader transformer writer program

reader: reader.c
	gcc reader.c -o reader
transformer: transformer.c
	gcc transformer.c -o transformer
writer: writer.c
	gcc writer.c -o writer
program: main.c
	gcc main.c -o program

clean:
	rm reader writer transformer program
