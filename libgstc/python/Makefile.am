all-local:
	(cd $(srcdir); python3 setup.py build --version=$(PACKAGE_VERSION))

install-exec-local:
	python3 $(srcdir)/setup.py install --prefix $(DESTDIR)$(prefix)/local --version=$(PACKAGE_VERSION)

clean-generic:
	rm -rf $(srcdir)/build
