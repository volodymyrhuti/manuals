# NOTE: mannual files do not have extension, they are always txt files

PRIVATE=private.tar.gz
PRIVATE_CPT=$(PRIVATE).cpt

merge: clean
	@echo Merging mannuals
	@git ls-files | grep -vE 'Makefile|*.cpt|gitignore|README' | \
		xargs -I{} cat {} > mannual.txt

clean:
	-@ rm mannual.txt $(PRIVATE) $(PRIVATE_CPT) 2>/dev/null

encrypt:
	tar acvf $(PRIVATE) $$(git ls-files -o -x mannuals.txt -x $(PRIVATE))
	ccencrypt $(PRIVATE)
	git add $(PRIVATE_CPT)

decrypt:
	ccdecrypt $(PRIVATE_CPT)

.PHONY: clean merge
