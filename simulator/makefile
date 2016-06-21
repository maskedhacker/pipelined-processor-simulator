TARGET=pipeline
$(TARGET): main.o r_func.o i_func.o js_func.o get_images.o inst_ana.o
	gcc main.o r_func.o i_func.o js_func.o get_images.o inst_ana.o -o $(TARGET)
main.o: main.c get_images.h
	gcc -c main.c
r_func.o: r_func.c get_images.h
	gcc -c r_func.c
i_func.o: i_func.c get_images.h
	gcc -c i_func.c
js_func.o: js_func.c get_images.h
	gcc -c js_func.c
get_images.o: get_images.c get_images.h
	gcc -c get_images.c
inst_ana.o: inst_ana.c get_images.h
	gcc -c inst_ana.c
clean: 
	rm -r $(TARGET) *.o *.bin *rpt *.exe *.out *.txt

