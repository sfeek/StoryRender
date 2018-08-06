all:
	$(CC) -Wall -Werror story_render.c simplestring.c tokenizer.c iolibrary.c -O2 -o story_render 

clean:
	$(RM) -rf story_render
