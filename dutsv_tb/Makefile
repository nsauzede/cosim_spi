.PHONY: all clean view
all clean view:
	for d in `find -mindepth 1 -maxdepth 1 -type d`; do \
	( cd $$d && $(MAKE) $@ || exit 1) || exit 1; \
	done
