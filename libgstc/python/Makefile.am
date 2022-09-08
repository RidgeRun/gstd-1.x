all-local:
	(cd $(srcdir); python3 setup.py build)

install-exec-local:
	python3 $(srcdir)/setup.py install --prefix $(DESTDIR)$(prefix)/local

clean-generic:
	rm -rf $(srcdir)/build
