dovecot-salted-hashing
======================

Version 1.0 - Mon 07 Jan 2012

by Valentin Zickner
<zickner(at)querformatik(dot)de>

Important
Please use MD5-CRYPT if you only have salted passwords. This programm is only
for compatiblity. It allows md5-hashes, md5-hashes salted and md5 salted-hashes.



Usage
-----

This is a programm to use encrypted passwords for dovecot. You can use
it in combination with the TYPO3 fe_users table. Enable in TYPO3 the
salted user password hashes for frontend users and set the hashing method
to "MD5 salted hashing (secure)".

Configure this program in checkpassword.h and compile it with "make all".

Then you can configure dovecot to use this hashing programm by adding
following to your auth section:
<MTMarkdownOptions output='raw'>
  passdb checkpassword {
     args = /path/to/checkpassword
  }
</MTMarkdownOptions>
