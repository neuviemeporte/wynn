TEMPLATE = subdirs
SUBDIRS = app db plugins

app.depends = db
