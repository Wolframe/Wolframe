--
--input
CMD1B
CMD1A "hi" "\'arg\'"
CMD2B 'h' '' 'u' ''
CMD3A 123
QUIT
--output
OK CMD1B ?
OK CMD1A 'hi' ''arg''
OK CMD2B 'h' '' 'u' ''
OK CMD3A '123'
OK enter cmd
BYE
--end
