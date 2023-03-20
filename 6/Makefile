all: io_manipulator transformer program

transformer: transformer.c
	gcc transformer.c -o transformer
io_manipulator: io_manipulator.c 
	gcc io_manipulator.c -o io_manipulator
program: main.c
	gcc main.c -o program

clean:
	rm io_manipulator transformer program
