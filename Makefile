.PHONY : test
test:
	ruby ./build.rb test

.PHONY : all
all:
	echo "NOOP"

.PHONY : clean
clean:
	ruby ./build.rb clean