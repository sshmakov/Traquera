######################################################################
# Automatically generated by qmake (2.01a) ?? 28. ??? 23:26:04 2013
######################################################################

TEMPLATE = subdirs

CONFIG += ordered

# Directories
SUBDIRS += \
    tqplugapi \
    tqgui \
    tqcondapi \
    tracker \
    msplans   \
    tqservice  \
    tqaxfactory \
    jira \
    client \
    trindex \
    install

msplans.depends = client
client.depends = tqplugapi


TARGET = traquera
#TRANSLATIONS =
CODECFORTR = UTF-8
CODECFORSRC = UTF-8

#TRANSLATIONS = \
#    jira\redistribute\jira\lang\jira.en.ts \
#    jira\redistribute\jira\lang\jira.ru.ts \
#    tracker\redistribute\tracker\lang\tracker.en.ts \
#    tracker\redistribute\tracker\lang\tracker.ru.ts \
#    client\lang\traquera.en.ts \
#    client\lang\traquera.ru.ts \
