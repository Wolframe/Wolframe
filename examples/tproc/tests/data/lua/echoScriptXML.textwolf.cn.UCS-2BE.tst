--
--requires:LUA
--input
HELLO
RUN
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " y e s " ? > 
 < e m p l o y e e > < f i r s t n a m e >Y� < / f i r s t n a m e > < s u r n a m e >f < / s u r n a m e > < p h o n e > 0 2 1 4 3 6 5 8 7 < / p h o n e > < e m a i l > y a o . m i n g @ g m a i l . c o m < / e m a i l > < j o b t i t l e > s p o r t s m a n < / j o b t i t l e > < / e m p l o y e e >
.
QUIT
--file:echo.lua
function run( )
	f = filter( "xml:textwolf")
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
		}
	}
}
proc
{
	cmd run
}
--output
OK enter cmd
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " y e s " ? > 
 < e m p l o y e e > < f i r s t n a m e >Y� < / f i r s t n a m e > < s u r n a m e >f < / s u r n a m e > < p h o n e > 0 2 1 4 3 6 5 8 7 < / p h o n e > < e m a i l > y a o . m i n g @ g m a i l . c o m < / e m a i l > < j o b t i t l e > s p o r t s m a n < / j o b t i t l e > < / e m p l o y e e > 

.
OK
BYE
--end

