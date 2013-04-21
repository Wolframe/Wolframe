Set arg = WScript.Arguments
ZipFile = arg(0)
InputFolder = arg(1)

CreateObject( "Scripting.FileSystemObject" ).CreateTextFile( ZipFile, True ).Write "PK" & chr(5) & chr(6) & string(18,vbNullChar)

Set shellObj = CreateObject( "Shell.Application" )
Set source = shellObj.NameSpace( InputFolder ).Items

shellObj.NameSpace( ZipFile ).CopyHere( source )

wScript.Sleep 60000
