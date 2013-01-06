dovecot-salted-hashing
======================
This is a programm to use encrypted passwords for dovecot. You can use
it in combination with the TYPO3 fe_users table. Enable in TYPO3 the
salted user password hashes for frontend users and set the hashing method
to "MD5 salted hashing (secure)".

Configure this program in checkpassword.h and compile it with "make all".

Then you can configure dovecot to use this hashing programm by adding
following to your auth section:
  passdb checkpassword {
     args = /path/to/checkpassword
  }

For questions and bugs please contact me:
Valentin Zickner zickner(at)querformatik(dot)de
