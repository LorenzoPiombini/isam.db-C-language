TARGET = /usr/local/bin/isam.db
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
OBJ_PROD = $(patsubst src/%.c, obj/%_prod.o, $(SRC))
OBJlibht = obj/debug.o  obj/hash_tbl.o
OBJlibf = obj/debug.o  obj/file.o  obj/float_endian.o 
OBJlibs = obj/debug.o  obj/str_op.o
OBJlibr = obj/debug.o  obj/record.o
OBJlibp = obj/debug.o  obj/sort.o obj/parse.o
OBJlibl = obj/debug.o  obj/lock.o
OBJlibbst = obj/debug.o  obj/bst.o obj/str_op.o
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

LIBNAMEht = ht
LIBDIR = /usr/local/lib
INCLUDEDIR = /usr/local/include
SHAREDLIBht = lib$(LIBNAMEht).so

LIBNAMEf = file
LIBDIR = /usr/local/lib
INCLUDEDIR = /usr/local/include
SHAREDLIBf = lib$(LIBNAMEf).so

LIBNAMEs = strOP
LIBDIR = /usr/local/lib
INCLUDEDIR = /usr/local/include
SHAREDLIBs = lib$(LIBNAMEs).so

LIBNAMEr = record
LIBDIR = /usr/local/lib
INCLUDEDIR = /usr/local/include
SHAREDLIBr = lib$(LIBNAMEr).so

LIBNAMEp = parse
LIBDIR = /usr/local/lib
INCLUDEDIR = /usr/local/include
SHAREDLIBp = lib$(LIBNAMEp).so

LIBNAMEl = lock
LIBDIR = /usr/local/lib
INCLUDEDIR = /usr/local/include
SHAREDLIBl = lib$(LIBNAMEl).so

LIBNAMEbst = bst
LIBDIR = /usr/local/lib
INCLUDEDIR = /usr/local/include
SHAREDLIBbst = lib$(LIBNAMEbst).so

SCRIPTS = GET FILE LIST WRITE UPDATE DEL DELa KEYS 

default: $(TARGET)


prod: $(TARGET)_prod

object-dir:
	@if [ ! -d ./obj ]; then\
		echo "creating object directory...";\
		mkdir -p obj ;\
	fi

check-linker-path:
	@if [ ! -f /etc/ld.so.conf.d/customtech.conf ]; then \
		echo "setting linker configuration..." ;\
		echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/customtech.conf >/dev/null ;\
		sudo ldconfig;\
	fi

library:
	sudo gcc -Wall -fPIC -shared -o $(SHAREDLIBht) $(OBJlibht)
	sudo gcc -Wall -fPIC -shared -o $(SHAREDLIBs) $(OBJlibs)
	sudo gcc -Wall -fPIC -shared -o $(SHAREDLIBr) $(OBJlibr)
	sudo gcc -Wall -fPIC -shared -o $(SHAREDLIBf) $(OBJlibf)
	sudo gcc -Wall -fPIC -shared -o $(SHAREDLIBp) $(OBJlibp)
	sudo gcc -Wall -fPIC -shared -o $(SHAREDLIBl) $(OBJlibl)
	sudo gcc -Wall -fPIC -shared -o $(SHAREDLIBbst) $(OBJlibbst)

