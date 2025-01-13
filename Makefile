#CFLAGS = -O3
transform_image: io.o transfo.o cycles.o
	$(CC) -o $@ $^
io.o: transfo.h cycles.h

# Variables
IMAGES = $(shell seq 1 10)
DATA_DIR = ./data
REF_DIR = ./data/references

# Test target
test:
	@echo "Running tests..."
	@all_correct=1; \
	for i in $(IMAGES); do \
	  if ! diff -q $(DATA_DIR)/image$${i}_t.pgm $(REF_DIR)/res$${i}.pgm >/dev/null; then \
	    echo "Difference found in image$${i}"; \
	    all_correct=0; \
	  fi; \
	done; \
	if [ $$all_correct -eq 1 ]; then \
	  echo "All tests successful"; \
	fi

# Clean target 
clean:
	rm -f *.o transform_image
	rm -fr cmake-build-debug
