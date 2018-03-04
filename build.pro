TEMPLATE = subdirs
SUBDIRS = libixplog libixpdict wynn dicttool

wynn.depends = libixplog libixpdict
libixpdict.depends = libixplog
dicttool.depends = libixplog libixpdict
