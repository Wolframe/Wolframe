Some helper scripts:

wolfwizard.simpleform:
	XML Schema definition for the output of wolfwizard
wolfwizard.lua:
	Some lua procedures (lua command handler interface) for transforming the output of wolfwizard
	1) function filterRequests() :Transforms the form definitions into the format of request answer definitions of the Qt client as defined in project Wolframe/qtClient
		Example call:
		wolfwizard -c <wolframe config file> | wolfilter -m mod_filter_textwolf -m mod_filter_line -m mod_ddlcompiler_simpleform -m mod_command_lua -e xml -x ./wolfwizard.lua -p ./wolfwizard.simpleform filterRequests

