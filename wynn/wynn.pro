TEMPLATE = subdirs
SUBDIRS = app db plugins

app.depends = db

# 
# CONFIG(test) {
#     SUBDIRS += test
#     test.depends = libixplog libixpdict
# }

