--
--requires:TEXTWOLF
--requires:LUA
--input
HELLO
RUN
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " y e s " ? > 
 < e m p l o y e e > < f i r s t n a m e > S a r a < / f i r s t n a m e > < s u r n a m e > S a m p l e < / s u r n a m e > < p h o n e > 0 1 2 3 4 5 6 7 8 9 < / p h o n e > < e m a i l > s a r a . s a m p l e @ g m a i l . c o m < / e m a i l > < j o b t i t l e > e n g i n e e r < / j o b t i t l e > < / e m p l o y e e > 
.
QUIT
--file:echo.lua
function run( )
	f = provider.filter( "textwolf")
	f.empty = false

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end
--config
provider
{
	cmdhandler
	{
		lua
		{
			program echo.lua
			filter char
		}
	}
}
proc
{
	cmd run
}
--output
OK enter cmd
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " y e s " ? > 
 < e m p l o y e e > < f i r s t n a m e > S a r a < / f i r s t n a m e > < s u r n a m e > S a m p l e < / s u r n a m e > < p h o n e > 0 1 2 3 4 5 6 7 8 9 < / p h o n e > < e m a i l > s a r a . s a m p l e @ g m a i l . c o m < / e m a i l > < j o b t i t l e > e n g i n e e r < / j o b t i t l e > < / e m p l o y e e > 
 
.
OK
BYE
--end

