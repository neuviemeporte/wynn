TEMPLATE = subdirs
SUBDIRS = libixplog libixpdict wynn dicttool test

wynn.depends = libixplog libixpdict
libixpdict.depends = libixplog
dicttool.depends = libixplog libixpdict
test.depends = libixpdict wynn