test:	
	$(TARGET) -nf test -a name:TYPE_STRING:ls:age:TYPE_BYTE:37:addr:TYPE_STRING:"Vattella a Pesca 122":city:TYPE_STRING:"Somerville":zip_code:TYPE_STRING:07921 -k pi90 
	$(TARGET) -a  code:t_s:"man78-g-hus":price:t_f:33.56:discount:TYPE_FLOAT:0.0 -nf item -k ui7
	$(TARGET) -nf test7789 -a  name:TYPE_STRING:Lorenzo:age:TYPE_BYTE:23:addr:TYPE_STRING:somerville_rd_122:city:TYPE_STRING:Bedminster:zip_code:TYPE_STRING:07921 -k jj6
	$(TARGET) -f test -k pi90 -D 0
	$(TARGET) -nf prova -a code:TYPE_STRING:"par45-Y-us":price:TYPE_FLOAT:33.56:discount:TYPE_INT:0.0 -k45rt
	
	$(TARGET) -f item -a price:TYPE_FLOAT:33.56:discount:TYPE_FLOAT:0.0:code:TYPE_STRING:"par45-Y-us" -k ui8
	$(TARGET) -f item -a price:TYPE_FLOAT:67.56:discount:TYPE_FLOAT:0.0:code:TYPE_STRING:"met90-x-us":unit:TYPE_STRING:"each":weight:TYPE_DOUBLE:45.43 -k ui9
	$(TARGET) -nf cmc 
	$(TARGET) -f item -a code:TYPE_STRING:nhy-X-it -k ui10
	$(TARGET) -f item -a code:TYPE_STRING:pio-u-ES:weight:TYPE_DOUBLE:10.9 -k ui11
	$(TARGET) -f item -a code:TYPE_STRING:"par45-Y-us":price:TYPE_FLOAT:33.56:discount:TYPE_INT:0.0 -k ui10

pause:
	@bash -c 'read -p "Press any key to continue..." -n 1 -s'

memory:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -nf test -a name:TYPE_STRING:ls:age:TYPE_BYTE:37:addr:TYPE_STRING:"Vattella a Pesca 122":city:TYPE_STRING:"Somerville":zip_code:TYPE_STRING:07921 -k pi90
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -a code:TYPE_STRING:"man78-g-hus":price:TYPE_FLOAT:33.56:discount:TYPE_FLOAT:0.0 -nf item -k ui7
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -nf test7789 -a name:TYPE_STRING:Lorenzo:age:TYPE_BYTE:23:addr:TYPE_STRING:somerville_rd_122:city:TYPE_STRING:Bedminster:zip_code:TYPE_STRING:07921 -k jj6     
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -f test -D0 -k pi90
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -nf prova  -a code:TYPE_STRING:"par45-Y-us":price:TYPE_FLOAT:33.56:discount:TYPE_INT:0.0 -k45rt
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -f item -a price:TYPE_FLOAT:33.56:discount:TYPE_FLOAT:0.0:code:TYPE_STRING:"par45-Y-us" -k ui8
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -f item -a price:TYPE_FLOAT:67.56:discount:TYPE_FLOAT:0.0:code:TYPE_STRING:"met90-x-us":unit:TYPE_STRING:"each":weight:TYPE_DOUBLE:45.43 -k ui9
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -nf cmc 
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -f item -a code:TYPE_STRING:nhy-X-it -k ui10
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -f item -a code:TYPE_STRING:pio-u-ES:weight:TYPE_DOUBLE:10.9 -k ui11
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -f item -R code:TYPE_STRING:country:t_s:length:t_d:cost:t_s
	$(MAKE) pause
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --undef-value-errors=yes -s $(TARGET) -f item -a code:TYPE_STRING:"par45-Y-us":price:TYPE_FLOAT:33.56:discount:TYPE_INT:0.0 -k ui10
	$(MAKE) pause

