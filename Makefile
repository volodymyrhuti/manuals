merge: clean
	@echo Merging mannuals
	@find -maxdepth 1 -type f ! -path '*/\.*'                     \
		! -name 'mannual.txt' ! -name 'Makefile' ! -name 'README' \
		-exec cat {} \; >> mannual.txt

clean:
	-@ rm mannual.txt 2>/dev/null

.PHONY: clean merge
