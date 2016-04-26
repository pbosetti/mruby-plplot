.PHONY : test
test:
	ruby ./run_test.rb test

.PHONY : all
all:
	echo "NOOP"

.PHONY : clean
clean:
	ruby ./build.rb clean