clean:
	sudo rm -f $(BINDIR)/GET $(BINDIR)/LIST $(BINDIR)/FILE $(BINDIR)/KEYS $(BINDIR)/WRITE $(BINDIR)/UPDATE $(BINDIR)/DEL $(BINDIR)/DELa
	sudo rm -f $(INCLUDEDIR)/file.h $(INCLUDEDIR)/str_op.h $(INCLUDEDIR)/record.h $(INCLUDEDIR)/parse.h $(INCLUDEDIR)/bst.h $(INCLUDEDIR)/hash_tbl.h $(INCLUDEDIR)/lock.h 
	sudo rm -f $(LIBDIR)/$(SHAREDLIBf) $(LIBDIR)/$(SHAREDLIBs) $(LIBDIR)/$(SHAREDLIBr) $(LIBDIR)/$(SHAREDLIBp) $(LIBDIR)/$(SHAREDLIBht) $(LIBDIR)/$(SHAREDLIBl) $(LIBDIR)/$(SHAREDLIBbst) 
	sudo ldconfig
	rm -f obj/*.o 
	rm -f bin/*
	rm *.so
	sudo rm -f $(TARGET)
	rm *.dat *.inx
	rm *core*
	 
$(TARGET): $(OBJ)
	sudo gcc -o $@ $? -fsanitize=address -fpie -pie -z relro -z now -z noexecstack



obj/%.o : src/%.c
	sudo gcc -O -Wall -g3 -c $< -o $@ -Iinclude -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIC -pie -fsanitize=address
#	sudo gcc -Wall -g3 -c $< -o $@ -Iinclude

$(TARGET)_prod: $(OBJ_PROD)
	sudo gcc -o $@ $? -fpie -pie -z relro -z now -z noexecstack

obj/%_prod.o : src/%.c
	sudo gcc -O -Wall -c $< -o $@ -Iinclude -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIC



$(BINDIR)/GET:
	@if [ !  -f $@ ]; then \
		echo "Creating $@ . . ."; \
		echo "#!/bin/bash" > $@; \
		echo "#Check if both arguments are provided" >> $@; \
		echo "if [ -z \"\$$1\" ] || [ -z \"\$$2\" ]; then" >> $@; \
		echo "echo \"Usage: GET [file name] [record_id]\"" >> $@; \
		echo "exit 1" >> $@; \
		echo "fi" >> $@; \
		echo "" >> $@; \
		echo "$(TARGET) -f \"\$$1\" -k \"\$$2\"" >> $@; \
		chmod +x $@; \
	fi
$(BINDIR)/LIST:
	@if [ !  -f $@ ]; then  \
		echo "Creating $@ . . ."; \
		echo "#!/bin/bash" > $@; \
		echo "#Check if the argument is provided" >> $@; \
		echo "if [ -z \"\$$1\" ]; then" >> $@; \
		echo "echo \"Usage: LIST [file name]\"" >> $@; \
		echo "exit 1" >> $@; \
		echo "fi" >> $@; \
		echo "" >> $@; \
		echo "$(TARGET) -lf \"\$$1\"" >> $@; \
		chmod +x $@; \
	fi

$(BINDIR)/FILE:
	@if [ ! -f $@ ]; then \
		echo "Creating $@ . . ."; \
		echo "#!/bin/bash" > $@; \
		echo "if [ -z \"\$$1\" ] || [ -z \"\$$2\" ]; then" >> $@; \
		echo "echo \"Usage: FILE [file name] [fields name and type]\"" >> $@; \
		echo "exit 1" >> $@; \
		echo "fi" >> $@; \
		echo "" >> $@; \
		echo "if [ -e \"\$$1.dat\" ]; then" >> $@; \
		echo "	 $(TARGET) -f \"\$$1\" -R \"\$$2\"" >> $@; \
		echo "else" >> $@; \
		echo "$(TARGET) -nf \"\$$1\" -R \"\$$2\"" >> $@; \
		echo "fi" >> $@; \
		chmod +x $@; \
	fi

$(BINDIR)/WRITE:
	@if [ ! -f $@ ]; then \
		echo "Creating $@ . . ."; \
		echo "#!/bin/bash" > $@; \
		echo "if [ -z \"\$$1\" ] || [ -z \"\$$2\" ] || [ -z \"\$$3\" ]; then" >> $@; \
		echo "echo \"Usage: WRITE [file name] [fields name and type] [key]\"" >> $@; \
		echo "exit 1" >> $@; \
		echo "fi" >> $@; \
		echo "$(TARGET) -f \"\$$1\" -a \"\$$2\" -k \"\$$3\" " >> $@; \
		chmod +x $@; \
	fi

$(BINDIR)/UPDATE:
	@if [ ! -f $@ ]; then \
		echo "Creating $@ . . ."; \
		echo "#!/bin/bash" > $@; \
		echo "if [ -z \"\$$1\" ] || [ -z \"\$$2\" ] || [ -z \"\$$3\" ]; then" >> $@; \
		echo "echo \"Usage: UPDATE [file name] [fields name and type] [key]\"" >> $@; \
		echo "exit 1" >> $@; \
		echo "fi" >> $@; \
		echo "$(TARGET) -uf \"\$$1\" -a \"\$$2\" -k \"\$$3\" " >> $@; \
		chmod +x $@; \
	fi
$(BINDIR)/KEYS:
	@if [ ! -f $@ ]; then \
		echo "Creating $@ . . ."; \
		echo "#!/bin/bash" > $@; \
		echo "if [ -z \"\$$1\" ]; then" >> $@; \
		echo "echo \"Usage: KEYS [file name]\"" >> $@; \
		echo "exit 1" >> $@; \
		echo "fi" >> $@; \
		echo "" >> $@; \
		echo "value=\"\$$2\"" >> $@; \
		echo "if [ -z \"\$$2\" ]; then" >> $@; \
		echo "value=0" >> $@; \
		echo "fi" >> $@; \
		echo "$(TARGET) -f \"\$$1\" -x \"\$$value\"" >> $@; \
		chmod +x $@; \
	fi
$(BINDIR)/DEL:
	@if [ ! -f $@ ]; then \
		echo "Creating $@ . . ."; \
		echo "#!/bin/bash" > $@; \
		echo "if [ -z \"\$$1\" ] || [ -z \"\$$2\" ]; then" >> $@; \
		echo "echo \"Usage: DEL [file name] [key] [index_number]\"" >> $@; \
		echo "echo \"index number is not mandatory\"" >> $@; \
		echo "echo \"if index_number is not specified index 0 is used\"" >> $@; \
		echo "exit 1" >> $@; \
		echo "fi" >> $@; \
		echo "" >> $@; \
		echo "value=\"\$$3\"" >> $@; \
		echo "if [ -z \"\$$3\" ]; then" >> $@; \
		echo "value=0" >> $@; \
		echo "fi" >> $@; \
		echo "" >> $@; \
		echo "$(TARGET) -f \"\$$1\" -k \"\$$2\" -D \"\$$value\"" >> $@; \
		chmod +x $@; \
	fi

$(BINDIR)/DELa:
	@if [ ! -f $@ ]; then \
		echo "Creating $@ . . ."; \
		echo "#!/bin/bash" > $@; \
		echo "if [ -z \"\$$1\" ]; then" >> $@; \
		echo "echo \"Usage: DELa [file name]\"" >> $@; \
   		echo "exit 1" >> $@; \
   		echo "fi" >> $@; \
		echo "$(TARGET) -f \"\$$1\" -D 0 -o all" >> $@; \
		chmod +x $@; \
	fi

install: $(TARGET) $(BINDIR)/GET $(BINDIR)/LIST $(BINDIR)/FILE $(BINDIR)/KEYS $(BINDIR)/WRITE $(BINDIR)/UPDATE $(BINDIR)/DEL $(BINDIR)/DELa check-linker-path
	install -d $(INCLUDEDIR)
	install -m 644 include/bst.h include/hash_tbl.h include/file.h include/str_op.h include/record.h include/parse.h include/lock.h $(INCLUDEDIR)/
	install -m 755 $(SHAREDLIBht) $(LIBDIR)
	install -m 755 $(SHAREDLIBf) $(LIBDIR)
	install -m 755 $(SHAREDLIBs) $(LIBDIR)
	install -m 755 $(SHAREDLIBr) $(LIBDIR)
	install -m 755 $(SHAREDLIBp) $(LIBDIR) 
	install -m 755 $(SHAREDLIBl) $(LIBDIR)
	install -m 755 $(SHAREDLIBbst) $(LIBDIR)
	ldconfig
	

build: object-dir default library install 
.PHONY: default test memory clean install library check-linker-path object-dir
