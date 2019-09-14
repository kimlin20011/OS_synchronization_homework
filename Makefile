# To run, enter make or make all

file = sleepingTA
RM   = rm -f

defult: all

all: clean ST run

ST:
	gcc  -lpthread -o $(file) $(file).c

run: ${file}
	./${file} 

clean:
	$(RM) $(file)





