**
**requires:LIBXML2
**input
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E " ? > 
 < ! - -   t h i s   e x a m p l e   h a s   b e e n   c o p i e d   f r o m   h t t p : / / w w w . w 3 s c h o o l s . c o m / s c h e m a / s c h e m a _ s c h e m a . a s p   - - > 
 < n o t e   x m l n s = " h t t p : / / w w w . w 3 s c h o o l s . c o m "   x m l n s : x s i = " h t t p : / / w w w . w 3 . o r g / 2 0 0 1 / X M L S c h e m a - i n s t a n c e "   x s i : s c h e m a L o c a t i o n = " h t t p : / / w w w . w 3 s c h o o l s . c o m   n o t e . x s d " > < t o > T o v e < / t o > < f r o m > J a n i < / f r o m > < h e a d i n g > R e m i n d e r < / h e a d i n g > < b o d y > D o n ' t   f o r g e t   m e   t h i s   w e e k e n d ! < / b o d y > < / n o t e > **config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml - 
**output
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E " ? > 
 < n o t e   x m l n s = " h t t p : / / w w w . w 3 s c h o o l s . c o m "   x m l n s : x s i = " h t t p : / / w w w . w 3 . o r g / 2 0 0 1 / X M L S c h e m a - i n s t a n c e "   x s i : s c h e m a L o c a t i o n = " h t t p : / / w w w . w 3 s c h o o l s . c o m   n o t e . x s d " > < t o > T o v e < / t o > < f r o m > J a n i < / f r o m > < h e a d i n g > R e m i n d e r < / h e a d i n g > < b o d y > D o n ' t   f o r g e t   m e   t h i s   w e e k e n d ! < / b o d y > < / n o t e > 
 **end
