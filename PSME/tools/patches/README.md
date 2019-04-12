# WARNING

Don't edit these files using editor stripping trailing spaces! It causes errors in these files!

# Naming convention

Patches should be put in the file named as "target" directory. They are automatically taken by the build system.
Only one patch is allowed for product version (other are skipped). Target directory is named exactly as the patch
(with stripped extension).

# Creating a patch

After making code corrections, patch file should be prepared. Patch prefix is up to the user.

$ diff -Naur --ignore-trailing-space ${ORG} ${ORG}_patched > ${ORG}_patched.patch

Trailing spaces should be ignored during patch creation (otherwise editing a patch file might
cause problems with these: hunks won't match anymore).