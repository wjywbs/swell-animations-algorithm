.PHONY: run clean
run:
	./hermite.py | awk '{print $$1$$2","$$3$$4","}' > output
	cat top output bottom > foo
	python foo
clean: 
	$(RM) foo output
