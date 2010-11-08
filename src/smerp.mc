;// messages file for SMERP messages

;// language codes for translated messages
;// (409 ist the windows locale for en-US)
;// (see http://msdn.microsoft.com/en-us/library/ms776260.aspx)
LanguageNames = (
	English		= 0x0409	: MSG00409
)

;// Category names come first, the 'CategoryCount' registry entry
;// for the event source must match the number of entries here.
;// Also the numbering must be strictly starting by one and increase
;// by one. Careful: categories MUST be defined here, the message
;// compiler is a little bit flag and context infected. Also make
;// sure they are not longer than 10 characters, the event viewer
;// field is quite small.

MessageId = 1
SymbolicName = SMERP_CATEGORY
Language = English
Smerp
.

;// event log severity levels (severity bits)
SeverityNames = ( 
	Success		= 0x0	: STATUS_SEVERITY_SUCCESS
	Informational	= 0x1	: STATUS_SEVERITY_INFORMATIONAL
	Warning		= 0x2	: STATUS_SEVERITY_WARNING
	Error		= 0x3	: STATUS_SEVERITY_ERROR
)

;// we get a already defined message? but where is Application defined?
;// facility names
FacilityNames = (
	System		= 0x0FF
	Application	= 0xFFF
)

;// event messages from here, currently one to log a plain text field

MessageIdTypedef = DWORD

MessageId = 1
Severity = Error
Facility = Application
SymbolicName = SMERP_ERROR
Language = English
%1
.

MessageId = 1
Severity = Warning
Facility = Application
SymbolicName = SMERP_WARNING
Language = English
%1
.

MessageId = 1
Severity = Informational
Facility = Application
SymbolicName = SMERP_INFO
Language = English
%1
.
