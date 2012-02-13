#!/bin/sh

encoding=$1
type=$2

echo '<?xml version="1.0" encoding='"\"$encoding\" standalone=\"yes\""'?>'
doc=""

xmlElement()
{
	doc="$doc<$1>$2</$1>"
}
xmlOpenTag()
{
	doc="$doc<$1>"
}
xmlCloseTag()
{
	doc="$doc</$1>"
}


case _"$type" in
_employee)
	xmlOpenTag employee
	xmlElement firstname $3
	xmlElement surname $4
	xmlElement phone $5
	xmlElement email $6
	xmlElement jobtitle $7
	xmlCloseTag employee
	;;
_customer)
	xmlOpenTag customer
	xmlElement name $3
	xmlElement addresshdr $4
	xmlElement street $5
	xmlElement streetnumber $6
	xmlElement state $7
	xmlElement postalcode $8
	xmlElement country $9
	xmlCloseTag customer
	;;
_customercontact)
	xmlOpenTag customercontact
	xmlElement firstname $3
	xmlElement surname $4
	xmlElement phone $5
	xmlElement email $6
	xmlElement department $7
	xmlCloseTag customercontact
	;;
_task)
	xmlOpenTag task
	xmlElement title $3
	xmlElement key $4
	xmlElement opendate $5
	xmlElement requireddate $6
	xmlElement closedate $7
	xmlElement timebudget $8
	xmlElement status $9
	xmlCloseTag task
	;;
_timerecording)
	xmlOpenTag timerecording
	xmlElement taskkey $3
	xmlElement start $4
	xmlElement stop $5
	xmlElement employeenumber $6
	xmlCloseTag timerecording
	;;
_assignement)
	xmlOpenTag assignement
	xmlElement taskkey $3
	xmlElement issuedate $4
	xmlElement reportto $5
	xmlElement employeenumber $6
	xmlCloseTag assignement
	;;
esac

if [ `echo $encoding | grep -c '[LB]E$'` = 0 ]; then
	echo "$doc" | recode XML-standalone..$encoding
else
	echo "$doc" | recode XML-standalone..$encoding | cut -b3-
	# ... remove BOM with cut:
fi
