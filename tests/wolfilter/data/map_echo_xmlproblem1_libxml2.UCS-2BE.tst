**
**requires:LIBXML2
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E " ? > < ! D O C T Y P E   d o c   S Y S T E M   " C a t e g o r y H i e r a r c h y R e q u e s t . s i m p l e f o r m " > < d o c > < c a t e g o r y   s t a t e = " "   i d = " 1 " / > < / d o c >**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 - 
**output
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   d o c   S Y S T E M   " C a t e g o r y H i e r a r c h y R e q u e s t . s i m p l e f o r m " > < d o c > < c a t e g o r y   s t a t e = " "   i d = " 1 " / > < / d o c > 
**end
