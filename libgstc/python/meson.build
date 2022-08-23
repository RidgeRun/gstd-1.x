pymod = import('python')
python = pymod.find_installation(
  get_option('with-python-version'),
  required : get_option('enable-python'),
  disabler : true
)

pythonver = python.language_version()
if pythonver.version_compare('<3.5')
  error('Python @0@ is not supported anymore, please port your code to python3.5 or newer.'.format(python.language_version()))
endif

pygstc_src_files = [
  'pygstc/__init__.py',
  'pygstc/gstc.py',
  'pygstc/gstcerror.py',
  'pygstc/logger.py',
  'pygstc/tcp.py'
]

python.install_sources(
  pygstc_src_files,
  subdir : 'pygstc',
  pure : true
)
