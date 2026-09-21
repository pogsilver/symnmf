from setuptools import Extension, setup
module = Extension("symnmf_module", sources=['symnmf.c', 'symnmfmodule.c'])
setup(name='symnmf_module',
      version='1.0',
      description="Python wrapper for the symnmf c functions",
      ext_modules=[module])