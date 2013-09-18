hinit( )
{
	rm -f /tmp/hashmap.$$.$1
}


hput( )
{
	echo "$2 $3" >> /tmp/hashmap.$$.$1
}

hget( )
{
	grep "^$2 " /tmp/hashmap.$$.$1 | awk '{ print $2 };'        
}

hdestroy( )
{
	rm -f /tmp/hashmap.$$.$1
}
