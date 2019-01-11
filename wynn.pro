TEMPLATE = subdirs
SUBDIRS = libixplog libixpdict app dicttool test

app.depends = libixplog libixpdict
libixpdict.depends = libixplog
dicttool.depends = libixplog libixpdict
test.depends = libixpdict app
