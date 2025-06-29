.PHONY:all
all: tocdo

#.PHONY:test
#test: test.c
#	gcc $^ -o $@ -l curl

.PHONY:tocdo
tocdo: tocdo.c config.c
	gcc -ggdb $^ -o $@
