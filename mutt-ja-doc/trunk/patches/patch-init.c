--- init.c.orig	2010-08-26 01:31:40.000000000 +0900
+++ init.c	2012-12-18 20:37:26.000000000 +0900
@@ -2876,7 +2876,11 @@
 void mutt_init (int skip_sys_rc, LIST *commands)
 {
   struct passwd *pw;
+#if __FreeBSD_version < 500000
+  char nodename[STRING];
+#else
   struct utsname utsname;
+#endif
   char *p, buffer[STRING], error[STRING];
   int i, default_rc = 0, need_pause = 0;
   BUFFER err;
@@ -2941,16 +2945,21 @@
 #endif
 
   /* And about the host... */
+#if __FreeBSD_version < 500000
+  gethostname(nodename, sizeof (nodename));
+#else
   uname (&utsname);
+  strfcpy (nodename, utsname.nodename, sizeof (nodename));
+#endif
   /* some systems report the FQDN instead of just the hostname */
-  if ((p = strchr (utsname.nodename, '.')))
+  if ((p = strchr (nodename, '.')))
   {
-    Hostname = mutt_substrdup (utsname.nodename, p);
+    Hostname = mutt_substrdup (nodename, p);
     p++;
     strfcpy (buffer, p, sizeof (buffer)); /* save the domain for below */
   }
   else
-    Hostname = safe_strdup (utsname.nodename);
+    Hostname = safe_strdup (nodename);
 
 #ifndef DOMAIN
 #define DOMAIN buffer
